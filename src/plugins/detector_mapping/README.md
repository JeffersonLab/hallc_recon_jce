# hallc_detector_mapping

This plugin owns the HMS hodoscope translations and mapping catalog previously
provided by the JCE HMS example. It loads generic JCE translation and common
module parsers and provides `JEventService_HallCTranslatorRegistration` during
plugin loading. That service registers the catalog, all six typed conversions,
and CSV writers in its `Init()` through JANA service dependencies. It does not require the removed `hms_detector_translation` plugin.

Implementation lives under `HMS/Hodoscope/FADC/` and `HMS/Hodoscope/FADCScaler/`.
Each raw-family directory owns its translator, data objects, CSV functions,
CMake targets, and tests. A shared identity header defines the detector name.

Mappings under `config/detector_mappings/` install in the plugin's Hall C config
namespace. Existing mappings are retained and remain synthetic development
inputs. Supply a real scaler board address using `none` before testing scaler
translation on production data.

See [the feature contract](../../../docs/hallc-detector-mapping.md),
[the extension guide](TRANSLATORS.md), and the root README for setup and tests.
