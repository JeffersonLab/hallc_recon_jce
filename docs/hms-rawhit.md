# HMS Raw-Hit ROOT Output

## Purpose

The `hms_rawhit` plugin writes uncalibrated HMS hodoscope pulse and
waveform DigiHits to ROOT using the tree and branch layout from Hanjie's plugin.

## Main Flow

1. Build Hall C with ROOT, then load `hms_rawhit`.
2. The plugin requests `hallc_detector_mapping` and registers its processor.
3. The processor groups hits by plane, bar, and signal end and fills tree `T`
   once per processed event. JANA serializes its `ProcessSequential` callback.
4. At finish it writes the tree and closes the output file.

## Expected Behavior

- Always built; Hall C requires ROOT Core, RIO, Tree, and Hist.
- `ROOT_OUT_FILENAME` selects the output; default is `HMS_rawhits.root`.
  Opening uses ROOT's `RECREATE` mode, replacing any existing file at that path.
- Plane order is 1x, 2x, 1y, 2y; signal 0 is positive and 1 is negative.
- Branch prefixes are `H.hod.<plane>.<pos|neg>Adc`. Suffixes are `Counter`, `Ped`,
  `PedQuality`, `Nhits`, `Waveform`, `Integral`, `IntegralQuality`,
  `IntegralNsample`, `CoarseTime`, `FineTime`, `TimeQuality`, and `PulsePeak`.
- One counter, pedestal, pedestal quality, and pulse count are stored per bar.
  Pedestal values come from the first pulse, or zero for waveform-only bars.
- Pulse measurements and waveform samples are flattened in first-seen bar order.
  This preserves the source layout; no waveform lengths or offsets are added.
- Missing pulse or waveform collections are allowed. Empty events produce an
  empty tree entry. Buffers are cleared between events.
- This processor consumes combined pulse and waveform DigiHits only; separate
  Hall-B integral/time/peak formats and scalers are not written.
- Mapping catalogs remain synthetic until replaced with validated Hall C maps.

## Failure Behavior

An unusable output file, plane outside 1–4, bar below 1, signal outside 0–1,
or multiple waveforms for the same plane/bar/signal causes an exception.

## Key Components

- `src/plugins/hms_rawhit/`
- `src/plugins/detector_mapping/`

## Verification

Configure with `BUILD_TESTING=ON`, build, then run
`ctest --test-dir build -R '^hms_rawhit_tests$' --output-on-failure`.
The test checks all plane/end routes, multiple pulses per bar, waveform content,
empty-event buffer clearing, duplicate-waveform rejection, and saved ROOT output.
An EVIO integration run with validated channel mappings remains necessary.
