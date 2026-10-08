# hallc_recon_jce

Hall C reconstruction extensions for `jana2-common-extensions`.

## Dependencies

Install these prerequisites before building:

| Dependency | Requirement |
|------------|-------------|
| CMake | 3.22 or newer |
| C++ compiler | C++20; GCC 11+ or Clang 13+ recommended |
| Git | Required to clone JCE and download its dependencies |
| Boost and LZ4 | Required by EVIO; installed on the system |
| tcsh | Shell used by the commands below |
| ROOT | Required; Core, RIO, Tree, and Hist components |

The JCE superbuild downloads and installs pinned JANA2 `v2026.03.01`, EVIO
`v6.1.2`, and JCE together. Hall C requires an existing ROOT installation.

## Build Instructions

Use `RelWithDebInfo` for optimized processing with debug symbols for profiling.
`Release` is also suitable for optimized runs. An empty CMake build type is not
equivalent to an optimized build. The JCE superbuild forwards the selected type
to JANA2, EVIO, and JCE; rebuild and install after changing it.
CMake defaults to `RelWithDebInfo` when no build type is selected; an explicit
build type in the CMake cache is preserved.

Start in your cloned `hallc_recon_jce` directory, in **Bash**:

```bash
source hallc.sh build /path/to/ROOT
```

For **tcsh**, use `source hallc.csh build /path/to/ROOT`. It delegates to `hallc.sh` and
applies the resulting environment to your tcsh session. For an existing
installation, omit `build`. Set a custom checkout with
`export JCE_SOURCE_DIR=/path/to/jana2-common-extensions` in Bash or
`setenv JCE_SOURCE_DIR /path/to/jana2-common-extensions` in tcsh.
Both helpers must be sourced from the Hall C checkout. Build mode requires the
ROOT installation prefix containing `ROOTConfig.cmake`; setup mode takes no arguments.
In Bash, run JCE through `"${JCE_HOME}/scripts/jce.sh"`.

This clones JCE alongside Hall C if needed, builds and installs the JCE stack,
builds and installs Hall C into the current directory, and sets up your shell.
Run it again after code changes; CMake handles incremental builds. If a command
fails, the script stops so you can fix the reported error and rerun it.

To use an existing JCE checkout instead of cloning:

```tcsh
setenv JCE_SOURCE_DIR /path/to/jana2-common-extensions
source hallc.csh build /path/to/ROOT
```

The checkout must contain `superbuild/CMakeLists.txt`. The script builds its
stack into `JCE_SOURCE_DIR/jce-stack`. To return to the default sibling checkout,
run `unsetenv JCE_SOURCE_DIR`.

In subsequent tcsh sessions, from the same Hall C directory:

```tcsh
source hallc.csh
```

This only sets up the installed environment. If Hall C has not been installed,
it prints the build command. Use `source` so environment changes apply to your
current shell. If you used a custom checkout, set `JCE_SOURCE_DIR` to that
same path again in each new shell before sourcing the script.

### Manual build (optional)

The following commands perform the same steps individually, in the same shell.

#### 1. Clone JCE

Save the Hall C directory, then clone JCE alongside it:

```tcsh
setenv HALLC_RECON_HOME "$cwd"
cd ..
git clone https://github.com/JeffersonLab/jana2-common-extensions.git
cd jana2-common-extensions
```

#### 2. Build and install the JCE stack

```tcsh
cmake -S superbuild -B build-super
cmake --build build-super --parallel
setenv JCE_HOME `pwd`/jce-stack
```

