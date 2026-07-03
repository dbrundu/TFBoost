###############################################################################
# TFBoost — single-container build & run image
#
# Starts from an image that already ships ROOT (so ROOT is NOT recompiled),
# pulls in the remaining dependencies (HYDRA headers, TBB, TCLAP, libconfig,
# FFTW), builds the TBB backends, and exposes an entrypoint that runs a chosen
# target against input data mounted at /data.
#
# Build (run from the repository ROOT, the build context must be the repo):
#   podman build -f install/Dockerfile -t tfboost .
#   docker build -f install/Dockerfile -t tfboost .
#
# Run (mount a host folder with your input signals at /data):
#   podman run --rm -v "$PWD/mydata:/data" tfboost
#
# See install/DOCKER.md for the full usage guide.
###############################################################################

# Base image carrying a prebuilt ROOT (Ubuntu based, ships gcc/cmake/git too).
# Override with --build-arg BASE_IMAGE=rootproject/root:<tag> to pin a version.
ARG BASE_IMAGE=docker.io/rootproject/root:latest
FROM ${BASE_IMAGE}

# HYDRA source (header-only). Override repo/ref with --build-arg if needed.
# Verified working build used the fork synced with upstream at HYDRA_VERSION
# 4.0.1; upstream master tracks the same code (the renamed spline/ Thrust bits
# the in-tree fixes depend on).
ARG HYDRA_REPO=https://github.com/MultithreadCorner/Hydra.git
ARG HYDRA_REF=master

# Microarchitecture for the build. CMakeLists.txt forces -march=native, which
# bakes the *build host's* CPU into the binaries and can yield SIGILL on a
# different CPU. Keep "native" for a same-machine image; set e.g.
# --build-arg TFB_MARCH=x86-64-v2 to produce a portable image.
ARG TFB_MARCH=native

# Which targets to compile into the image (space separated).
ARG TFB_TARGETS="analysis_tbb deconvolution_tbb resampling_tbb 3Ddiamond_tbb analyze_single_signal_tbb simple_analysis_tbb"

ENV TFB_HOME=/opt/TFBoost \
    HYDRA_INCLUDE_DIR=/opt/hydra \
    DEBIAN_FRONTEND=noninteractive

# --- Dependencies (ROOT is already present in the base image) -----------------
# Build toolchain + libraries, plus the Python/Tk runtime the GUI needs so the
# same image can either run a backend directly or launch the Tkinter front-end.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        ca-certificates \
        libtbb-dev \
        libtclap-dev \
        libconfig++-dev \
        libfftw3-dev \
        python3-tk \
        python3-matplotlib \
        python3-pandas \
        python3-pil.imagetk \
        fonts-liberation \
        fonts-dejavu-core \
        fontconfig \
    && rm -rf /var/lib/apt/lists/*

# --- HYDRA headers ------------------------------------------------------------
RUN git clone --depth 1 --branch "${HYDRA_REF}" "${HYDRA_REPO}" "${HYDRA_INCLUDE_DIR}"

# --- TFBoost sources ----------------------------------------------------------
COPY . ${TFB_HOME}
WORKDIR ${TFB_HOME}

# Optionally relax -march=native for a portable image, then configure + build
# the requested TBB targets.
RUN if [ "${TFB_MARCH}" != "native" ]; then \
        sed -i "s/-march=native/-march=${TFB_MARCH}/g" CMakeLists.txt ; \
    fi \
 && cmake -S . -B build -DHYDRA_INCLUDE_DIR="${HYDRA_INCLUDE_DIR}" \
 && cmake --build build --target ${TFB_TARGETS} -j "$(nproc)"

# Make the example config the live one so a default run works out of the box
# (the committed etc/configuration.cfg points at the original author's machine).
RUN cp examples/config.cfg etc/configuration.cfg

# Binaries read their config via a hardcoded ../etc/*.cfg path, so they MUST be
# launched from the build directory.
WORKDIR ${TFB_HOME}/build

COPY install/docker-entrypoint.sh /usr/local/bin/tfboost-run
RUN chmod +x /usr/local/bin/tfboost-run

VOLUME ["/data"]

ENTRYPOINT ["/usr/local/bin/tfboost-run"]
CMD ["analysis_tbb"]
