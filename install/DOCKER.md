# Running TFBoost in a container

A single image that bundles everything TFBoost needs — ROOT (from the base
image, **not** recompiled), the HYDRA headers, TBB, TCLAP, libconfig and FFTW —
builds the TBB backends, and runs a chosen target against input data you mount
at `/data`.

Works with both `podman` and `docker` (the commands below use `podman`; swap in
`docker` if you prefer).

## Build the image

Run from the **repository root** (the build context must be the repo, while the
Dockerfile lives in `install/`):

```bash
podman build -f install/Dockerfile -t tfboost .
```

This pulls a ROOT base image, clones HYDRA, builds:
`analysis_tbb deconvolution_tbb resampling_tbb 3Ddiamond_tbb
analyze_single_signal_tbb simple_analysis_tbb`.

### Build options (`--build-arg`)

| Arg | Default | Purpose |
|-----|---------|---------|
| `BASE_IMAGE` | `rootproject/root:latest` | ROOT-carrying base image; pin a tag for reproducibility. |
| `HYDRA_REPO` | `…/MultithreadCorner/Hydra.git` | HYDRA source repo. |
| `HYDRA_REF`  | `master` | HYDRA branch/tag (verified build used 4.0.1). |
| `TFB_MARCH`  | `native` | CPU target. **See portability note below.** |
| `TFB_TARGETS`| the 6 targets above | Space-separated list of targets to compile. |

> **Portability note.** `CMakeLists.txt` forces `-march=native`, so the binaries
> are tuned to the **build host's** CPU and may crash with *illegal instruction*
> on a different machine. For a same-machine image the default is fine. To make a
> portable image, build with e.g. `--build-arg TFB_MARCH=x86-64-v2` (the
> Dockerfile patches the flag inside the image only).

## Run

Mount a host folder at `/data`. Outputs are written under `/data/results` by
default, so they persist after the container exits.

```bash
# Default: runs analysis_tbb. Put input signals in ./data/input to have them
# picked up automatically; results appear in ./data/results.
podman run --rm -v "$PWD/data:/data" tfboost
```

```bash
# Point at an arbitrary input directory inside the mount:
podman run --rm -v "$PWD/data:/data" -e TFB_INPUT=/data/signals tfboost

# Run a different target:
podman run --rm -v "$PWD/data:/data" tfboost deconvolution_tbb

# Use your own config file (mounted in):
podman run --rm -v "$PWD/data:/data" -e TFB_CONFIG=/data/my.cfg tfboost

# Drop into a shell for debugging:
podman run --rm -it -v "$PWD/data:/data" tfboost shell
```

### Launching the GUI from the container

The image also carries the Python/Tk front-end. `./start.sh` is the easy path
(it detects the runtime, builds the image and wires up X11). To do it by hand you
must give the container access to the host X server:

```bash
xhost +local:                               # allow local X connections
podman run --rm \
    -e DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    --security-opt label=disable \          # needed on SELinux (Fedora); harmless elsewhere
    -v "$PWD/data:/data" \
    tfboost gui
xhost -local:                               # revoke when done
```

Notes / troubleshooting:
- `--security-opt label=disable` lets a rootless-podman container reach the X
  socket on SELinux systems; on Docker/non-SELinux it is a harmless no-op.
- On Wayland this uses XWayland (the `/tmp/.X11-unix` socket), which works for
  Tkinter. If windows do not appear, check `echo $DISPLAY` is set on the host and
  that `xhost +local:` succeeded.
- Use `./start.sh` to avoid getting these flags right by hand.

### Runtime environment variables

| Var | Default | Effect |
|-----|---------|--------|
| `TFB_TARGET` | `analysis_tbb` (or 1st CLI arg) | Which executable to run. |
| `TFB_CONFIG` | baked example config | Config file to use; copied into the path the binary reads. |
| `TFB_INPUT`  | `/data/input` if non-empty, else the config value | Input-signals directory. |
| `TFB_OUTPUT` | `/data/results` | Output directory (`plots/` + `data/` are created inside). |

The entrypoint rewrites `InputDirectory` / `OutputDirectory` in the active
`configuration.cfg` (the file `analysis_tbb` reads). Other targets read their own
`etc/*.cfg`; for those, supply paths via `TFB_CONFIG` or mount an edited `etc/`.

With no `/data` mount and no overrides the image still runs end-to-end against
the bundled `examples/input_files`, writing to `/data/results` inside the
(ephemeral) container — handy as a smoke test:

```bash
podman run --rm tfboost
```

## Verified toolchain

The reference build this image mirrors used ROOT 6.40.02, HYDRA 4.0.1 and the
system GCC with `--std=c++20`. Any ROOT ≥ 6.14 in the base image is expected to
work.
