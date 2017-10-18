FROM gcc
WORKDIR cosy
RUN apt-get update \
&&  apt-get install -y \
    cmake
COPY Makefile Makefile
COPY adapters adapters
COPY include include
COPY makefiles makefiles
COPY scripts scripts
COPY solvers solvers
COPY src src
COPY tests tests
RUN make
