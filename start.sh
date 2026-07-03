#!/usr/bin/env bash
###############################################################################
# TFBoost one-shot launcher.
#
# Clone the repository and run this script. By default it runs the analysis
# backend (CLI) on a configuration file, by whichever route is available:
#
#   1. If podman or docker is present  -> build the container image (once, ROOT
#      and all deps bundled) and run the backend inside it.
#   2. Otherwise                       -> install deps, build TFBoost natively,
#      and run the backend.
#
# Usage:
#   ./start.sh                              example run (bundled config + inputs)
#   ./start.sh -c my.cfg                    run the analysis with your config
#   ./start.sh -c my.cfg -i ./sig -o ./out  ...with custom input/output dirs
#   ./start.sh --gui                        launch the Tkinter GUI instead
#
# Options:
#   -c, --configuration FILE   analysis config file (transfer function + params)
#   -i, --input DIR            directory of input signals   (default: ./data/input)
#   -o, --output DIR           directory for results        (default: ./data/results)
#       --gui                  launch the GUI instead of the CLI backend
#       --native               force the native (no-container) route
#       --rebuild              rebuild the container image / re-run the native build
#   -h, --help
#
# In the container the config supplies the transfer function and analysis
# parameters, while the data location comes from --input/--output (mounted in);
# a config's own InputDirectory/OutputDirectory cannot see the host filesystem.
###############################################################################
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$HERE"

IMAGE=tfboost
FORCE_NATIVE=0
REBUILD=0
MODE=analysis          # analysis | gui
CONFIG=""
INPUT=""
OUTPUT=""

usage() { sed -n '3,30p' "$0" | sed 's/^# \{0,1\}//'; }
need_val() { [ "$2" -ge 2 ] || { echo "Option $1 needs an argument (try --help)"; exit 2; }; }

while [ $# -gt 0 ]; do
    case "$1" in
        -c|--config|--configuration) need_val "$1" $#; CONFIG="$2"; shift 2 ;;
        -i|--input)                  need_val "$1" $#; INPUT="$2";  shift 2 ;;
        -o|--output)                 need_val "$1" $#; OUTPUT="$2"; shift 2 ;;
        --gui)      MODE=gui;        shift ;;
        --native)   FORCE_NATIVE=1;  shift ;;
        --rebuild)  REBUILD=1;       shift ;;
        -h|--help)  usage; exit 0 ;;
        *) echo "Unknown option: $1 (try --help)"; exit 2 ;;
    esac
done

log()  { printf '\033[1;34m[TFBoost]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[TFBoost]\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31m[TFBoost]\033[0m %s\n' "$*" >&2; exit 1; }

# The GUI has a fixed-pixel layout designed for ~96 dpi; on HiDPI / fractionally
# scaled desktops it renders tiny. Pick a UI scale factor so it comes out the
# right physical size. Honor an explicit TFB_UI_SCALE, else use the largest
# monitor scale GNOME has configured, else 1.0.
detect_ui_scale() {
    if [ -n "${TFB_UI_SCALE:-}" ]; then echo "$TFB_UI_SCALE"; return; fi
    local mx="$HOME/.config/monitors.xml" s=""
    if [ -f "$mx" ]; then
        s=$(grep -oE '<scale>[0-9.]+</scale>' "$mx" 2>/dev/null \
            | grep -oE '[0-9.]+' | sort -rn | head -1)
    fi
    echo "${s:-1.0}"
}

#------------------------------------------------------------------------------
# Container route
#------------------------------------------------------------------------------
detect_runtime() {
    if command -v podman >/dev/null 2>&1; then echo podman
    elif command -v docker >/dev/null 2>&1; then echo docker
    fi
}

build_image_if_needed() {
    local rt="$1"
    if [ "$REBUILD" -eq 1 ] || ! "$rt" image inspect "$IMAGE" >/dev/null 2>&1; then
        log "Building image '$IMAGE' (first run downloads a ROOT base image, be patient)..."
        "$rt" build -f install/Dockerfile -t "$IMAGE" .
    else
        log "Reusing existing image '$IMAGE' (use --rebuild to force a rebuild)."
    fi
}

run_container_analysis() {
    local rt="$1"
    local args=(--rm --security-opt label=disable)

    if [ -n "$CONFIG" ]; then
        [ -f "$CONFIG" ] || die "Config file not found: $CONFIG"
        args+=(-v "$(realpath "$CONFIG"):/work/config.cfg:ro" -e TFB_CONFIG=/work/config.cfg)
    fi

    # Input: explicit --input, else ./data/input if populated, else the config's
    # own value (the bundled example points at baked-in example signals).
    local input="$INPUT"
    if [ -z "$input" ] && [ -d "$HERE/data/input" ] && [ -n "$(ls -A "$HERE/data/input" 2>/dev/null)" ]; then
        input="$HERE/data/input"
    fi
    if [ -n "$input" ]; then
        [ -d "$input" ] || die "Input directory not found: $input"
        args+=(-v "$(realpath "$input"):/data/input:ro" -e TFB_INPUT=/data/input)
    fi

    local output="${OUTPUT:-$HERE/data/results}"
    mkdir -p "$output"
    args+=(-v "$(realpath "$output"):/data/results" -e TFB_OUTPUT=/data/results)

    log "Running analysis  (config: ${CONFIG:-bundled example}, input: ${input:-<from config>}, output: $output)"
    exec "$rt" run "${args[@]}" "$IMAGE"
}

