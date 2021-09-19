
/*
 *   _____    _ _        .      .    .
 *  |_   _|  | | |  .       .           .
 *    | |  __| | | ___         .    .        .
 *    | | / _` | |/ _ \                .
 *   _| || (_| | |  __/ github.com/Naios/idle
 *  |_____\__,_|_|\___| AGPL v3 (Early Access)
 *
 * Copyright(c) 2018 - 2021 Denis Blank <denis.blank at outlook dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IDLE_SERVICE_DETAIL_PROCESS_GROUP_ASYNC_PROCESS_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PROCESS_GROUP_ASYNC_PROCESS_HPP_INCLUDED

#include <atomic>
#include <exception>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/service/process_group.hpp>

namespace boost {
namespace asio {
class io_context;
}
} // namespace boost

namespace idle {
namespace detail {
std::exception_ptr make_bad_exit_code_exception_ptr(std::error_code ec,
                                                    int exit_code);

std::exception_ptr make_error_code_exception(std::error_code ec);

template <typename Parent, typename Child>
class ProcessFrame : public Child, public Upcastable<Parent> {

  class CompletionHandler : public boost::process::extend::async_handler {
    Ref<ProcessFrame> frame_;

  public:
    explicit CompletionHandler(Ref<ProcessFrame> frame)
      : frame_(std::move(frame)) {
      IDLE_ASSERT(frame_);
    }

    template <typename Executor>
    auto on_exit_handler(Executor&) {
      return [frame = std::move(frame_)](int exit_code,
                                         std::error_code const& ec) mutable {
        ProcessFrame& current = *frame;
        boost::asio::post(current.strand(), [frame = std::move(frame),
                                             exit_code, ec]() mutable {
          IDLE_DETAIL_LOG_TRACE(
              "Process frame finished with exit code {} (\"{}\")", exit_code,
              ec.message());

          frame->on_exit(exit_code, ec);
          frame.reset();
        });
      };
    }
  };

public:
  explicit ProcessFrame(boost::asio::io_context::strand& strand,
                        promise<Ref<Child>> promise,
                        bool throw_on_bad_exit_code)
    : strand_(strand)
    , promise_(std::move(promise))
    , throw_on_bad_exit_code_(throw_on_bad_exit_code)
#ifndef NDEBUG
    , called_(false)
#endif
  {
  }

  ~ProcessFrame() {
    IDLE_DETAIL_LOG_TRACE("Destroy a process frame");
  }

  template <typename... T>
  void async_spawn(T&&... args) {
    try {
      // Spawn the asynchronous child process here with additional arguments
      // received from the parent and from the arguments.
      static_cast<Parent*>(this)->onSetup([&](auto&&... additional) {
        child_ = boost::process::child(std::forward<T>(args)...,
                                       CompletionHandler{refOf(this)},
                                       std::forward<decltype(additional)>(
                                           additional)...);
      });
    } catch (boost::process::process_error const& exception) {
#ifndef NDEBUG
      IDLE_ASSERT(!called_);
      called_ = true;
#endif

      promise_->set_exception(std::make_exception_ptr(exception));
      static_cast<Parent*>(this)->onCleanup();
      return;
    }

    static_cast<Parent*>(this)->onSpawned();
  }

  boost::asio::io_context::strand& strand() {
    return strand_;
  }

private:
  void on_exit(int exit_code, std::error_code const& ec) {
#ifndef NDEBUG
    IDLE_ASSERT(!called_);
    called_ = true;
#endif

    if (!ec && (exit_code != 0) && throw_on_bad_exit_code_) {
      // Throw an exception on bad exit codes
      promise_->set_exception(make_bad_exit_code_exception_ptr(ec, exit_code));
    } else {
      if (ec) {
        std::move(*promise_).set_exception(
            make_error_code_exception(std::move(ec)));
      } else {
        static_cast<Parent*>(this)->onFinished(exit_code);

        std::move(*promise_).set_value(refOf(this));
      }
    }

    static_cast<Parent*>(this)->onCleanup();
    promise_.reset();

    child_ = {};
  }

  boost::asio::io_context::strand& strand_;
  optional<promise<Ref<Child>>> promise_;
  bool const throw_on_bad_exit_code_;
  boost::process::child child_;

#ifndef NDEBUG
  std::atomic<bool> called_;
#endif
};

namespace detail {
template <typename Callable, typename... Args>
void append_env(SpawnOptions& options, Callable&& callable, Args&&... args) {
  if (!options.inherit_env && !options.env_var) {
    std::forward<Callable>(callable)(std::forward<Args>(args)...);
    return;
  }

  boost::process::environment child_env = [&]() -> boost::process::environment {
    if (options.inherit_env) {
      return boost::this_process::environment();
    } else {
      return {};
    }
  }();

  if (options.env_var) {
    SpawnOptions::Environment& env = *options.env_var;
    for (auto&& entry : env) {
      child_env[entry.first] = std::move(entry.second);
    }
  }

  std::forward<Callable>(callable)(std::forward<Args>(args)..., child_env);
}

template <typename Callable, typename... Args>
void append_working_dir(SpawnOptions& options, Callable&& callable,
                        Args&&... args) {
  if (options.working_dir) {
    std::forward<Callable>(
        callable)(std::forward<Args>(args)...,
                  boost::process::start_dir(*options.working_dir));
  } else {
    std::forward<Callable>(callable)(std::forward<Args>(args)...);
  }
}
} // namespace detail

template <typename Callable>
void append_options(boost::asio::io_context& context,
                    boost::process::group& group, std::string&& executable,
                    std::vector<std::string>&& arguments,
                    SpawnOptions&& options, Callable&& callable) {
  detail::append_env(options, [&callable, &options, &executable, &arguments,
                               &context, &group](auto&&... args) mutable {
    detail::append_working_dir(
        options,
        [&callable, &executable, &arguments, &context,
         &group](auto&&... args) mutable {
          if (executable.empty()) {
            std::forward<Callable>(
                callable)(boost::process::shell,
                          boost::process::args(std::move(arguments)), context,
                          group, std::forward<decltype(args)>(args)...);
          } else {
            std::forward<Callable>(
                callable)(std::move(executable),
                          boost::process::args(std::move(arguments)), context,
                          group, std::forward<decltype(args)>(args)...);
          }
        },
        std::forward<decltype(args)>(args)...);
  });
}
} // namespace detail
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PROCESS_GROUP_ASYNC_PROCESS_HPP_INCLUDED
