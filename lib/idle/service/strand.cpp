
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

#include <boost/asio/defer.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/strand.hpp>
#include <idle/core/context.hpp>
#include <idle/core/detail/exception_handling.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/util/lazy.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/service/strand.hpp>

namespace idle {
class StrandImpl : public Strand, public Upcastable<StrandImpl> {
public:
  using Strand::Strand;

  void queue_impl(work&& work) noexcept {
    boost::asio::dispatch(*strand_, [work = std::move(work),
                                     actor = weakOf(this)]() mutable {
      if (auto ref = actor.lock()) {
        (void)ref;
        work.set_value();
      } else {
        std::move(work).set_canceled();
      }
    });
  }

protected:
  continuable<> onStart() override {
    return root().event_loop().async_post(*this, [](auto&& me) {
      me->strand_.emplace(me->io_context_->get());
    });
  }

  continuable<> onStop() override {
    return root().event_loop().async_post(*this, [](auto&& me) {
      me->strand_.reset();
    });
  }

private:
  Dependency<IOContext> io_context_{*this};
  Lazy<boost::asio::io_context::strand> strand_;
};

bool Strand::can_dispatch_inplace() const noexcept {
  return false;
}

void Strand::queue(work&& work) noexcept {
  StrandImpl::from(this)->queue_impl(std::move(work));
}

Ref<Strand> Strand::create(Inheritance parent) {
  return spawn<StrandImpl>(std::move(parent));
}
} // namespace idle
