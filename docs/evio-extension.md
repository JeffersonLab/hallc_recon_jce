# Hall C EVIO Extension

## Purpose

Keep Hall C-owned detector translation code and configuration outside
`jana2-common-extensions` while consuming its installed extension APIs.

## Main Flow

1. `hallc_detector_mapping` requests the reusable module and generic detector
   translation plugins.
2. It registers HMS Hodoscope translators with the shared translator service.
3. It registers the Hall C mapping catalog before translation services start.
4. The generic processor converts mapped FADC raw hits into Hall C DigiHits.

## Expected Behavior

- Hall C links only namespaced targets exported by `jana2-common-extensions`.
- Hall C owns and installs its detector mapping catalog.
- The JANA parameter `hallc:CONFIG_DIR` overrides the compiled installation
  config root with `-Phallc:CONFIG_DIR=/path/to/config`.
- Mapping files remain synthetic development data until validated replacements
  are supplied.

## Failure Behavior

- Missing or invalid Hall C mapping configuration fails during translation
  service initialization.
- Duplicate detector catalogs or translator routes fail immediately.

## Key Components

- `src/plugins/detector_mapping/`
- `src/plugins/detector_mapping/config/detector_mappings/`

## Verification

- Configure against an installed `jana2-common-extensions` package.
- Load `hallc_detector_mapping` and confirm its catalog is accepted.
- Process representative FADC data and confirm HMS Hodoscope DigiHits appear.
