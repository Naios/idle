
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

#include <iostream>
#include <sstream>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <idle/core/context.hpp>
#include <idle/core/dep/format.hpp>
#include <idle/core/graph.hpp>
#include <idle/core/iterators.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/registry.hpp>
#include <idle/core/views/dereference.hpp>
#include <idle/interface/arguments.hpp>
#include <idle/service/commands.hpp>
#include <idle/service/sink.hpp>
#include <idle/service/visualizer.hpp>

namespace idle {
continuable<> HelpCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_dispatch(
      *this,
      [session = std::move(session), args = std::move(args)](auto&& cmd) {
        IDLE_ASSERT(cmd->registry().id() == Command::id());

        Sink& sink = session->sink();
        fmt::memory_buffer buffer;

        for (Interface const& exp : cmd->registry().interfaces()) {
          Command const& current = cast<Command>(exp);
          format_to(buffer, FMT_STRING("{}\n"), current.current_command_name());
        }

        sink.write(buffer);
      });
}

continuable<> ExitCommand::invoke(Ref<Session> session, Arguments&& args) {
  return async(wrap(root(), [args = std::move(args)](auto&& root) {
    root->stop();
  }));
}

continuable<> VersionCommand::invoke(Ref<Session> session, Arguments&& args) {
  return async([session = std::move(session), args = std::move(args)] {
    // Currently not read from anywhere
    Sink& sink = session->sink();
    sink.write("v0.1");
  });
}

continuable<> StartCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_post(
      root(),
      [session = std::move(session),
       args = std::move(args)](auto&& root) mutable -> continuable<> {
        Sink& sink = session->sink();
        fmt::memory_buffer buffer;

        Guid const guid = Guid::fromValue(args.get_uint(0));

        if (Ref<Service> const current = root->lookup(guid)) {
          format_to(buffer, FMT_STRING(">> Starting service {}"), *current);
          sink.write(buffer);

          return current->start();
        } else {
          format_to(buffer,
                    FMT_STRING("Could not find any service with guid {}"),
                    guid);
          sink.write(buffer);

          return make_ready_continuable();
        }
      });
}

continuable<> StopCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_post(
      [root = &root(), session = std::move(session),
       args = std::move(args)]() mutable -> continuable<> {
        Sink& sink = session->sink();
        fmt::memory_buffer buffer;

        Guid const guid = Guid::fromValue(args.get_uint(0));

        if (Ref<Service> current = root->lookup(guid)) {
          format_to(buffer, FMT_STRING(">> Stopping service {}"), *current);
          sink.write(buffer);

          return current->stop();
        } else {
          format_to(buffer, FMT_STRING("Could not find service with guid {}"),
                    guid);
          sink.write(buffer);

          return make_ready_continuable();
        }
      });
}

continuable<> RestartCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_post(
      [root = &root(), session = std::move(session),
       args = std::move(args)]() mutable -> continuable<> {
        Sink& sink = session->sink();
        fmt::memory_buffer buffer;

        Guid const guid = Guid::fromValue(args.get_uint(0));

        if (Ref<Service> current = root->lookup(guid)) {
          format_to(buffer, FMT_STRING(">> Restarting service {}"), *current);
          sink.write(buffer);

          return current->stop().then(current->start());
        } else {
          format_to(buffer, FMT_STRING("Could not find service with guid {}"),
                    guid);
          sink.write(buffer);

          return make_ready_continuable();
        }
      });
}

static auto spaces(std::size_t length) {
  return printable([length](std::ostream& os) {
    for (std::size_t i = 0; i < length; ++i) {
      os.put(' ');
    }
  });
}

static fmt::text_style style_of_service(Service const& current) noexcept {
  if (current.state().isStopped()) {
    return fmt::fg(fmt::terminal_color::red);
  } else if (current.state().isRunning()) {
    return fmt::fg(fmt::terminal_color::bright_cyan);
  } else if (current.state().isStopping()) {
    return fmt::fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold;
  } else {
    return fmt::fg(fmt::terminal_color::bright_yellow) | fmt::emphasis::bold;
  }
}

