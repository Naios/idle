
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

#include <atomic>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <idle/core/async.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/service.hpp>
#include <idle/core/use.hpp>
#include <idle/core/util/thread_name.hpp>
#include <idle/interface/io_context.hpp>

namespace idle {
class DefaultIOContext final : public Implements<IOContext> {
public:
  explicit DefaultIOContext(Inheritance parent)
    : Implements<IOContext>(std::move(parent)) {
    this->io_context_ = &default_io_context_;
  }

  continuable<> onStart() override {
    return async([this] {
      auto const count = std::min(4U, std::thread::hardware_concurrency());
      running_ = count;

      IDLE_ASSERT(!default_io_context_work_);
      default_io_context_work_.emplace(default_io_context_);

      for (std::size_t i = 0; i < count; ++i) {
        threads_.emplace_back([i, this]() mutable {
          auto const name = format(FMT_STRING("boost::io_context[{}]"), i);
          set_this_thread_name(name);

          default_io_context_.run();

          auto const previous = running_.fetch_sub(1U,
                                                   std::memory_order_acquire);
          IDLE_ASSERT(previous != 0U);
          if (previous == 1U) {
            root().event_loop().post([this] {
              for (std::thread& t : threads_) {
                if (t.joinable()) {
                  t.join();
                }
              }

              stopped_.set_value();
            });
          }
        });
      }

      IDLE_ASSERT(threads_.size() == count);
    });
  }

  continuable<> onStop() override {
    return async([this] {
             return make_continuable<void>([this](auto&& promise) mutable {
               IDLE_ASSERT(root().is_on_event_loop());
               IDLE_ASSERT(!!default_io_context_work_);
               default_io_context_work_ = nullopt;

               if (stopped_) {
                 stopped_ = split(std::move(stopped_),
                                  std::forward<decltype(promise)>(promise));
               } else {
                 // TODO Stop io_context
                 stopped_ = std::forward<decltype(promise)>(promise);
               }
             });
           })
        .then(
            [this] {
              threads_.clear();
              running_ = 0;
            },
            root().event_loop().through_dispatch());
  }

private:
  boost::asio::io_context default_io_context_;
  optional<boost::asio::io_context::work> default_io_context_work_;
  std::vector<std::thread> threads_;
  std::atomic<std::size_t> running_{0U};
  promise<> stopped_;

  IDLE_SERVICE
};

Ref<IOContext> IOContext::create(Inheritance parent) {
  return spawn<DefaultIOContext>(std::move(parent));
}

bool IOContext::can_dispatch_inplace() const noexcept {
  return io_context_->get_executor().running_in_this_thread();
}

void IOContext::queue(work work) noexcept {
  boost::asio::post(*io_context_, [work = std::move(work)]() mutable {
    std::move(work)();
  });
}
} // namespace idle
