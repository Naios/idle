
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

#ifndef IDLE_SERVICE_DETAIL_COMMAND_LINE_COMMAND_LINE_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_COMMAND_LINE_COMMAND_LINE_IMPL_HPP_INCLUDED

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/parts/thread.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/activity.hpp>
#include <idle/interface/command.hpp>
#include <idle/interface/command_processor.hpp>
#include <idle/interface/log_sink_facade.hpp>
#include <idle/interface/logger.hpp>
#include <idle/service/command_line.hpp>
#include <idle/service/store.hpp>
#include <idle/service/timer.hpp>
#include <replxx/replxx.hxx>

namespace idle {
class OStreamSaveRAII {
public:
  explicit OStreamSaveRAII(std::ostream& os)
    : os_(os)
    , buffer_(nullptr) {}

  ~OStreamSaveRAII() noexcept {
    reset();
  }

  void set(std::streambuf& buffer) {
    reset();
    buffer_ = os_.rdbuf(&buffer);
    IDLE_ASSERT(buffer_);
  }

  void reset() {
    if (buffer_) {
      os_.rdbuf(buffer_);
      buffer_ = nullptr;
    }
  }

private:
  std::ostream& os_;
  std::streambuf* buffer_;
};

// TODO Fix this streambuf so it reroutes everything buffered
//      and thread safe to the sink.
class StreambufToSink final : public std::streambuf {
public:
  explicit StreambufToSink()
    : sink_(nullptr) {}
  explicit StreambufToSink(Sink& sink)
    : sink_(&sink) {}

  std::streambuf::int_type overflow(std::streambuf::int_type value) override {
    IDLE_ASSERT(sink_);

    if (!traits_type::eq_int_type(value, traits_type::eof())) {
      auto const str = static_cast<char>(value);
      sink_->write(StringView(&str, 1));
      return !traits_type::eof();
    } else {
      return traits_type::not_eof(value);
    }
  }

  std::streamsize xsputn(char const* data, std::streamsize count) override {
    IDLE_ASSERT(sink_);
    sink_->write(StringView(data, count));
    return !traits_type::eof();
  }

  std::streambuf::int_type sync() override {
    return 0;
  }

  void reset() {
    sink_ = nullptr;
  }

private:
  Sink* sink_;
};

class CommandLineImpl final
  : public Extends<CommandLine, Thread, Logger, Command, ActivityListener>,
    public Session,
    public LogSinkFacade<CommandLineImpl>,
    public Upcastable<CommandLineImpl> {

  using Clock = std::chrono::steady_clock;

public:
  using Extends<CommandLine, Thread, Logger, Command,
                ActivityListener>::Extends;

  using LogSinkFacade<CommandLineImpl>::sink;

  Sink& sink() noexcept override {
    return sink(LogLevel::info);
  }

  void log(LogLevel level, LogMessage const& message) noexcept override;

protected:
  continuable<> onStart() override;
  continuable<> onStop() override;

  void onLaunch() override;
  void onRun() override;
  void onStopRequested() noexcept override;
  void onTeardown() noexcept;

  // bool isImmediateLogger() const noexcept override {
  //  // TODO Make this logger immediate by making the dependency to
  //  // CommandProcessor optional
  //  return true;
  //}

  void setPrompt(std::string prompt);
  void setDefaultPrompt();

  std::string command_name() const noexcept override {
    return "clear";
  }

  continuable<> invoke(Ref<Session> session, Arguments&& args) override;

  void onActivityAdd(Activity const& activity) noexcept override;
  void onActivityDel(Activity const& activity) noexcept override;
  void onActivityUpdate(Activity const& activity) noexcept override;

  void update(Clock::time_point now) noexcept;
  void schedule(Clock::duration next) noexcept;
  void debounce() noexcept;

private:
  Ref<Session> session() noexcept;

  replxx::Replxx::completions_t hook_completion(std::string const& context,
                                                int& contextLen);
  replxx::Replxx::hints_t hook_hint(std::string const& context, int& contextLen,
                                    replxx::Replxx::Color& color);
  void hook_color(std::string const& context, replxx::Replxx::colors_t& colors);

  void onCtrlCPressed();

  Dependency<CommandProcessor> command_processor_{*this};
  Dependency<Store> store_{*this};
  Component<Timer> timer_{*this};
  Dependency<Activities> activities_{*this};

  Store::ID record_;

  Lazy<replxx::Replxx> repl_;
  std::string prompt_;
  std::mutex prompt_mutex_;
  std::string default_prompt_;
  std::array<std::string, std::size_t(LogLevel::disabled)> names_;
  std::string indentation_;

  OStreamSaveRAII save_out_{std::cout};
  OStreamSaveRAII save_err_{std::cerr};
  StreambufToSink sink_out_;
  StreambufToSink sink_err_;

  Clock::duration frequency_{std::chrono::milliseconds(500)};
  Clock::time_point last_;
  bool scheduled_{false};
  std::size_t step_{0};
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_COMMAND_LINE_COMMAND_LINE_IMPL_HPP_INCLUDED
