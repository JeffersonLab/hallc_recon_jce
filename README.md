# hallc_recon

## Build example:
cmake -B build -S . -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="/Users/hanjie/Documents/JCE/JANA2;/Users/hanjie/Documents/JCE/evio/Darwin-arm64;/Users/hanjie/Documents/JCE/jana2-common-extensions/install" -DCMAKE_INSTALL_PREFIX="/Users/hanjie/Documents/JCE/hallc_recon/install"

## Running command

`hallc_detector_mapping` registers Hall C translators. JCE's
`detector_translation` service reads the mapping files. It starts at
`config/evio_parser/detector_mappings/manifest.map` when `JCE_CONFIG_DIR`
points to this project's `config` directory.

From the repository root, `source sourceme.csh` sets `JCE_CONFIG_DIR` to the
source `config` directory. To select only the Hall C detector maps without
changing JCE's bank, filter, or default-plugin files, pass:

```text
-PTRANSLATION:DIRECTORY=/Users/hanjie/Documents/JCE/hallc_recon/config/evio_parser/detector_mappings
```

`cmake --install build` installs the same maps under
`install/config/evio_parser/detector_mappings`. Point the parameter there for
an installed run. The current HMS map is synthetic demonstration data; replace
it with verified, run-specific Hall C channel assignments before interpreting
physics output.
