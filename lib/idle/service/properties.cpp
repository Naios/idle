
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
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>
#include <idle/core/casting.hpp>
#include <idle/core/context.hpp>
#include <idle/core/dep/continuable.hpp>
#include <idle/core/detail/log.hpp>
#include <idle/core/parts/collection.hpp>
#include <idle/core/parts/component.hpp>
#include <idle/core/parts/dependency.hpp>
#include <idle/core/util/assert.hpp>
#include <idle/core/util/upcastable.hpp>
#include <idle/interface/logger.hpp>
#include <idle/service/art/reflection.hpp>
#include <idle/service/detail/default_paths.hpp>
#include <idle/service/external/toml11/serialize.hpp>
#include <idle/service/file_watcher.hpp>
#include <idle/service/properties.hpp>
#include <idle/service/timer.hpp>
#include <idle/service/var.hpp>
#include <toml11/toml.hpp>

using boost::filesystem::path;

namespace idle {
class ReloadablePropertiesImpl;

class Debouncer final : public Service {
public:
  using Duration = Timer::Duration;
  using TimePoint = Timer::TimePoint;

  using Service::Service;

  void setup(Duration debounce_duration) {
    debounce_duration_ = debounce_duration;
  }

  void bounce() {
    TimePoint const next = std::chrono::steady_clock::now() +
                           debounce_duration_;
    TimePoint previous = next_.load(std::memory_order_relaxed);

    if (next <= previous) {
      return;
    }

    while (!next_.compare_exchange_weak(previous, next,
                                        std::memory_order_release,
                                        std::memory_order_relaxed)) {
      if (next <= previous) {
        return;
      }
    }
  }

  continuable<> debounce() {
    return async(weakly(handleOf(*this), [](auto&& me) -> continuable<> {
      me->bounce();
      return me->waitUntilDebounce();
    }));
  }

private:
  continuable<> waitUntilDebounce() {
    return async(weakly(handleOf(*this), [](auto&& me) -> continuable<> {
      TimePoint const next = me->next_.load(std::memory_order_acquire) +
                             me->debounce_duration_;
      TimePoint const now = std::chrono::steady_clock::now();

      if (next <= now) {
        return make_ready_continuable();
      } else {
        return me->timer_->waitUntil(next).then(me->waitUntilDebounce());
      }
    }));
  }

  Component<Timer> timer_{*this};
  Duration debounce_duration_{std::chrono::milliseconds(50)};
  std::atomic<TimePoint> next_;
};

static void consume_shebang(std::istream& is) {
  auto const current = is.tellg();

  std::string line;
  std::getline(is, line);

  if (!StringView(line).starts_with("#!")) {
    is.seekg(current, std::ios_base::beg);
  }
}

static StringView config_shebang() {
  return "#!/usr/bin/env idle";
}

// For now this is specialized for toml, but it could be abstracted to
// support other formats such as json, yaml or ini.
class TOMLPropertiesSource final : public PropertiesSource {
  using Value = toml::basic_value<toml::preserve_comments>;

public:
  TOMLPropertiesSource() = default;

  void read(std::istream& is, StringView file_name) override {
    consume_shebang(is);

    value_ = toml::parse<toml::preserve_comments>(is,
                                                  std::string(file_name.begin(),
                                                              file_name.end()));
  }

  void write(std::ostream& os, StringView file_name) override {
    (void)file_name;

    std::ostringstream ss;
    ss << toml::format(value_);
    auto str = ss.str();
    boost::algorithm::trim(str);

    os << config_shebang() << '\n' << str << std::endl;
  }

  bool serialize(ConstReflectionPtr ptr, StringView key,
                 Sink& sink) noexcept override {
    (void)sink;

    toml_serialize(advance_scope(value_, key), ptr);
    return true;
  }
  bool deserialize(ReflectionPtr ptr, StringView key,
                   Sink& sink) const noexcept override {
    if (Value const* scope = advance_scope(value_, key)) {
      toml_deserialize(*scope, ptr, sink);
      return true;
    } else {
      return false;
    }
  }

  Ref<PropertiesSource> clone() const override {
    return make_ref<TOMLPropertiesSource>(*this);
  }

  Ref<PropertiesSource> create() const override {
    return make_ref<TOMLPropertiesSource>();
  }

  bool equals(PropertiesSource const& other,
              StringView key) const noexcept override {
    auto const& real = static_cast<TOMLPropertiesSource const&>(other);

    Value const* left = advance_scope(value_, key);
    Value const* right = advance_scope(real.value_, key);

    if (left) {
      if (right) {
        return *left == *right;
      } else {
        return false;
      }
    } else {
      return !right;
    }
  }

private:
  static Value& advance_scope(Value& value, StringView scope) noexcept {
    Value* itr = &value;

    while (StringView current = scope.split('.')) {
      std::string const str(current.begin(), current.end());
      itr = &(*itr)[str];

      if (!itr->is_table()) {
        *itr = toml::table{};
      }
    }

    return *itr;
  }
  static Value const* advance_scope(Value const& value,
                                    StringView scope) noexcept {
    Value const* itr = &value;

    while (StringView current = scope.split('.')) {
      std::string const str(current.begin(), current.end());

      if (itr->is_table() && itr->contains(str)) {
        itr = &(itr->at(str));
      } else {
        return nullptr;
      }
    }

    return itr;
  }

