# install/

Files that support getting TFBoost up and running. The easiest entry point is
the [`start.sh`](../start.sh) launcher at the repository root — it uses the
pieces here automatically.

- **[`Dockerfile`](Dockerfile)** — builds a self-contained image from a base that
  already ships ROOT, adds the remaining dependencies (HYDRA, TBB, TCLAP,
  libconfig, FFTW) and the Python/Tk GUI runtime, compiles the TBB backends, and
  can run a backend or launch the GUI.
- **[`docker-entrypoint.sh`](docker-entrypoint.sh)** — the image entrypoint:
  runs a chosen backend against data mounted at `/data`, or launches the GUI
  (`gui` mode), or drops to a shell.
- **[`DOCKER.md`](DOCKER.md)** — full guide to building and running the container
  (backends, the GUI over X11, build options, environment variables).

## Getting started

From the repository root:

```bash
./start.sh            # container if podman/docker is present, else native build
```

If you have `podman` or `docker`, you can also drive the image directly:

```bash
podman build -f install/Dockerfile -t tfboost .
podman run --rm -v "$PWD/data:/data" tfboost          # run the analysis backend
```

To build entirely by hand (installing ROOT and the other dependencies yourself),
see the **Dependencies** and **Manual installation and build** sections of the
top-level [README](../README.md).
