FROM nvidia/cuda
MAINTAINER Deepak Roy Chittajallu <deepk.chittajallu@kitware.com>

# Install system pre-requisites
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    wget \
    git \
    emacs vim \
    make cmake cmake-curses-gui \
    ninja-build && \
    apt-get autoremove && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Working directory
ENV BUILD_PATH=$PWD/build

# Install miniconda
RUN mkdir -p $BUILD_PATH && \
    wget https://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh \
    -O $BUILD_PATH/install_miniconda.sh && \
    bash $BUILD_PATH/install_miniconda.sh -b -p $BUILD_PATH/miniconda && \
    rm $BUILD_PATH/install_miniconda.sh && \
    chmod -R +r $BUILD_PATH && \
    chmod +x $BUILD_PATH/miniconda/bin/python
ENV PATH=$BUILD_PATH/miniconda/bin:${PATH}


# Install CMake
ENV CMAKE_ARCHIVE_SHA256 10ca0e25b7159a03da0c1ec627e686562dc2a40aad5985fd2088eb684b08e491
ENV CMAKE_VERSION_MAJOR 3
ENV CMAKE_VERSION_MINOR 8
ENV CMAKE_VERSION_PATCH 1
ENV CMAKE_VERSION ${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR}.${CMAKE_VERSION_PATCH}
RUN cd $BUILD_PATH && \
  wget https://cmake.org/files/v${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR}/cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz && \
  hash=$(sha256sum ./cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz | awk '{ print $1 }') && \
  [ $hash = "${CMAKE_ARCHIVE_SHA256}" ] && \
  tar -xzvf cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz && \
  rm cmake-${CMAKE_VERSION}-Linux-x86_64.tar.gz
ENV PATH=$BUILD_PATH/cmake-${CMAKE_VERSION}-Linux-x86_64/bin:${PATH}

# Disable "You are in 'detached HEAD' state." warning
RUN git config --global advice.detachedHead false

# Download/configure/build/install TubeTK
ENV TubeTK_SRC_DIR=$PWD/TubeTK
ENV TubeTK_BUILD_DIR=$TubeTK_SRC_DIR/build
RUN mkdir -p $TubeTK_SRC_DIR && mkdir -p $TubeTK_BUILD_DIR
COPY . $TubeTK_SRC_DIR
WORKDIR $TubeTK_BUILD_DIR

RUN pip install --upgrade --ignore-installed pip && \
    pip install -U -r $TubeTK_SRC_DIR/requirements.txt && \
    cmake \
        -G Ninja \
        -DTubeTK_BUILD_APPLICATIONS:BOOL=ON \
        -DTubeTK_USE_PYTHON:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        -DTubeTK_USE_ARRAYFIRE:BOOL=OFF \
        -DTubeTK_USE_EXAMPLES_AS_TESTS:BOOL=OFF \
        -DTubeTK_USE_BOOST:BOOL=OFF \
        -DTubeTK_USE_PYQTGRAPH:BOOL=OFF \
        -DTubeTK_USE_VALGRIND:BOOL=OFF \
        -DUSE_SYSTEM_ITK:BOOL=OFF \
        -DUSE_SYSTEM_SLICER_EXECUTION_MODEL:BOOL=OFF \
        ../ && \
    ninja && \
    cd $TubeTK_SRC_DIR/Applications && \
    python generate_slicer_cli_list_json.py
ENV PATH="${TubeTK_BUILD_DIR}/TubeTK-build/bin/:${PATH}"

# Install ctk-cli
RUN conda install --yes -c cdeepakroy ctk-cli=1.4.1

# Download and install slicer_cli_web
RUN cd $BUILD_PATH && \
    git clone https://github.com/girder/slicer_cli_web.git && \
    git checkout take_cpp_cli_from_path

# Set workdir to TubeTK Applications
WORKDIR $TubeTK_SRC_DIR/Applications

# Test slicer_cli_web entrypoint
RUN python /build/slicer_cli_web/server/cli_list_entrypoint.py --list_cli

# Set entrypoint
ENTRYPOINT ["/build/miniconda/bin/python", "/build/slicer_cli_web/server/cli_list_entrypoint.py"]

