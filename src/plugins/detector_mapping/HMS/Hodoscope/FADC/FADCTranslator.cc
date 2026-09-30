#include "FADCTranslator.h"

#include <JANA/JEvent.h>

#include "HMSHodoscopeIdentity.h"

HMSHodoscopeFADCPulseDigiHit makeHMSHodoscopeFADCPulseDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        pulse.rocid, pulse.slot, pulse.chan,
        pulse.pedestal_quality, pulse.pedestal_sum, pulse.integral_sum,
        pulse.integral_quality, pulse.nsamples_above_th, pulse.coarse_time,
        pulse.fine_time, pulse.pulse_peak, pulse.time_quality
    };
}

HMSHodoscopeFADCWaveformDigiHit makeHMSHodoscopeFADCWaveformDigiHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address) {
    const auto detector = getHMSHodoscopeIdentity(address);
    return {
        detector.plane, detector.bar, detector.signal,
        waveform.rocid, waveform.slot, waveform.chan,
        waveform.waveform
    };
}

void translateHMSHodoscopeFADCPulseHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCPulseDigiHit(
        makeHMSHodoscopeFADCPulseDigiHit(pulse, address)));
}

void translateHMSHodoscopeFADCWaveformHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address,
    const JEvent& event) {
    event.Insert(new HMSHodoscopeFADCWaveformDigiHit(
        makeHMSHodoscopeFADCWaveformDigiHit(waveform, address)));
}
