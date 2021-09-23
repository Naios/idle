
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

#ifdef _MSC_VER
#  pragma warning(disable : 4456)
#  pragma warning(disable : 4459)
#  pragma warning(disable : 4996)
#  pragma warning(disable : 4458)
#endif // _MSC_VER

#include <cstdio>
#include <istream>
#include <tuple>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/strand.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <idle/core/api.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/detail/exception_base.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/detail/open_file.hpp>
#include <idle/core/detail/traits.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/service/detail/process_group/async_process.hpp>
#include <idle/service/detail/process_group/process_group_impl.hpp>
#include <idle/service/process_group.hpp>

namespace idle {
template <typename Impl, typename Interface>
struct spawn_trait {
  using result = Ref<Interface>;

  template <typename... Args>
  static continuable<result>
  spawn_from(ProcessGroupImpl& group, std::string&& executable,
             std::vector<std::string>&& arguments, SpawnOptions&& options,
             Args&&... construct) {
    return make_continuable<result>(
        [weak = weakOf(group), executable = std::move(executable),
         arguments = std::move(arguments), options = std::move(options),
         construct = std::make_tuple(std::forward<Args>(construct)...)](
            auto&& promise) mutable {
          if (Ref<ProcessGroupImpl> me = weak.lock()) {
            boost::asio::post(
                *me->strand_,
                [me, executable = std::move(executable),
                 arguments = std::move(arguments), options = std::move(options),
                 construct = std::move(construct),
                 promise = std::forward<decltype(promise)>(promise)]() mutable {
                  IDLE_DETAIL_LOG_DEBUG("Launching process: '{}' '{}'",
                                        executable,
                                        fmt::join(arguments, "' '"));

                  // Create the process frame which is joined with the result
                  // object such that we only use one allocation for every
                  // spawn.
                  Ref<Impl> frame = detail::unpack(
                      [&](auto&&... args) {
                        return make_ref<Impl>(*me->strand_, std::move(promise),
                                              options.throw_on_bad_exit_code,
                                              std::forward<decltype(args)>(
                                                  args)...);
                      },
                      std::move(construct));

                  // Append the spawn_options process args to the async_spawn
                  detail::append_options(
                      me->io_context_->get(), me->group(),
                      std::move(executable), std::move(arguments),
                      std::move(options), [&frame](auto&&... args) {
                        frame->async_spawn(
                            std::forward<decltype(args)>(args)...);
                      });
                });
          } else {
            promise.set_canceled();
          }
        });
  }
};

ProcessGroupImpl::~ProcessGroupImpl() noexcept {
  IDLE_ASSERT(!group_);
  IDLE_ASSERT(!strand_);
}

continuable<ProcessResult>
ProcessGroupImpl::spawn_impl(std::string&& executable,
                             std::vector<std::string>&& arguments,
                             SpawnOptions&& options) {
  using trait = spawn_trait<ProcessResultDataImpl, ProcessResultData>;
  return trait::spawn_from(*this, std::move(executable), std::move(arguments),
                           std::move(options));
}

continuable<BufferedProcessResult>
ProcessGroupImpl::spawn_buffered_impl(std::string&& executable,
                                      std::vector<std::string>&& arguments,
                                      SpawnOptions&& options) {
  using trait = spawn_trait<BufferedProcessResultDataImpl,
                            BufferedProcessResultData>;
  return trait::spawn_from(*this, std::move(executable), std::move(arguments),
                           std::move(options));
}

continuable<ProcessResult> ProcessGroupImpl::spawn_streamed_impl(
    std::string&& executable, std::vector<std::string>&& arguments,
    SpawnOptions&& options, StreamingOptions&& stream) {

  using trait = spawn_trait<StreamedProcessResultDataImpl, ProcessResultData>;
  return trait::spawn_from(*this, std::move(executable), std::move(arguments),
                           std::move(options), std::move(stream));
}

continuable<> ProcessGroupImpl::open_impl(std::string&& file_path,
                                          SpawnOptions&&) {
  return async([file_path]() mutable {
    IDLE_DETAIL_LOG_DEBUG("Shell executing: \'{}\'", file_path);
    auto const abs = boost::filesystem::absolute(std::move(file_path));

    detail::open_file(abs.generic_string().c_str());
  });
}

continuable<> ProcessGroupImpl::onStart() {
  return async([this] {
    IDLE_ASSERT(!group_);

    group_.emplace();
    strand_.emplace(io_context_->get());

    IDLE_ASSERT(group_);
    IDLE_ASSERT(strand_);
  });
}

continuable<> ProcessGroupImpl::onStop() {
  return async([this] {
    std::error_code ec;

    group_->terminate(ec);
    if (ec) {
      IDLE_DETAIL_LOG_ERROR("ProcessGroupImpl terminate failed with '{}'!",
                            ec.message());

      ec = {};
    }

    // TODO Use an async wait without blocking the event loop!
    group_->wait(ec);
    if (ec) {
      IDLE_DETAIL_LOG_ERROR("ProcessGroupImpl wait failed with '{}'!",
                            ec.message());

      ec = {};
    }

    group_.reset();
    IDLE_ASSERT(!group_);

    IDLE_ASSERT(strand_);
    strand_.reset();
  });
}
} // namespace idle
