#include "InitHMSHodoscopeTranslators.h"

#include "FADC250PulseHit.h"
#include "FADC250WaveformHit.h"
#include "HMSHodFADCTranslator.h"
#include "JEventService_DetectorTranslatorsMap.h"

void InitHMSHodoscopeTranslators(
    JEventService_DetectorTranslatorsMap& translators) {
    translators.addTranslator<FADC250PulseHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCPulseHit);
    translators.addTranslator<FADC250WaveformHit>(
        "HMS_HODOSCOPE",
        translateHMSHodoscopeFADCWaveformHit);
}
