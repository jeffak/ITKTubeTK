FROM kitwaremedical/cuda-itk-vtk-seml
MAINTAINER Deepak Roy Chittajallu <deepk.chittajallu@kitware.com>

# Download/configure/build/install TubeTK
ENV TubeTK_SRC_DIR=$PWD/TubeTK
ENV TubeTK_BUILD_DIR=$TubeTK_SRC_DIR/build
RUN mkdir -p $TubeTK_SRC_DIR && mkdir -p $TubeTK_BUILD_DIR
COPY . $TubeTK_SRC_DIR
WORKDIR $TubeTK_BUILD_DIR

RUN cmake \
        -G Ninja \
        -DTubeTK_BUILD_APPLICATIONS:BOOL=ON \
        -DTubeTK_USE_PYTHON:BOOL=ON \
        -DBUILD_SHARED_LIBS:BOOL=ON \
        -DTubeTK_USE_VTK:BOOL=ON \
        -DBUILD_TESTING:BOOL=OFF \
        -DTubeTK_BUILD_USING_SLICER:BOOL=OFF \
        -DTubeTK_USE_ARRAYFIRE:BOOL=ON \
        -DTubeTK_USE_EXAMPLES_AS_TESTS:BOOL=OFF \
        -DTubeTK_USE_BOOST:BOOL=OFF \
        -DTubeTK_USE_PYQTGRAPH:BOOL=OFF \
        -DTubeTK_USE_NUMPY_STACK:BOOL=OFF \
        -DTubeTK_USE_VALGRIND:BOOL=OFF \
        -DUSE_SYSTEM_ITK:BOOL=ON \
        -DUSE_SYSTEM_SLICER_EXECUTION_MODEL:BOOL=ON \
        -DUSE_SYSTEM_VTK:BOOL=ON \
        ../ && \
    ninja && \
    cd $TubeTK_SRC_DIR/Applications && \
    python generate_slicer_cli_list_json.py
ENV PATH="${TubeTK_BUILD_DIR}/TubeTK-build/bin/:${PATH}"

# Set workdir to TubeTK Applications
WORKDIR $TubeTK_SRC_DIR/Applications

# Test slicer_cli_web entrypoint
RUN python /build/slicer_cli_web/server/cli_list_entrypoint.py --list_cli

# Set entrypoint
ENTRYPOINT ["/build/miniconda/bin/python", "/build/slicer_cli_web/server/cli_list_entrypoint.py"]

