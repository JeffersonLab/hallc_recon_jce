# Hall C Detector Mapping

## Purpose

`hallc_detector_mapping` owns Hall C mapping catalogs, detector translators,
DigiHit schemas, and CSV row formatting. JCE supplies generic translation and
dumping infrastructure, hardware decoding, and common raw-hit types.

## Main Flow

1. The plugin requests `evio_common_modules` and `detector_translation`.
2. During plugin loading it provides `JEventService_HallCTranslatorRegistration`.
   That service registers its catalog and all six HMS routes, including each
   route's CSV header and row function, in its `Init()`.
3. JCE selects run-dependent mappings and publishes typed DigiHits from the
   conversion results. Optional `detector_translation_dump` writes those outputs.

## Expected Behavior

- Five FADC formats (pulse, waveform, Hall-B integral, time, peak) and the
  board-level FADC scaler each have one translation route and one CSV writer.
- The shared `HMS_HODOSCOPE_DETECTOR_NAME` is used for registration and identity
  validation. All routes require `plane`, `bar`, and `signal` mapping fields.
- DigiHits copy detector identity and digitized payloads without calibration
  or geometry. FADC hits retain module, trigger, and timestamp metadata.
- Scaler translation copies all 16 counters; its mapping row uses `none` in
  the DAQ channel column. The synthetic catalog does not guess board addresses.
- Headers are exported through `hallc_detector_data_types`; core JCE does not
  provide HMS data types. Existing consumers must rebuild for expanded schemas.
- Catalogs install under `config/<namespace>/hallc_detector_mapping/detector_mappings/`.
  The JANA parameter `hallc:CONFIG_DIR` selects the parent config tree and defaults
  to the compiled installation config root. Set it with `-Phallc:CONFIG_DIR=/path/to/config`;
  `TRANSLATION:DIRECTORY` explicitly overrides registered catalogs through JCE.
- CSVs use `HMS_HODOSCOPE/<RawHitType>.csv`. Formatting functions live beside
  the translators; output paths are derived by core. No files are opened until
  the generic dump processor is enabled.
- Current mappings are synthetic development data, not production-approved.

## Failure Behavior

Invalid detector identities or missing required fields fail. Core rejects
invalid catalogs, duplicate routes, missing writers, and late registration.
JANA initializes the registration service before event processing. Core freezes
routes, loads mappings, and opens dump files on the first event, so processor and
plugin initialization order cannot exclude Hall C routes. Mapping errors surface
on the first translation event; no events means no dump files.

## Key Components

- `src/plugins/detector_mapping/InitPlugin.cc`
- `src/plugins/detector_mapping/HMS/Hodoscope/`
- `src/plugins/detector_mapping/config/detector_mappings/`

## Verification

Enable `BUILD_TESTING=ON` when configuring Hall C. The route-local translator
and dump-writer tests cover all six formats, detector metadata validation,
typed event insertion, generated filenames, CSV headers, column order, and
waveform/scaler array formatting. JCE's generic registry tests cover route
isolation and mandatory writer registration. An EVIO integration run must load
`hallc_detector_mapping,detector_translation_dump` and compare known channels
with the approved mapping.