continuable<> LSCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_post([root = &root(),
                                         session = std::move(session),
                                         args = std::move(args)]() mutable {
    Sink& sink = session->sink();
    fmt::memory_buffer buffer;

    std::vector<Service*> frame;
    for (Service& current : transitive_services(*root)) {
      if (isa<Export>(current.parent())) {
        frame.push_back(std::addressof(current));
      }
    }

    std::sort(frame.begin(), frame.end(),
              [](Service const* left, Service const* right) {
                return left->guid().low() < right->guid().low();
              });

    for (Service& head : frame | views::dereference()) {
      format_to(buffer, // style_of_service(head),
                FMT_STRING("- [{:>4}] {} ({})\n"), head.guid().low(),
                head.name(), head.stats().state());

      for (Service& child : cluster_members(head).next()) {
        auto const level = cluster_parent_services(child).size();
        IDLE_ASSERT(level >= 2);

        format_to(buffer, // style_of_service(child),
                  FMT_STRING("        {} * {} ({})\n"), spaces((level - 2) * 2),
                  child.name(), child.stats().state());
      }
    }

    sink.write(buffer);
  });
}

static auto fill_seq(std::size_t length, StringView seq) {
  return printable([length, seq](std::ostream& os) {
    for (std::size_t i = 0; i < length; ++i) {
      os << seq;
    }
  });
}

inline auto stats_or_details(Service& current, bool details) {
  return printable([details, current = &current](std::ostream& os) {
    if (details) {
      os << current->stats().state() << " - " << current->stats().usage();
    } else {
      os << current->stats().state();
    }
  });
}

template <typename Range>
void dump_tree(Sink& sink, Range&& range, bool details) noexcept {
  fmt::memory_buffer buffer;

  for (Service& current : range) {
    auto const depth = parent_services(current).size() - 1;

    if (isa<Export>(current.parent())) {
      format_to(buffer, // style_of_service(current),
                FMT_STRING("- [{:>4}] {}{} ({})\n"), current.guid().low(),
                fill_seq(depth, "* "), current.name(),
                stats_or_details(current, details));
    } else {
      format_to(buffer, // style_of_service(current),
                FMT_STRING("         {}{} ({})\n"), fill_seq(depth, "* "),
                current.name(), stats_or_details(current, details));
    }
  }

  sink.write(buffer);
}

continuable<> TreeCommand::invoke(Ref<Session> session, Arguments&& args) {
  return root().event_loop().async_post([root = &root(),
                                         session = std::move(session),
                                         args = std::move(args)]() mutable {
    Sink& sink = session->sink();

    if ((args.size()) >= 1 && (args.get_string(0) == "post")) {
      dump_tree(sink, transitive_postorder_services(*root), false);
    } else if ((args.size()) >= 1 && (args.get_string(0) == "details")) {
      dump_tree(sink, transitive_services(*root), true);
    } else {
      dump_tree(sink, transitive_services(*root), false);
    }
  });
}

class graph_show_command_impl final : public GraphShowCommand {
public:
  using GraphShowCommand::GraphShowCommand;

  continuable<> invoke(Ref<Session> session, Arguments&& args) override {
    return async([this, session = std::move(session), args = std::move(args)] {
      using options = Visualizer::Options;
      options opt;
      opt.type = options::GraphType::dependency_graph;

      if (!args.empty()) {
        std::string const type = args.get_string(0);

        if (type == "service") {
          opt.type = options::GraphType::service_graph;
        } else if (type == "cluster") {
          opt.type = options::GraphType::cluster_graph;
        }
      }

      return visualizer_->showGraph(opt);
    });
  }

private:
  Component<Visualizer> visualizer_{*this};
};

continuable<> VerifyCommand::invoke(Ref<Session> session,
                                    Arguments&& /*args*/) {
  return root().event_loop().async_post(
      [root = &root(), session = std::move(session)] {
        Sink& sink = session->sink();

        std::stringstream ss;

        if (root->verify(ss)) {
          sink.write("> Verification finished successfully");
        } else {
          fmt::memory_buffer buffer;
          format_to(buffer, FMT_STRING("{}\n> Verification failed with errors"),
                    ss.str());
          sink.write(buffer);
        }
      });
}

Ref<GraphShowCommand> GraphShowCommand::create(Inheritance parent) {
  return spawn<graph_show_command_impl>(std::move(parent));
}
} // namespace idle
