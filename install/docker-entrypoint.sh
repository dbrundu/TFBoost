#!/bin/bash
###############################################################################
# TFBoost container entrypoint.
#
# Runs a TFBoost target from the build directory (binaries read a hardcoded
# ../etc/<name>.cfg path, so the CWD must be build/). Input/output locations are
# steered toward the mounted /data volume so results survive the container.
#
# Environment variables (all optional):
#   TFB_TARGET   executable to run            (default: analysis_tbb, or 1st arg)
#   TFB_CONFIG   config file to use           (default: baked example config)
#   TFB_INPUT    input-signals directory      (default: /data/input if non-empty,
#                                              else the config's own value)
#   TFB_OUTPUT   output directory             (default: /data/results)
#
# Special first-argument modes:
#   gui           launch the Tkinter GUI (needs an X server; see start.sh)
#   shell / bash  drop into an interactive shell
#
# Examples:
#   podman run --rm -v "$PWD/data:/data" tfboost
#   podman run --rm -v "$PWD/data:/data" -e TFB_INPUT=/data/signals tfboost
#   podman run --rm -v "$PWD/data:/data" tfboost deconvolution_tbb
#   podman run --rm -it -v "$PWD/data:/data" tfboost shell
###############################################################################
set -euo pipefail

TFB_HOME="${TFB_HOME:-/opt/TFBoost}"
BUILD_DIR="${TFB_HOME}/build"

# --- argument handling --------------------------------------------------------
case "${1:-}" in
    bash|sh|shell)        exec /bin/bash ;;
    gui)
        # The GUI writes ../etc/*.cfg and launches ../build binaries relative to
        # its own directory, so it must run from ${TFB_HOME}/gui.
        cd "${TFB_HOME}/gui"
        exec python3 TFBoostGui.py ;;
    help|-h|--help)
        sed -n '3,33p' "$0"
        exit 0 ;;
    "" )                  : ;;                      # use TFB_TARGET / default
    -* )                  : ;;                      # leading flag: leave for binary
    * )   TFB_TARGET="$1"; shift ;;                 # explicit target name
esac

: "${TFB_TARGET:=analysis_tbb}"
: "${TFB_CONFIG:=${TFB_HOME}/etc/configuration.cfg}"
: "${TFB_OUTPUT:=/data/results}"

if [[ ! -x "${BUILD_DIR}/${TFB_TARGET}" ]]; then
    echo "ERROR: target '${TFB_TARGET}' was not built into this image." >&2
    echo "Available targets:" >&2
    find "${BUILD_DIR}" -maxdepth 1 -type f -executable -printf '  %f\n' | sort >&2
    exit 1
fi

# --- config: copy chosen config into the path the binary actually reads -------
LIVE_CFG="${TFB_HOME}/etc/configuration.cfg"
if [[ "${TFB_CONFIG}" != "${LIVE_CFG}" ]]; then
    if [[ ! -f "${TFB_CONFIG}" ]]; then
        echo "ERROR: TFB_CONFIG '${TFB_CONFIG}' does not exist." >&2
        exit 1
    fi
    cp "${TFB_CONFIG}" "${LIVE_CFG}"
fi

# --- input: auto-pick /data/input when populated, unless overridden -----------
if [[ -z "${TFB_INPUT:-}" && -d /data/input ]] && [[ -n "$(ls -A /data/input 2>/dev/null)" ]]; then
    TFB_INPUT=/data/input
fi

if [[ -n "${TFB_INPUT:-}" ]]; then
    if [[ ! -d "${TFB_INPUT}" ]]; then
        echo "ERROR: TFB_INPUT '${TFB_INPUT}' is not a directory." >&2
        exit 1
    fi
    sed -i -E "s#^[[:space:]]*InputDirectory[[:space:]]*=.*#InputDirectory = \"${TFB_INPUT%/}/\";#" "${LIVE_CFG}"
fi

# --- output: always land in the mounted volume by default ---------------------
# Pre-create the plots/ and data/ subdirs: TFBoost's CreateDirectories() builds
# paths relative to the CWD (it drops a leading '/'), so it cannot create these
# under an absolute OutputDirectory. Making them here lets the absolute-path
# writes (SaveConvToFile, histograms) succeed.
mkdir -p "${TFB_OUTPUT}/plots" "${TFB_OUTPUT}/data"
sed -i -E "s#^[[:space:]]*OutputDirectory[[:space:]]*=.*#OutputDirectory = \"${TFB_OUTPUT%/}/\";#" "${LIVE_CFG}"

echo "TFBoost container"
echo "  target : ${TFB_TARGET}"
echo "  config : ${TFB_CONFIG}"
echo "  input  : ${TFB_INPUT:-<from config>}"
echo "  output : ${TFB_OUTPUT}"
echo

cd "${BUILD_DIR}"
exec "./${TFB_TARGET}" "$@"
