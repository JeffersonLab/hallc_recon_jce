# Extending Hall C Detector Translation

The generic registration, CMake, mapping, and CSV API is documented in
[JCE's detector translation guide](https://github.com/JeffersonLab/jana2-common-extensions/blob/main/src/plugins/detector_translation/TRANSLATORS.md).
Hall C owns the concrete detector code and its mapping catalog.

## Layout

```text
HMS/Hodoscope/
├── HMSHodoscopeIdentity.h/.cc
├── InitHMSHodoscopeTranslators.h/.cc
├── FADC/
│   ├── FADCTranslator.h/.cc
│   ├── FADCDumpWriter.h/.cc
│   ├── data_objects/
│   ├── tests/
│   └── CMakeLists.txt
└── FADCScaler/
    ├── FADCScalerTranslator.h/.cc
    ├── FADCScalerDumpWriter.h/.cc
    ├── data_objects/
    ├── tests/
    └── CMakeLists.txt
```

Each raw-family directory owns conversion, CSV formatting, typed data objects,
and tests. The detector parent aggregates targets and public headers; the
plugin parent installs the catalog and the `hallc_detector_data_types` interface.

## Registration

Declare the detector key once in its identity header. Conversion functions
return a single DigiHit. CSV headers and row functions live beside the conversion
and are passed together to `addTranslator<RawHit, DigiHit>()` in the detector
initializer. Call that initializer in `JEventService_HallCTranslatorRegistration::Init()`
using its translator-registry service dependency. `InitPlugin()` requests generic
translation and provides the registration service; it does not fetch services.
JANA initializes all services before first-event mapping loading and dump setup.

Link reusable JCE dependencies through exported targets such as
`jana2_common_extensions::detector_mapping_api` and
`jana2_common_extensions::evio_common_modules_data_types`.

## Configuration and Verification

Mappings belong under this plugin's `config/detector_mappings/` tree and use
its Hall C install namespace. Require validated detector fields; do not invent
physical channel or board addresses. Board-level scaler records use `none`.
Enable `BUILD_TESTING=ON` to run the route-local translator and CSV tests.
Load `hallc_detector_mapping,detector_translation_dump` for EVIO verification.
See [the behavior contract](../../../docs/hallc-detector-mapping.md).
