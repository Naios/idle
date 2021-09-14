
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

#ifndef IDLE_CORE_PARTS_THREAD_HPP_INCLUDED
#define IDLE_CORE_PARTS_THREAD_HPP_INCLUDED

#include <atomic>
#include <cstdint>
#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <idle/core/api.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/lazy.hpp>

namespace idle {
/// Implements an exception safe thread on top of std::thread while exposing
/// C++20 std::jthread like facilities for simple thread setup and cancellation.
///
/// This Part is usually used to run a std::thread while a specific Service
/// is running. For this use case, Thread provides the launch() and
/// requestStop() methods that can be directly called from Service::onStart and
/// Service::onStop to sync the state of the std::thread with its Service.
class IDLE_API(idle) Thread : public Import {
  friend class ThreadImpl;

public:
  /// Represents the given thread state
  enum class ThreadState : std::uint8_t {
    stopped,
    launching,
    running,
    finished,
    stop_requested
  };

  using Import::Import;
  ~Thread() override;

  /// Launches the given thread and calls the onLaunch hook afterwards on
  /// the represented thread. On Success onRun is called from the thread.
  /// Resolves the returned promise when onLaunch has completed.
  continuable<> launch(std::string thread_name = {});

  /// Requests the thread to stop, calls the onStopRequested hook
  /// if the thread has not been stopped yet.
  continuable<> requestStop();

  /// Returns the current state of the thread
  ThreadState current_state() const noexcept {
    return state_.load(std::memory_order_acquire);
  }

  /// Similar to C++20 stop_requested
  bool isStopRequested() const noexcept {
    return current_state() == ThreadState::stop_requested;
  }

  /// Returns true when the thread is currently running
  bool isRunning() const noexcept {
    return current_state() != ThreadState::stopped;
  }

  /// Returns true when the thread has finished without requesting a stop
  bool isFinished() const noexcept {
    return current_state() == ThreadState::finished;
  }

  /// Returns the thread id of the created std::thread
  std::thread::id threadId() const noexcept {
    return handle_.load(std::memory_order_acquire);
  }

  /// Returns true when the current thread is the one represented by this class
  bool isCurrentThread() const noexcept {
    return std::this_thread::get_id() == threadId();
  }

protected:
  /// Is called once after the thread was launched and before on_run.
  ///
  /// In here you can set up things from inside the thread.
  ///
  /// \note Exceptions thrown from on_run are propagated to the promise
  ///       that is returned from launch, the thread is stopped immediately
  ///       in that case.
  virtual void onLaunch();

  /// Is called once when the thread is spawned
  ///
  /// \note Exceptions thrown from on_run are propagated to the promise
  ///       that is returned from request_stop.
  virtual void onRun();

  /// Is called once when the thread is requested to be stopped
  ///
  /// \attention If the thread returns from itself (is_finished returns true),
  ///            before request_stop is called this method is never invoked!
  virtual void onStopRequested() noexcept;

private:
  std::atomic<ThreadState> state_{ThreadState::stopped};
  std::atomic<std::thread::id> handle_;
  std::exception_ptr exception_;
  Lazy<std::thread> thread_;
  std::mutex mutex_;
  promise<> promise_;

  IDLE_PART
};
} // namespace idle

#endif // IDLE_CORE_PARTS_THREAD_HPP_INCLUDED