The build installs JANA2, EVIO, and JCE into `jce-stack/` automatically.
See the [JCE build instructions](https://github.com/JeffersonLab/jana2-common-extensions#build-instructions)
for additional build options.

#### 3. Build and install Hall C

Return to the Hall C directory and use that directory as the installation
prefix:

```tcsh
cd "$HALLC_RECON_HOME"
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH="$JCE_HOME;/path/to/ROOT" \
  -DCMAKE_INSTALL_PREFIX="$cwd"
cmake --build build --parallel
cmake --install build
```

Hall C plugins are installed under `lib/plugins/`, public headers under
`include/plugins/`, and detector mapping files under
`config/hallc_detector_mapping/detector_mappings/` in this directory.

The installation prefix is compiled into the detector-mapping plugin as its
default configuration root. If you change it, configure and build again before
installing.

## Running the Plugins

After `source hallc.csh build /path/to/ROOT` or `source hallc.csh`, replace `/path/to/data.evio` with your EVIO file path:

```tcsh
"${JCE_HOME}/scripts/jce.csh" -Pplugins=hallc_detector_mapping /path/to/data.evio
```

The script sets `JCE_HOME` to `jce-stack` inside the selected JCE checkout and `HALLC_RECON_HOME` to the
current Hall C checkout. It adds Hall C's plugin directory to
`JANA_PLUGIN_PATH`, preserving existing entries without duplicates. You can
then run from any working directory. In a new shell, return to the Hall C
checkout and run `source hallc.csh` again.

The helper installs JCE into its checkout's `jce-stack` directory and Hall C
into the Hall C checkout. For custom installation prefixes,
use the manual build commands with your chosen paths and set `JCE_HOME`,
`HALLC_RECON_HOME`, and `JANA_PLUGIN_PATH` accordingly.

The JCE wrapper finds `jana` and adds the JCE plugin directory automatically.
The Hall C plugin requests its required JCE plugins.

To use a configuration tree outside the installation directory:

```tcsh
"${JCE_HOME}/scripts/jce.csh" -Pplugins=hallc_detector_mapping -Phallc:CONFIG_DIR=/path/to/config /path/to/data.evio
```

The override directory must contain
`hallc_detector_mapping/detector_mappings/manifest.map`.

## Plugins

The `hallc_detector_mapping` plugin loads the generic `detector_translation`
pipeline, registers HMS Hodoscope translators, and contributes its own mapping
catalog. Its installed catalog is used by default; set the JANA parameter `hallc:CONFIG_DIR`
to the parent configuration directory when running from another location.

The current catalog contains synthetic development entries. Production use
must replace them with validated Hall C channel mappings.

## HMS Raw-Hit ROOT Output

The migrated Hanjie HMS raw-hit processor is always built. To enable its tests:

```tcsh
cmake -S . -B build -DBUILD_TESTING=ON \
  -DCMAKE_PREFIX_PATH="$JCE_HOME;/path/to/ROOT" -DCMAKE_INSTALL_PREFIX="$cwd"
cmake --build build --parallel
ctest --test-dir build -R '^hms_rawhit_tests$' --output-on-failure
cmake --install build
```

After sourcing the Hall C setup helper, run:

```tcsh
"${JCE_HOME}/scripts/jce.csh" -Pplugins=hms_rawhit \
  -PROOT_OUT_FILENAME=HMS_rawhits.root /path/to/data.evio
```

The plugin loads `hallc_detector_mapping` automatically. It writes tree `T`
with the original `H.hod.*Adc*` branches, replacing the selected output file.
See [the raw-hit output contract](docs/hms-rawhit.md) for grouping and format limits.

## Translation and Diagnostic Tests

Hall C owns five HMS FADC translations and one FADC scaler translation,
together with their DigiHit schemas and CSV formatters. It registers them
through a JANA registration service in its `Init()`. Rebuild against the matching
JCE checkout; the old `hms_detector_translation` plugin and its data-types
export are no longer supplied by core.

To configure the focused tests on your test system:

```tcsh
cmake -S . -B build -DBUILD_TESTING=ON \
  -DCMAKE_PREFIX_PATH="$JCE_HOME;/path/to/ROOT" -DCMAKE_INSTALL_PREFIX="$cwd"
cmake --build build --parallel
ctest --test-dir build -R '^hms_hodoscope_fadc(_scaler)?_(translator|dump_writer)_tests$' --output-on-failure
```

To inspect translated output after sourcing `hallc.csh`:

```tcsh
"${JCE_HOME}/scripts/jce.csh" \
  -Pplugins=hallc_detector_mapping,detector_translation_dump \
  -Pdetector_translation_dump:OUTPUT_DIRECTORY=detector_translation_dump \
  /path/to/data.evio
```

Files are named `HMS_HODOSCOPE/<RawHitType>.csv`. The headers and formatting
match the migrated implementation. Scaler mapping rows require the actual
board ROC and slot with channel `none`; the development catalog does not add
an invented board address. See [the mapping feature contract](docs/hallc-detector-mapping.md).
