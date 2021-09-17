<img src="https://user-images.githubusercontent.com/1146834/133035137-32dd8bf6-17c1-42a3-8ba3-ccffdd8f3fbf.gif" style="zoom: 80%;" />

*Idle* is an **asynchronous**, **hot-reloadable, and highly reactive dynamic component framework** similar to OSGI that is:

- :package: **Modular**: Your program logic is encapsulated into **services** that provide **interfaces** for usage
- :cyclone: **Dynamic**: Services can come and go whenever they want, *Idle* will keep your application stable
- :rocket: **Progressive**: Your code is **recompiled automatically** asynchronously into your application

Therefore *Idle* can improve your workflow significantly through **decreased code iteration time** and **cleaner code-bases** based on the *SOLID* principles, especially **interface segregation**. *Idle* is a **C++ engine** that is capable of **hot-reloading** large multi-module codebases on the fly through a scheduled dependency graph. What a game engine is to graphics, that is *Idle* for C++, without targeting a specific domain.
*Idle* can be used for almost any C++ application type e.g. **servers**, **GUIs**, or **graphic engines**.

Traditional solutions to C++ hot-reloading, such as binary patching, usually do not work when headers are modified, files are added, or code is modified out-of-function or even on optimized release builds. *Idle* solves this by using shared library loading/unloading techniques that are far more powerful than existing solutions, which are usually limited to simple classes and re-loadable loops. Therefore, *Idle* can automatically reload headers, compilation units, embedded resources and also takes changes of dependencies and build-system files into account.

Additionally, as C++ becomes more asynchronous in the future, the difficult question of how we can effectively use asynchrony in large applications arises. *Idle* provides simple patterns and solutions to solve such problems. Furthermore, C++ applications can be prototyped rapidly through *Idle's* asynchronous ecosystem that provides a CLI, logging, configuration, and persistency.

----

**:wrench: *Idle* is currently in development and therefore not ready for production yet.** But, *Idle* is highly suited already for side-projects, coding playgrounds, and case studies.

-----

## Table of Contents

