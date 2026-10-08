# Hall C Build and Setup

## Purpose

Provide a primary Bash helper and a tcsh wrapper for building the JCE stack and Hall C, or setting
up an existing installation in the current shell.

## Main Flow

- `hallc.sh` owns all validation, build, and environment setup logic.
  `hallc.csh` invokes it in Bash and transfers the three resulting environment
  variables through a temporary file, removed after use. Failed setup leaves
  the tcsh environment unchanged.

- From the Hall C checkout, `source hallc.csh build /path/to/ROOT` clones the upstream JCE
  repository into the sibling `jana2-common-extensions` directory if absent,
  runs its superbuild, builds and installs Hall C into its checkout, and sets up
  the environment directly.
- If `JCE_SOURCE_DIR` is set, use that existing checkout instead of cloning.
  The override also applies in setup mode and must be set again in new shells.
- `source hallc.csh` only sets up the environment; it never builds.
- Repeated build requests use CMake's incremental builds. Existing JCE source
  checkouts are reused without pulling or resetting them.

## Expected Behavior

- JCE and Hall C CMake configurations default to `RelWithDebInfo` when the
  build type is unset or empty; explicit cached choices are preserved.

- The caller's working directory remains unchanged.
- Setup uses `jce-stack` inside the selected JCE checkout and the Hall C checkout as its
  installation prefix. It adds Hall C to `JANA_PLUGIN_PATH`, preserving other
  entries without duplicates. Custom prefixes require manual setup.
- Source `hallc.csh` in tcsh or `hallc.sh` in Bash to update the caller's
  environment. Both accept no arguments for setup, or `build ROOT_PREFIX` for building.
  Build mode validates the supplied ROOT prefix before cloning or building and
  forwards its package directory as `ROOT_DIR`, with the prefix and existing
  `CMAKE_PREFIX_PATH` entries passed to Hall C CMake. Hall C always builds
  `hms_rawhit` and requires ROOT Core, RIO, Tree, and Hist.
- Source the script from the Hall C checkout; afterward, run from any directory.

## Failure Behavior

- Missing JCE executable or Hall C plugin directory prints
  `source hallc.csh build /path/to/ROOT`.
- Invalid arguments or working directory print an explanation.
- Missing ROOT prefix or `ROOTConfig.cmake` fails before any build starts.
- Empty or invalid `JCE_SOURCE_DIR` prints an error without cloning.
- Failed clone, configure, build, or install stops the sequence before setup.
- Failures return a nonzero status without exiting the caller's shell.

## Key Components

- `hallc.csh` and `hallc.sh`
- `CMakeLists.txt`

## Verification

Check Bash syntax with `bash -n hallc.sh`. Verify build command order and
stop-on-failure behavior using mocked build tools. Check tcsh syntax with `tcsh -n hallc.csh`. Verify setup in a temporary
installation with unset and existing plugin paths and repeated sourcing.

The Bash equivalent of every `source hallc.csh [build ROOT_PREFIX]` invocation is
`source hallc.sh [build ROOT_PREFIX]`; use `export JCE_SOURCE_DIR=...` for its override.