run_container_gui() {
    local rt="$1"
    [ -n "${DISPLAY:-}" ] || die "No DISPLAY set; a running X server is required for the GUI. \
Drop --gui for the headless CLI, or use --native."

    # Give the container access to the X server. Host-based access (xhost) alone
    # is often not enough (the X server wants a cookie -> "Authorization required,
    # but no authorization protocol specified"). The portable fix is to hand the
    # container an Xauthority cookie whose entry has a *wildcard* hostname, so it
    # authenticates from inside the container's network namespace.
    local xauth=""
    if command -v xauth >/dev/null 2>&1; then
        xauth="$(mktemp --tmpdir tfboost.xauth.XXXXXX)"
        xauth nlist "$DISPLAY" 2>/dev/null | sed -e 's/^..../ffff/' \
            | xauth -f "$xauth" nmerge - 2>/dev/null || true
        chmod 644 "$xauth"
    fi
    command -v xhost >/dev/null 2>&1 && xhost +local: >/dev/null 2>&1 || true
    trap '[ -n "$xauth" ] && rm -f "$xauth"; command -v xhost >/dev/null 2>&1 && xhost -local: >/dev/null 2>&1 || true' EXIT

    local xauth_args=()
    [ -n "$xauth" ] && xauth_args=(-e XAUTHORITY=/tmp/.Xauthority -v "$xauth:/tmp/.Xauthority:ro")
    local tty_flags=()
    [ -t 0 ] && tty_flags=(-it)
    local ui_scale
    ui_scale="$(detect_ui_scale)"

    mkdir -p "$HERE/data/input" "$HERE/data/results"
    log "Launching the TFBoost GUI from the container (UI scale ${ui_scale}; override with TFB_UI_SCALE=..)..."
    # Overlay the live gui/ sources over the baked-in copy so GUI code edits take
    # effect without rebuilding the image (the GUI is pure Python).
    exec "$rt" run --rm "${tty_flags[@]}" \
        -e DISPLAY \
        -e TFB_UI_SCALE="$ui_scale" \
        "${xauth_args[@]}" \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        --security-opt label=disable \
        -v "$HERE/gui:/opt/TFBoost/gui:ro" \
        -v "$HERE/data:/data:Z" \
        "$IMAGE" gui
}

run_container() {
    local rt="$1"
    log "Container runtime: $rt"
    build_image_if_needed "$rt"
    if [ "$MODE" = gui ]; then run_container_gui "$rt"; else run_container_analysis "$rt"; fi
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
    for c in /opt/root/bin/thisroot.sh /usr/local/bin/thisroot.sh "${ROOTSYS:-}/bin/thisroot.sh"; do
        # shellcheck disable=SC1090
        [ -f "$c" ] && { . "$c"; break; }
    done
    command -v root-config >/dev/null 2>&1 && { log "ROOT found: $(root-config --version)"; return; }
    die "ROOT (root-config) not found. Install ROOT from https://root.cern/install/ \
(and 'source .../bin/thisroot.sh'), then re-run — or use the container route (install podman/docker)."
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
    log "Configuring and building the TBB backends (this can take a while)..."
    cmake -S "$HERE" -B "$HERE/build" -DHYDRA_INCLUDE_DIR="$HYDRA_INCLUDE_DIR"
    cmake --build "$HERE/build" -j "$(nproc)" \
        --target analysis_tbb deconvolution_tbb resampling_tbb 3Ddiamond_tbb
}

run_native() {
    if [ "$REBUILD" -eq 1 ] || [ ! -x "$HERE/build/analysis_tbb" ]; then
        install_deps_native
        ensure_root
        ensure_hydra
        build_native
    else
        log "Reusing existing native build/ (use --rebuild to reconfigure)."
        ensure_root
    fi

    if [ "$MODE" = gui ]; then
        local ui_scale; ui_scale="$(detect_ui_scale)"; export TFB_UI_SCALE="$ui_scale"
        log "Launching the TFBoost GUI (UI scale ${ui_scale}; override with TFB_UI_SCALE=..)..."
        cd "$HERE/gui"
        exec python3 TFBoostGui.py
    fi

    # --- analysis: the binary reads ../etc/configuration.cfg from build/ -------
    local cfg="${CONFIG:-$HERE/examples/config.cfg}"
    [ -f "$cfg" ] || die "Config file not found: $cfg"
    cp "$cfg" "$HERE/etc/configuration.cfg"
    local live="$HERE/etc/configuration.cfg"
    if [ -n "$INPUT" ]; then
        [ -d "$INPUT" ] || die "Input directory not found: $INPUT"
        sed -i -E "s#^[[:space:]]*InputDirectory[[:space:]]*=.*#InputDirectory = \"$(realpath "$INPUT")/\";#" "$live"
    fi
    if [ -n "$OUTPUT" ]; then
        mkdir -p "$OUTPUT/plots" "$OUTPUT/data"
        sed -i -E "s#^[[:space:]]*OutputDirectory[[:space:]]*=.*#OutputDirectory = \"$(realpath "$OUTPUT")/\";#" "$live"
    fi
    log "Running analysis natively (config: $cfg)..."
    cd "$HERE/build"
    exec ./analysis_tbb
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
        run_native
    fi
}

main
