#!/usr/bin/env bash
###############################################################################
# TFBoost one-shot launcher.
#
# Clone the repository and run this script. It will get you to a running
# TFBoost GUI by whichever route is available:
#
#   1. If podman or docker is present  -> build the container image (once) and
#      launch the GUI from inside it (ROOT + all deps are bundled).
#   2. Otherwise                       -> install the system dependencies,
#      build TFBoost natively, and launch the GUI.
#
# Usage:
#   ./start.sh                 auto: container if available, else native
#   ./start.sh --native        force the native (no-container) route
#   ./start.sh --rebuild       rebuild the container image / re-run cmake build
#   ./start.sh --analysis      run the analysis backend on ./data instead of GUI
#                              (container route only)
#   ./start.sh --help
###############################################################################
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$HERE"

IMAGE=tfboost
FORCE_NATIVE=0
REBUILD=0
MODE=gui           # gui | analysis

for arg in "$@"; do
    case "$arg" in
        --native)   FORCE_NATIVE=1 ;;
        --rebuild)  REBUILD=1 ;;
        --analysis) MODE=analysis ;;
        -h|--help)
            sed -n '3,19p' "$0" | sed 's/^# \{0,1\}//'
            exit 0 ;;
        *) echo "Unknown option: $arg (try --help)"; exit 2 ;;
    esac
done

log()  { printf '\033[1;34m[TFBoost]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[TFBoost]\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31m[TFBoost]\033[0m %s\n' "$*" >&2; exit 1; }

#------------------------------------------------------------------------------
# Container route
#------------------------------------------------------------------------------
detect_runtime() {
    if command -v podman >/dev/null 2>&1; then echo podman
    elif command -v docker >/dev/null 2>&1; then echo docker
    fi
}

run_container() {
    local rt="$1"
    log "Container runtime: $rt"

    if [ "$REBUILD" -eq 1 ] || ! "$rt" image inspect "$IMAGE" >/dev/null 2>&1; then
        log "Building image '$IMAGE' (first run downloads a ROOT base image, be patient)..."
        "$rt" build -f install/Dockerfile -t "$IMAGE" .
    else
        log "Reusing existing image '$IMAGE' (use --rebuild to force a rebuild)."
    fi

    mkdir -p "$HERE/data/input" "$HERE/data/results"

    if [ "$MODE" = analysis ]; then
        log "Running the analysis backend on ./data (inputs in data/input, results in data/results)..."
        exec "$rt" run --rm -v "$HERE/data:/data:Z" "$IMAGE"
    fi

    # --- GUI: needs an X server on the host --------------------------------
    [ -n "${DISPLAY:-}" ] || die "No DISPLAY set; a running X server is required for the GUI. \
Use './start.sh --analysis' for a headless run, or './start.sh --native'."

    # Allow the container to talk to the local X server (revoked on exit).
    if command -v xhost >/dev/null 2>&1; then
        xhost +local: >/dev/null 2>&1 || true
        trap 'xhost -local: >/dev/null 2>&1 || true' EXIT
    fi

    local tty_flags=()
    [ -t 0 ] && tty_flags=(-it)

    log "Launching the TFBoost GUI from the container..."
    exec "$rt" run --rm "${tty_flags[@]}" \
        -e DISPLAY \
        -e XDG_RUNTIME_DIR=/tmp \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        --security-opt label=disable \
        -v "$HERE/data:/data:Z" \
        "$IMAGE" gui
}

#------------------------------------------------------------------------------
# Native route
#------------------------------------------------------------------------------
SUDO=""
need_sudo() { [ "$(id -u)" -eq 0 ] || SUDO="sudo"; }

install_deps_native() {
    need_sudo
    if command -v dnf >/dev/null 2>&1; then
        log "Installing dependencies with dnf..."
        $SUDO dnf install -y \
            gcc-c++ cmake git make \
            libconfig-devel tclap tbb-devel fftw-devel \
            root \
            python3-tkinter python3-matplotlib python3-pandas python3-pillow-tk
    elif command -v apt-get >/dev/null 2>&1; then
        log "Installing dependencies with apt..."
        $SUDO apt-get update
        $SUDO apt-get install -y \
            g++ cmake git make \
            libconfig++-dev libtclap-dev libtbb-dev libfftw3-dev \
            python3-tk python3-matplotlib python3-pandas python3-pil.imagetk
        # ROOT is not in the default apt repositories.
    else
        die "No supported package manager (dnf/apt) found. Install the deps listed in README.md manually, \
or install podman/docker to use the container route."
    fi
}

ensure_root() {
    if command -v root-config >/dev/null 2>&1; then
        log "ROOT found: $(root-config --version)"
        return
    fi
    # thisroot.sh may just need sourcing
    for c in /opt/root/bin/thisroot.sh /usr/local/bin/thisroot.sh "${ROOTSYS:-}/bin/thisroot.sh"; do
        # shellcheck disable=SC1090
        [ -f "$c" ] && { . "$c"; break; }
    done
    command -v root-config >/dev/null 2>&1 && { log "ROOT found: $(root-config --version)"; return; }
    die "ROOT (root-config) not found and could not be installed automatically on this distro. \
Install ROOT from https://root.cern/install/ (and 'source .../bin/thisroot.sh'), then re-run — \
or install podman/docker to use the fully-bundled container route instead."
}

ensure_hydra() {
    if [ -n "${HYDRA_INCLUDE_DIR:-}" ] && [ -f "${HYDRA_INCLUDE_DIR}/hydra/Hydra.h" ]; then
        log "Using HYDRA from HYDRA_INCLUDE_DIR=${HYDRA_INCLUDE_DIR}"
        return
    fi
    HYDRA_INCLUDE_DIR="$HERE/.deps/hydra"
    if [ ! -f "${HYDRA_INCLUDE_DIR}/hydra/Hydra.h" ]; then
        log "Cloning HYDRA headers into .deps/hydra ..."
        mkdir -p "$HERE/.deps"
        git clone --depth 1 https://github.com/MultithreadCorner/Hydra.git "$HYDRA_INCLUDE_DIR"
    fi
    export HYDRA_INCLUDE_DIR
}

build_native() {
    if [ "$REBUILD" -eq 1 ] || [ ! -x "$HERE/build/analysis_tbb" ]; then
        log "Configuring and building the TBB backends (this can take a while)..."
        cmake -S "$HERE" -B "$HERE/build" -DHYDRA_INCLUDE_DIR="$HYDRA_INCLUDE_DIR"
        cmake --build "$HERE/build" -j "$(nproc)" \
            --target analysis_tbb deconvolution_tbb resampling_tbb 3Ddiamond_tbb
    else
        log "Reusing existing build/ (use --rebuild to force a rebuild)."
    fi
}

run_native() {
    log "Installing dependencies..."
    install_deps_native
    ensure_root
    ensure_hydra
    build_native
    log "Launching the TFBoost GUI..."
    cd "$HERE/gui"
    exec python3 TFBoostGui.py
}

#------------------------------------------------------------------------------
main() {
    local rt=""
    [ "$FORCE_NATIVE" -eq 0 ] && rt="$(detect_runtime)"

    if [ -n "$rt" ]; then
        run_container "$rt"
    else
        [ "$FORCE_NATIVE" -eq 1 ] && log "Native route forced (--native)." \
                                  || log "No podman/docker found — using the native route."
        [ "$MODE" = analysis ] && die "--analysis is only supported on the container route."
        run_native
    fi
}

main
