
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

#ifndef IDLE_SERVICE_DETAIL_PROCESS_GROUP_PROCESS_GROUP_IMPL_HPP_INCLUDED
#define IDLE_SERVICE_DETAIL_PROCESS_GROUP_PROCESS_GROUP_IMPL_HPP_INCLUDED

#include <string>
#include <utility>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/process/group.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/dep/function.hpp>
#include <idle/core/dep/optional.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/ref.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/service.hpp>
#include <idle/core/support.hpp>
#include <idle/core/util/lazy.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/io_context.hpp>
#include <idle/service/detail/process_group/async_process.hpp>
#include <idle/service/process_group.hpp>

namespace idle {
class ProcessResultDataImpl
  : public detail::ProcessFrame<ProcessResultDataImpl, ProcessResultData> {

public:
  using ProcessFrame<ProcessResultDataImpl, ProcessResultData>::ProcessFrame;

  int exit_code() const noexcept override {
    return exit_code_;
  }

  template <typename Spawner>
  void onSetup(Spawner&& spawn) {
    spawn();
  }

  void onSpawned() {
    // -
  }

  void onFinished(int exit_code) {
    exit_code_ = exit_code;
  }

  void onCleanup() {
    // -
  }

private:
  int exit_code_{EXIT_FAILURE};
};

class BufferedProcessResultDataImpl
  : public detail::ProcessFrame<BufferedProcessResultDataImpl,
                                BufferedProcessResultData> {

public:
  explicit BufferedProcessResultDataImpl(
      boost::asio::io_context::strand& strand,
      promise<Ref<BufferedProcessResultData>> promise,
      bool throw_on_bad_exit_code)
    : ProcessFrame<BufferedProcessResultDataImpl, BufferedProcessResultData>(
          strand, std::move(promise), throw_on_bad_exit_code)
    , out_(strand.context())
    , err_(strand.context()) {}

  int exit_code() const noexcept override {
    return exit_code_;
  }

  template <typename Spawner>
  void onSetup(Spawner&& spawn) {
    using namespace boost::process;

    spawn(std_out > out_.pipe, std_err > err_.pipe);
  }

  void onSpawned() {
    async_read(out_);
    async_read(err_);
  }

  void onFinished(int exit_code) {
    exit_code_ = exit_code;
  }

  void onCleanup() {
    out_.pipe.close();
    err_.pipe.close();
  }

  StringView outImpl() const noexcept {
    return out_.view();
  }

  StringView errImpl() const noexcept {
    return err_.view();
  }

private:
  int exit_code_{EXIT_FAILURE};

  struct Stream {
    explicit Stream(boost::asio::io_context& context)
      : pipe(context) {
      IDLE_ASSERT(pipe.is_open());
      IDLE_ASSERT(pipe.native_sink());
      IDLE_ASSERT(pipe.native_source());
    }

    StringView view() const noexcept {
      return StringView(&*buffers_begin(buffer.data()), buffer.size());
    }

    boost::process::async_pipe pipe;
    boost::asio::streambuf buffer;
  };

  void async_read(Stream& stream) {
    boost::asio::async_read(
        stream.pipe, stream.buffer,
        [ref = refOf(*this)](boost::system::error_code const&, std::size_t) {
          // We need to keep a reference to this object
          // until the async read has completed.
        });
  }

  Stream out_;
  Stream err_;
};

class StreamedProcessResultDataImpl
  : public detail::ProcessFrame<StreamedProcessResultDataImpl,
                                ProcessResultData> {

public:
  explicit StreamedProcessResultDataImpl(
      boost::asio::io_context::strand& strand,
      promise<Ref<ProcessResultData>> promise, bool throw_on_bad_exit_code,
      StreamingOptions&& options)
    : ProcessFrame<StreamedProcessResultDataImpl, ProcessResultData>(
          strand, std::move(promise), throw_on_bad_exit_code)
    , options_(std::move(options)) {

    if (options_.out) {
      out_.emplace(strand, *options_.out);
    }

    if (options_.err) {
      err_.emplace(strand, *options_.err);
    }
  }

  int exit_code() const noexcept override {
    return exit_code_;
  }

  template <typename Spawner>
  void onSetup(Spawner&& spawn) {
    using namespace boost::process;

    if (out_) {
      if (err_) {
        spawn(std_out > out_->pipe, std_err > err_->pipe);
      } else {
        spawn(std_out > out_->pipe, std_err > boost::process::close);
      }
    } else if (err_) {
      spawn(std_out > boost::process::close, std_err > err_->pipe);
    } else {
      spawn(std_out > boost::process::close, std_err > boost::process::close);
    }
  }

  void onSpawned() {
    if (out_) {
      async_read_line(refOf(*this).pin(*out_));
    }

    if (err_) {
      async_read_line(refOf(*this).pin(*err_));
    }
  }

  void onFinished(int exit_code) {
    exit_code_ = exit_code;
  }

  void onCleanup() {
    if (out_) {
      out_->close();
    }

    if (err_) {
      err_->close();
    }
  }

private:
  int exit_code_{EXIT_FAILURE};

  struct Stream {
    explicit Stream(boost::asio::io_context::strand& strand_, Sink& sink_)
      : strand(strand_)
      , pipe(strand_.context())
      , sink(sink_) {

      IDLE_ASSERT(pipe.is_open());
      IDLE_ASSERT(pipe.native_sink());
      IDLE_ASSERT(pipe.native_source());
    }

    ~Stream() {
      IDLE_ASSERT(!pipe.is_open());
    }

    void close() {
      pipe.close();
    }

    bool is_open() const noexcept {
      return pipe.is_open();
    }

    boost::asio::io_context::strand& strand;
    boost::process::async_pipe pipe;
    boost::asio::streambuf buffer;
    Sink& sink;
  };

  static constexpr char delimiter = '\n';
  static constexpr std::size_t delimiter_length = 1;

  static void async_read_line(Ref<Stream>&& stream) {
    Stream& current = *stream;

    boost::asio::async_read_until(
        current.pipe, current.buffer, delimiter,
        [stream = std::move(stream)](boost::system::error_code const& ec,
                                     std::size_t length) mutable {
          IDLE_ASSERT(length != 0 || ec);

          if (ec) {
            if (ec != boost::asio::error::broken_pipe) {
              IDLE_DETAIL_LOG_ERROR(
                  "async_read_line: Size: {}, Value: {}, Message: '{}'", length,
                  ec.value(), ec.message());
            }
          } else {
            IDLE_ASSERT(length);

            if (length > delimiter_length) {
              stream->sink.write(
                  StringView(&*buffers_begin(stream->buffer.data()),
                             length - delimiter_length));
            }

            stream->buffer.consume(length);

            Stream& current = *stream;
            boost::asio::post(current.strand,
                              [stream = std::move(stream)]() mutable {
                                if (stream->is_open()) {
                                  async_read_line(std::move(stream));
                                }
                              });
          }
        });
  }

  StreamingOptions const options_;
  Lazy<Stream> out_;
  Lazy<Stream> err_;
};

class ProcessGroupImpl final : public ProcessGroup,
                               public Upcastable<ProcessGroupImpl> {

  template <typename, typename>
  friend struct spawn_trait;

public:
  explicit ProcessGroupImpl(Inheritance parent)
    : ProcessGroup(std::move(parent)) {}
  ~ProcessGroupImpl() noexcept;

  continuable<ProcessResult> spawn_impl(std::string&& executable,
                                        std::vector<std::string>&& arguments,
                                        SpawnOptions&& options);
  continuable<BufferedProcessResult>
  spawn_buffered_impl(std::string&& executable,
                      std::vector<std::string>&& arguments,
                      SpawnOptions&& options);

  continuable<ProcessResult>
  spawn_streamed_impl(std::string&& executable,
                      std::vector<std::string>&& arguments,
                      SpawnOptions&& options, StreamingOptions&& stream);

  continuable<> open_impl(std::string&& file_path, SpawnOptions&& options);

  /*
  continuable<ProcessResult>
  spawn_inplace_impl(std::string&& executable,
                     std::vector<std::string>&& arguments,
                     SpawnOptions&& options);

  continuable<ProcessResult>
  spawn_streamed_impl(std::string&& executable,
                      std::vector<std::string>&& arguments,
                      SpawnOptions&& options, process_sink_t sink);
                      */

  boost::process::group& group() noexcept {
    IDLE_ASSERT(state().isRunning());
    return *group_;
  }

protected:
  continuable<> onStart() override;
  continuable<> onStop() override;

private:
  Dependency<IOContext> io_context_{*this};
  Lazy<boost::asio::io_context::strand> strand_;
  Lazy<boost::process::group> group_;
};
} // namespace idle

#endif // IDLE_SERVICE_DETAIL_PROCESS_GROUP_PROCESS_GROUP_IMPL_HPP_INCLUDED
