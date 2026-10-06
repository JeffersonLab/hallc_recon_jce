#pragma once

#include "DetectorAddress.h"
#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "HMSHodoscopeFADCPulseDigiHit.h"
#include "HMSHodoscopeFADCWaveformDigiHit.h"

class JEvent;

HMSHodoscopeFADCPulseDigiHit makeHMSHodoscopeFADCPulseDigiHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address);

HMSHodoscopeFADCWaveformDigiHit makeHMSHodoscopeFADCWaveformDigiHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address);

void translateHMSHodoscopeFADCPulseHit(
    const FADC250PulseHit& pulse,
    const DetectorAddress& address,
    const JEvent& event);

void translateHMSHodoscopeFADCWaveformHit(
    const FADC250WaveformHit& waveform,
    const DetectorAddress& address,
    const JEvent& event);