  Value value_;
};

class ReloablePropertiesGeneration final : public Implements<Properties> {
public:
  explicit ReloablePropertiesGeneration(Inheritance parent,
                                        Ref<PropertiesSource const> data,
                                        Ref<PropertiesSource const> previous,
                                        Generation generation)
    : Implements<Properties>(std::move(parent), generation)
    , data_(std::move(data))
    , previous_(std::move(previous)) {
    IDLE_ASSERT(data_);
  }

  ReloadablePropertiesImpl& parent() noexcept;
  ReloadablePropertiesImpl const& parent() const noexcept;

  bool get(ReflectionPtr ptr, StringView key) const noexcept override;

  void set(ConstReflectionPtr ptr, StringView key) noexcept override;

  bool changed(StringView key) const noexcept override;

private:
  Ref<PropertiesSource const> data_;
  Ref<PropertiesSource const> previous_;

  IDLE_SERVICE
};

class ConfigReadException : public std::exception {

public:
  ConfigReadException() = default;

  char const* what() const noexcept override {
    return "failed to read the config!";
  }
};

class ReloadablePropertiesImpl final
  : public Extends<ReloadableProperties, Collection>,
    public Upcastable<ReloadablePropertiesImpl> {

  using Source = TOMLPropertiesSource;

public:
  using Extends<ReloadableProperties, Collection>::Extends;

  void setupImpl(Config config) {
    config_ = std::move(config);
  }

  void push(ConstReflectionPtr ptr, StringView key) {
    IDLE_ASSERT(root().is_on_event_loop());

    if (current_->serialize(ptr, key, sink())) {
      debouncer_->bounce();

      if (!changes_) {
        debouncer_->debounce().then(wrap(*this,
                                         [](auto&& me) {
                                           me->commit();
                                         }),
                                    root().event_loop().through_dispatch());
      }

      ++changes_;
    }
  }

  Sink& sink() noexcept {
    IDLE_ASSERT(state().isRunning());
    return log_->sink(LogLevel::error);
  }

protected:
  void onSetup() override {
    if (config_.path.empty()) {
      config_.path = defaultLocation();
    }

    auto const file = boost::filesystem::absolute(config_.path);
    file_ = file.generic_string();
    name_ = file.filename().generic_string();
    auto const dir = file.parent_path();

    auto const extension = path(file_).extension().generic_string();
    IDLE_ASSERT(extension == ".toml");

    FileWatcher::Config fw;
    fw.watched.emplace_back(dir.generic_string(), false, false);
    fw.initial_add = false;
    fw.filter = [file = file_](StringView current) {
      return current == file;
    };

    file_watcher_->setup(std::move(fw));
  }

  continuable<> onStart() override {
    return async([this] {
      generation_ = 0;
      IDLE_ASSERT(!current_);
      IDLE_ASSERT(!previous_);

      // TODO Generate the source depending on the extension
      current_ = make_ref<Source>();

      if (auto epoch = readFromFile()) {
        epoch_ = std::move(epoch);
      } else {
        epoch_.reset();

        IDLE_LOG_ERROR(log_, "Failed to open or create config file '{}'!",
                       file_);

        throw ConfigReadException(); // TODO Improve this
      }

      waitForFileChanges();
    });
  }

  continuable<> onStop() override {
    return async([this] {
      IDLE_ASSERT(current_);

      if (changes_) {
        writeBack();
      }

      epoch_.reset();

      changes_ = 0;
      ignore_next_file_change_ = false;

      current_.reset();
      previous_.reset();
    });
  }

private:
  void waitForFileChanges() {
    file_watcher_->watch().then(
        weakly(handleOf(*this),
               [](FileWatcher::FileChanges const& changes, auto&& me) {
                 me->onFileChanged(changes);
               }),
        root().event_loop().through_post());
  }

  void onFileChanged(FileWatcher::FileChanges const& changes) {
    (void)changes;

    if (ignore_next_file_change_) {
      ignore_next_file_change_ = false;
    } else {
      IDLE_LOG_INFO(log_, "Reloading {}", name_);

      if (auto epoch = readFromFile()) {
        epoch_ = std::move(epoch);
      } else {
        IDLE_LOG_ERROR(log_, "Failed to load {}! Using previous properties.",
                       file_);
      }
    }

    waitForFileChanges();
  }

  /// Creates a new generation and increments the epoch counter
  Ref<ReloablePropertiesGeneration>
  createGeneration(Ref<PropertiesSource const> current,
                   Ref<PropertiesSource const> previous) {
    IDLE_ASSERT(root().is_on_event_loop());

    IDLE_ASSERT(current);
    IDLE_ASSERT(previous);

    Ref<ReloablePropertiesGeneration>
        ref = instantiate<ReloablePropertiesGeneration>(
            Inheritance::weak(static_cast<Collection&>(*this)), //
            std::move(current), std::move(previous), generation_++);

    ref->init();

    ref->start()
        .then(root().update())
        .fail(wrap(*this, [](exception_t const& e, auto&& me) {
          if (e) {
            try {
              std::rethrow_exception(e);
            } catch (std::exception const& ex) {
              char const* what = ex.what();
              (void)what;
              IDLE_LOG_ERROR(me->log_,
                             "System update failed from config reload ()!",
                             what);
            }
          } else {
            IDLE_DETAIL_LOG_DEBUG(
                "A service part of the system update went away.");
          }
        }));

    return ref;
  }

  Ref<ReloablePropertiesGeneration> readFromFile() {
    IDLE_ASSERT(current_);

    Ref<PropertiesSource> source = current_->create();
    IDLE_ASSERT(source);

    if (boost::filesystem::exists(file_)) {
      std::fstream fs(file_, std::ios_base::in);
      if (!fs.is_open()) {
        return {};
      }

      source->read(fs, file_);
    }

    previous_ = std::move(current_);
    current_ = source;

    IDLE_ASSERT(current_);
    IDLE_ASSERT(previous_);
    return createGeneration(std::move(source), previous_);
  }

  void writeToFile(PropertiesSource& source) {
    std::fstream fs(file_, std::ios_base::out);

    if (!fs.is_open()) {
      IDLE_LOG_ERROR(log_, "Failed open config file {} for writing!", file_);
      return;
    }

    source.write(fs, file_);
    IDLE_LOG_INFO(log_, "Saved {}", name_);
  }

  void writeBack() {
    if (config_.write_back) {
      ignore_next_file_change_ = true;

      writeToFile(*current_);
    }
  }

  void commit() {
    IDLE_ASSERT(root().is_on_event_loop());
    IDLE_ASSERT(changes_);

    changes_ = 0;

    writeBack();

    Ref<PropertiesSource const> persistent = current_->clone();
    if (auto epoch = createGeneration(persistent, previous_)) {
      epoch_ = std::move(epoch);
      previous_ = std::move(persistent);
    } else {
      IDLE_LOG_ERROR(log_,
                     "Failed to generate a ReloablePropertiesGeneration from "
                     "values in memory!");
    }
  }

  Config config_;
  std::string file_;
  std::string name_;

  Properties::Generation generation_{0};
  std::size_t changes_{0};
  bool ignore_next_file_change_{false};
  Ref<PropertiesSource const> previous_;
  Ref<PropertiesSource> current_;
  Ref<ReloablePropertiesGeneration> epoch_;

  Dependency<Log> log_{*this};
  Component<Debouncer> debouncer_{*this};
  Component<FileWatcher> file_watcher_{*this};
};

ReloadablePropertiesImpl& ReloablePropertiesGeneration::parent() noexcept {
  return static_cast<ReloadablePropertiesImpl&>(Super::parent());
}

ReloadablePropertiesImpl const&
ReloablePropertiesGeneration::parent() const noexcept {
  return static_cast<ReloadablePropertiesImpl const&>(Super::parent());
}

bool ReloablePropertiesGeneration::get(ReflectionPtr ptr,
                                       StringView key) const noexcept {

  data_->deserialize(ptr, key,
                     const_cast<ReloadablePropertiesImpl&>(parent()).sink());
  return true;
}

void ReloablePropertiesGeneration::set(ConstReflectionPtr ptr,
                                       StringView key) noexcept {
  IDLE_ASSERT(root().is_on_event_loop());

  parent().push(ptr, key);
}

bool ReloablePropertiesGeneration::changed(StringView key) const noexcept {
  return !(previous_ && data_->equals(*previous_, key));
}

PropertiesSource::~PropertiesSource() {}

Properties::Properties(Service& owner, Generation generation)
  : Interface(owner)
  , generation_(generation) {}

bool Properties::operator>(Interface const& other) const noexcept {
  return generation_ > cast<Properties>(other).generation();
}

std::string ReloadableProperties::defaultLocation() {
  return detail::get_default_path("toml").generic_string();
}

void ReloadableProperties::setup(Config config) {
  ReloadablePropertiesImpl::from(this)->setupImpl(std::move(config));
}

Ref<ReloadableProperties> ReloadableProperties::create(Inheritance inh) {
  return spawn<ReloadablePropertiesImpl>(std::move(inh));
}
} // namespace idle
