FROM ubuntu:20.04

RUN apt-get update \
 && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    build-essential \
    ca-certificates \
    locales \
    software-properties-common \
    wget \
 && apt-get clean \
 && locale-gen en_US.UTF-8 \
 && update-locale LANG=en_US.UTF-8

# Install CMake from a distribution
RUN mkdir -p /opt/cmake-inst \
 && cd /opt/cmake-inst \
 && wget -O dist.tar.gz https://cmake.org/files/v3.21/cmake-3.21.2-linux-x86_64.tar.gz \
 && tar xzf dist.tar.gz \
 && mv cmake-* /opt/cmake \
 && rm -rf /opt/cmake-inst
ENV PATH="/opt/cmake/bin:${PATH}"

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
 && add-apt-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-12 main" \
 && apt-get update \
 && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    clang-12 \
    g++-9 \
    fish \
    git \
    libssl-dev \
    ninja-build \
    graphviz \
    gdb \
    valgrind \
 && apt-get clean \
 && update-alternatives --install /usr/bin/cc cc $(which clang-12) 100 \
 && update-alternatives --install /usr/bin/c++ c++ $(which clang++-12) 100

ENV IDLE_VM=1
ENV IDLE_INTERNAL_LOGLEVEL=2

CMD mkdir -p /home/persistent/build \
 && cd /home/persistent/build \
 && echo "- Building target: '${BUILD_TARGET}'" \
 && echo "- Building type: '${BUILD_TYPE}'" \
 && cmake -GNinja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
          -DIDLE_WITH_INTERNAL_LOG=ON \
          -DCMAKE_CXX_FLAGS="-ftemplate-backtrace-limit=0" /home/src \
 && ninja ${BUILD_TARGET}
