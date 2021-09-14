
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

#define SPDLOG_LEVEL_NAMES                                                     \
  { "trace", "debug", "info", "warn", "error", "fatal", "off" }

#include <memory>
#include <mutex>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/parts/dependency_list.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/functional.hpp>
#include <idle/core/util/thread_name.hpp>
#include <idle/interface/log_sink_facade.hpp>
#include <idle/interface/logger.hpp>
#include <idle/service/sink.hpp>
#include <spdlog/async_logger.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace idle {
Logger::~Logger() {}

class LogImpl final : public Implements<Log>,
                      public LogSinkFacade<LogImpl>,
                      public Upcastable<LogImpl> {
public:
  using Implements<Log>::Implements;

  void logImpl(LogLevel level, LogMessage const& message) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);

    for (Logger& current : logger_) {
      if (current.is_enabled(level)) {
        current.log(level, message);
      }
    }
  }

  UseReply onUseOffer(Logger& dep) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);

    if (dep.owner().state().isRunning()) {
      logger_.insert(dep);
      return UseReply::UseLater;
    } else {
      return UseReply::UseLaterAndNotify;
    }
  }

  ReleaseReply onReleaseRequest(Logger& dep) noexcept {
    std::lock_guard<std::mutex> lock(mutex_);

    logger_.erase(dep);
    return ReleaseReply::Now;
  }

  BitSet<DependenciesFlags> onInspect(Logger const& dep) noexcept {
    if (dep.isImmediateLogger()) {
      return {};
    } else {
      return {DependenciesFlags::Weak};
    }
  }

  void name(std::ostream& os) const override;

private:
  // TODO The dependencies must be defined non weak such that an issues log
  // message is delivered to all registered loggers for sure (not to drop any
  // message). Maybe this could be solved by marking a logger immideate or not.
  DynDependencyList<Logger, LogImpl> logger_{
      *this, //
      IDLE_STATIC_BIND(&LogImpl::onReleaseRequest),
      IDLE_STATIC_BIND(&LogImpl::onUseOffer),
      IDLE_STATIC_BIND(&LogImpl::onInspect)};

  std::mutex mutex_;
};

void LogImpl::name(std::ostream& os) const {
  os << "idle::Log";
}

void Log::log(LogLevel level, LogMessage const& message) noexcept {
  LogImpl::from(this)->logImpl(level, message);
}

Sink& Log::sink(LogLevel level) noexcept {
  return static_cast<LogSinkFacade<LogImpl>*>(LogImpl::from(this))->sink(level);
}

Ref<Log> Log::create(Inheritance parent) {
  return spawn<LogImpl>(std::move(parent));
}

class thread_name_formatter_flag : public spdlog::custom_flag_formatter {
public:
  void format(const spdlog::details::log_msg&, const std::tm&,
              spdlog::memory_buf_t& dest) override {

    if (StringView name = this_thread_name()) {
      dest.append(name.data(), name.data() + name.size());
    } else {
      fmt::format_to(dest, FMT_STRING("{}"), std::this_thread::get_id());
    }
  }

  std::unique_ptr<custom_flag_formatter> clone() const override {
    return spdlog::details::make_unique<thread_name_formatter_flag>();
  }
};

static std::unique_ptr<spdlog::pattern_formatter>
create_default_spdlog_formatter() {
  auto formatter = std::make_unique<spdlog::pattern_formatter>();
  formatter->add_flag<thread_name_formatter_flag>('N');
  return formatter;
}

static std::string create_default_spdlog_pattern() {
  return "[%H:%M:%S %z @%4N] %^%-4l%$: %v";
}

/// This sink routes the log message to cout or cerr based on the loglevel
class console_sink : public spdlog::sinks::sink {
public:
  console_sink()
    : out_sink_(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())
    , err_sink_(std::make_shared<spdlog::sinks::stderr_color_sink_mt>()) {}

  void log(spdlog::details::log_msg const& msg) override {
    if (msg.level < spdlog::level::err) {
      out_sink_->log(msg);
    } else {
      err_sink_->log(msg);
    }
  }

  void set_pattern(std::string const& pattern) override {
    out_sink_->set_pattern(pattern);
    err_sink_->set_pattern(pattern);
  }

  void
  set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {
    out_sink_->set_formatter(sink_formatter->clone());
    err_sink_->set_formatter(std::move(sink_formatter));
  }

  void flush() override {
    out_sink_->flush();
    err_sink_->flush();
  }

  spdlog::sink_ptr out_sink_;
  spdlog::sink_ptr err_sink_;
};

/*class default_log_sink_impl final : public FileLogger {
public:
  using FileLogger::DefaultLogSink;

  continuable<> onStart() override {
    return root().event_loop().async_dispatch(
        this, [](Ref<default_log_sink_impl> me) mutable {
          me->init_log();
        });
  }

  continuable<> onStop() override {
    return root().event_loop().async_dispatch(
        this, [](Ref<default_log_sink_impl> me) mutable {
          me->deinit_log();
        });
  }

  void init_log() {
    std::vector<spdlog::sink_ptr> sinks;

    try {
      auto file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
          "logs/log.txt", 23, 59);
      sinks.push_back(std::move(file_sink_));
    } catch (spdlog::spdlog_ex&) {
      // Ignore if we couldn't open the file for writing
    }

    sinks.push_back(std::make_shared<console_sink>());

    // TODO Implement async logging
    //auto tp = std::make_shared<spdlog::details::thread_pool>(8000, 1);
    //auto logger = std::make_shared<spdlog::async_logger>(
    //"default_logger", begin(sinks), end(sinks), tp,
    //spdlog::async_overflow_policy::block);

    logger_ = std::make_shared<spdlog::logger>("default_logger", begin(sinks),
                                               end(sinks));

    auto formatter = create_default_spdlog_formatter();
    formatter->set_pattern(create_default_spdlog_pattern());
    logger_->set_formatter(std::move(formatter));
    logger_->set_level(spdlog::level::level_enum::trace);
  }

  void deinit_log() {
    logger_->flush();
    logger_.reset();
  }

  void log(LogLevel level, LogMessage const& message) noexcept override {
    spdlog::source_loc loc{
        message.location.file_name().data(),
        int(message.location.line()),
        message.location.function_name().data(),
    };

    logger_->log(loc, static_cast<spdlog::level::level_enum>(level),
                 message.message);
  }

private:
  std::shared_ptr<spdlog::logger> logger_;
};*/

/*
ConsoleLogger::~ConsoleLogger() {}

void ConsoleLogger::log(LogLevel level, LogMessage const& message) noexcept {}

Ref<ConsoleLogger> ConsoleLogger::create(Inheritance parent) {
  return spawn<DefaultService<ConsoleLogger>>(std::move(parent));
}*/

/*Ref<FileLogger> FileLogger::create(Inheritance parent) {
  return spawn<default_log_sink_impl>(std::move(parent));
}*/
} // namespace idle