- [Preview](#preview)
- [Features](#features)
  * [Dynamic Component System](#dynamic-component-system)
  * [Asynchronous Ecosystem](#asynchronous-ecosystem)
- [Usage Introduction](#usage-introduction)
  * [Managed Lifetime](#managed-lifetime)
  * [Eager Services through the Autostarted Interface](#eager-services-through-the-autostarted-interface)
  * [Clean Dependencies through Interfaces](#clean-dependencies-through-interfaces)
  * [Encapsulated and Configurable Components](#encapsulated-and-configurable-components)
- [Installation](#installation)
  * [Native](#native)
  * [Docker](#docker)
- [How does it Work?](#how-does-it-work)
- [Issues and Troubleshooting](#issues-and-troubleshooting)
  * [System Issues/Crashes/Segfaults/Terminations](#system-issues-crashes-segfaults-terminations)
  * [Service Dependency Issues](#service-dependency-issues)
  * [Build/HotSwap/Rebuilding Issues](#build-hotswap-rebuilding-issues)
- [FAQ](#faq)

## Preview

The following demonstration shows how a multi-module GUI application can be hot-reloaded with *Idle*:

https://user-images.githubusercontent.com/1146834/133030091-a0e7f2bf-bf5f-4b98-b53a-bed4d802c599.mp4

*The actual application shown in this demo is not included in this repository*

---

## Features

### Dynamic Component System

An application in *Idle* is based on multiple fundamental concepts:

- **idle::Interface**: Provides an arbitrary API for implementation
- **idle::Service**: Provides the actual implementation of a running entity inside the system.
- **Clusters** that describe a direct relation of an idle::Service to another to allow local configuration.
- **Usages** that express a relation of an idle::Service to cluster-external entities.

Therefore *Idle* provides many improvements over traditional dynamic component systems:

- **Zero run-time overhead** and while your system is stable: references to connected interfaces are cached and not mutated while you possibly could access it (except on request, to ensure your service is always **thread-safe**).
- **Declarative dependencies** allow simple dependency configuration without manifests (no XML/JSON).
- For the locale **configuration** of services *idle* uses strongly typed property classes instead of bug-prone string key-value pairs. You fetch the global configuration from your dedicated idle::Config service and propagate these back to nested components!

### Asynchronous Ecosystem

To build an asynchronous application, *Idle* provides a feature-rich and ready to use **asynchronous ecosystem** based on **components** and **interfaces**:

- Concurrency abstractions:
  - [**continuable**](https://github.com/Naios/continuable) as future/task primitive with optional C++20 coroutine support
  - (event-loop) **executor** and programmer-friendly custom threads.
- Asynchronous **PluginHotswap**:
  - CMake-driven - can reload code in < 2s based on your machine.
  - Compatible with *MSBuild* and *Ninja* on Windows and *make* and *Ninja* on Posix.
- Asynchronous **FileWatcher** (based on [efsw](https://github.com/SpartanJ/efsw/))
- Asynchronous **ProcessGroup** (base on [boost::process](https://github.com/boostorg/process))
- Asynchronous **Command** interface and a default command processor
- A terminal **repl** with command auto-completion and activity progress indicators (based on [replxx](https://github.com/AmokHuginnsson/replxx)).
- Uniform **logger** facade for pluggable log message sinks ([spdlog](https://github.com/gabime/spdlog) for example)
- A **configuration** system based on a custom **reflection** system that can adapt to any configuration format (TOML currently implemented).
  - Changes to the configuration file are detected automatically and dependent services are updated accordingly.
  - The configuration file is kept in sync automatically with recently added configuration values and their description.
- A **storage** system to persist data across hot-swaps using our **reflection** system or a user-provided byte buffer.

*Networking/HTTP abstractions are considered out-of-scope for this project but might be provided through an external module* later

---

![](https://user-images.githubusercontent.com/1146834/128798987-ccdd81ca-380e-4d8c-9ba9-c8db24047cb6.png)

## Usage Introduction

This short introduction covers only a small part of the functionality of *Idle*. To get into the framework we provide you various [examples](examples/Readme.md) that you can try out dynamically inside the *Idle* CLI.

### Managed Lifetime

Because *Idle* understands the dependencies of your code and data, it can manage its lifetime much better than you could express it manually. Services are **lazy** by design and provide an asynchronous `onStart` and `onStop` override-able method for managing its lifetime. Thus the application start is automatically parallelized and fast, whereas stopping service cleans up resources safely:

```cpp
class HelloWorldService : public idle::Service {
public:
  using Super::Super;

  continuable<> onStart() override {
    return idle::async([] {
      puts("HelloWorldService service is being started!");
    });
  }
  continuable<> onStop() override {
    return idle::async([] {
      puts("HelloWorldService service is being stopped!");
    });
  }
};
```

For initialization, services also provide a thread-safe `onInit` and `onDestroy` method that is always dispatched on the same event loop (regardless of the thread that initialized or destroyed the service).

```cpp
class HelloWorldService : public idle::Service {
public:
  using Super::Super;

  void onInit() override {
    IDLE_ASSERT(root().is_on_event_loop());
  }

  void sayHello() {
    puts("Hello!");
  }
};
```

Initialization always happens automatically before starting the service:

```cpp
Ref<Context> context = Context::create();
Ref<HelloWorldService> hello_world = idle::spawn<HelloWorldService>(*context);

hello_world->start().then([=] {
  hello_world->sayHello();
});
```

Services not referenced anymore are stopped and garbage-collected automatically.

### Eager Services through the Autostarted Interface

Because an idle::Service is lazy by design, we have to start it manually through `service->start()`. Starting a service automatically is supported by the ecosystem itself, rather than the core system. We can export an `idle::Autostarted` interface from any service, which causes the service to be started automatically (because a global `idle::Autostarter` service will create a dependency on it).

```cpp
class AutostartedService : public idle::Implements<idle::Autostarted> {
public:
  using Super::Super;
};

IDLE_DECLARE(AutostartedService)
```

The `IDLE_DECLARE`macro introduces the `AutostartedService` to the system and can be used in any executable or plugin directly.

### Clean Dependencies through Interfaces

Dependencies in *idle* are expressed in two ways: parent-child relationships and usages.

*idle* provides many helper classes for expressing dependencies between services, specialized in cardinality (1:1, 1:n), and flexibility (statically or changeable at run-time).

Every helper class creates `Usage` objects in different ways and can also be created for a custom purpose.


The `Dependency` class models a 1:1 non-runtime-changeable dependency, for example, we can make our `HelloWorldService` depend on the `idle::Log` as following:

```cpp
class HelloWorldService : public idle::Service {
public:
  using Super::Super;
    
  continuable<> onStart() override {
    return idle::async([this] {
      IDLE_LOG_INFO(log_, "{} is being started!", this->name());
    });
  }

private:
  idle::Dependency<idle::Log> log_{*this};
};

IDLE_DECLARE(HelloWorldService)
```

The `Dependency` always ensures that the service we depend on is started before the depending service. Additionally, the dependent service stays running until all depending services have stopped.

*Idle* services encapsulate popular open-source libraries with best practices under the hood.

For example, the log message above is formatted with fmtlib (compile-time fmt) and can be dispatched to your favorite log sink (spdlog, for example).

Declaring an `Interface` is as easy as it gets. *Idle* supports default created services and exporting multiple `Interface` classes from the same `Service`:

```cpp
class MyInterface : public idle::Interface {
public:
  using Super::Super;
    
  virtual void doSomething() = 0;
    
  IDLE_INTERFACE //< Optional
};

class MyInterfaceProvider public : idle::Implements<MyInterface> {
public:
  using Super::Super;

  void doSomething() override {
    // ...
  }
    
  IDLE_SERVICE //< Optional
};
```

With the included support of weak dependencies, *Idle* can schedule weakly cyclic dependency graphs, which represent the strongest constrained graph type, that can be scheduled. Cycles are allowed to appear, as long as a dependency can be attached and detached at run-time to and from a depending idle::Service.

### Encapsulated and Configurable Components

In this example a nested component is configured from its parent idle::Service.

A configuration file is maintained automatically from the reflected configurations in the system. Services depending on a specific config key, are restarted automatically if a depending value is changed inside the file.

```cpp
struct ConfigurableConfig {
  bool flag{false};
  int value{0};
};

IDLE_REFLECT(ConfigurableConfig, flag, (value, "An optional comment"));

class ConfigurableService : public idle::Service {
public:
  using Super::Super;
    
  void setup(ConfigurableConfig conf) {
    config_ = std::move(conf);
  }

  void onSetup() override {
    my_component_->setup(config_.value);
  }

private:
  idle::Var<ConfigurableConfig> var_{*this, "config.hello"};
  idle::Component<MyComponent> my_component_{*this};
};

IDLE_DECLARE(ConfigurableService)
```

For a more detailed insight into the framework, further [examples](examples/Readme.md) are provided.

## Installation

### Native

Requirements:

- Windows, Linux, or macOS (not fully tested yet)
- A C++14 capable compiler: MSVC 2019 (16.7+), Clang 5+, GCC 9+
- CMake 3.17+

1. *Idle* uses custom CMake scripts to download and set up its source dependencies automatically.
2. A CMake install will build and install a ready-to-use distribution to your preferred install location.

```sh
git clone https://github.com/Naios/idle.git
cd idle
mkdir build
mkdir install
cd build

# MSBuild (Visual Studio)
cmake .. -DCMAKE_INSTALL_PREFIX=install
cmake --build . --config Debug  --target INSTALL

# Ninja/make
cmake ..  -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target install

../install/bin/idle

# Start editing the project files in `install/project/src`
# Ready-to-use example source files are installed in `install/project/examples`
```

### Docker

A ready-to-build `Dockerfile` is available inside the `docker` subdirectory:

1. `./docker/build.sh`: Builds the image with name *idle*
2. `./docker/make.sh`: Builds *idle* into `build/docker` (through mounting the source directory)
   Make sure that Docker is allowed to mount the `idle` source directory.
3. `./docker/shell.sh ./idle`: Invokes `idle` (from `build/docker`) inside the Docker container

In Docker, *Idle* supports the automatic rebuilding of source files from a mounted file system due to switching to a polling filewatcher in such cases (controlled with the `IDLE_VM` environment variable).
Therefore it is possible to edit one project that can be automatically tested and live reloaded inside multiple Docker containers at the same time.

## How does it Work?

1. Internally services and their imports and exports are represented as a **weakly cyclic directed graph**
    (*[click to enlarge](https://rawcdn.githack.com/Naios/assets/316fc4d6ca1f1bcfc3e5890d7d61fc2f3857100e/graph.svg)*):<img src="https://rawcdn.githack.com/Naios/assets/316fc4d6ca1f1bcfc3e5890d7d61fc2f3857100e/graph.svg" style="zoom:8%;" /> 

   > You can generate your own Graphviz graph as shown above, through the `idle graph show [service | cluster]` command, which is invocable from the *Idle* CLI.

2. Based on this graph, service starts and stops are scheduled, and preferred dependencies are selected

3. Because we know the topology of the whole application, we can change services effectively at run-time

## Issues and Troubleshooting

### System Issues/Crashes/Segfaults/Terminations

At its current state, *Idle* will work mostly out of the box when everything works as expected. The code-base currently lacks proper error/exception handling which will be reworked in a future iteration. For instance, throwing an exception from an `idle::Service::onStart` method currently leads to an application shutdown, but will properly be handled in the future.

The first step for troubleshooting is to enable the internal logs that are separated from the logger of the ecosystem:

1. If you are building in `Release` mode make sure that the CMake option is `IDLE_WITH_INTERNAL_LOG=ON` is set.
   In `Debug` builds  *Idle* is always compiled with internal logging support built-in.
2. Enable the internal *Idle* logs by defining the `IDLE_INTERNAL_LOGLEVEL` environment variable (system-wide or per invocation):
   1. `IDLE_INTERNAL_LOGLEVEL=0 ./idle` : Runs *Idle* with trace logs (most detailed but might print sensitive information)
   2. `IDLE_INTERNAL_LOGLEVEL=1 ./idle` : Runs *Idle* with debug logs (prints less than trace)
   3. `IDLE_INTERNAL_LOGLEVEL=2 ./idle`: Runs *Idle* with error logs (non-fatal errors which need to be handled properly)

### Service Dependency Issues

The entire graph of your current *Idle* system can be exported through GraphViz to many visual formats (pdf, SVG) for explanation purposes, to help debugging issues with resolved dependencies, and to analyze the overall system. This requires that the GraphViz `dot` program can be found through your systems `PATH` environment variable.

A graph (in various granularities) can be generated with the  `idle graph show [service | cluster]` command, which can be invoked from the *Idle* CLI directly.

**If you are reporting an issue make sure to attach the graph of your current system as a pdf, an internal trace log as well as any available stack trace logs to your bug report.**

### Build/HotSwap/Rebuilding Issues

If you are running into issues with rebuilds, try to delete the build directory first. In its default configuration, the build directory is created in `idle/project/build`. *Idle* detects a missing build directory on start, and will automatically re-create it for you.

## FAQ

- Is *Idle* related to an entity-component framework (ECS)?
  - No, services in *Idle* are meant to provide logical functionality and heavyweight data.
    Services require a unique position in memory and are mostly dynamically or statically allocated. They are not cache-optimized nor recommended for representing an entity where you expect to have thousands of instances alive or in iteration simultaneously.
    We recommend using an ECS from inside a service instead of making a service part of an ECS.
- Does *Idle* hot-swap require a specific platform, compiler flag, or toolchain?
  - No, theoretically *Idle* hot-swap can be used on every platform that supports dynamic runtime linking. Because dependencies are tracked and swapped on a per-service level, this will even work on heavily optimized release builds and can be used theoretically in production as well.
- Is it possible to ship my application with static instead of dynamic linkage?
  - You can statically link your plugins fully into your *idle* application without any change in your source code. *Idle* can even read your `IDLE_DECLARE` definitions from your statical linked executable and instantiate the exported services as they were exported from a plugin.
    A recommended workflow is to develop your application by using dynamic linking and automatic hot-swapping for a short code iteration while distributing it with static linking and no hot-swap.
    Through that, static lifetime differences between platforms and build types will belong to the past.
- Is it possible to disable RTTI?
  - *Idle* uses RTTI for improved naming of your services only, if available, and can be used without RTTI. Naming support through the `IDLE_SERVICE`, `IDLE_INTERFACE`, and `IDLE_PART` macros are implemented without RTTI through constexpr type reflection (`__PRETTY_FUNCTION__` parsing).
- Why is this software licensed under AGPL v3? Is it planned to relicense it?
  - *Idle* is licensed under the AGPL until the project has grown in maturity and we can narrow the project's direction. Different parts of the project might be relicensed/sublicensed to a more permissive license to make it available to a broader audience.
    I'm currently looking into possibilities to fund this project properly, and therefore licensing it now for open commercial use, would limit my options.

