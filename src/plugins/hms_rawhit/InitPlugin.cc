#include <JANA/JApplication.h>

#include "JEventProcessor_HMSRawHit.h"

extern "C" {
    void InitPlugin(JApplication* app) {
        InitJANAPlugin(app);
        app->AddPlugin("hallc_detector_mapping");
        app->Add(new JEventProcessor_HMSRawHit());
    }
}

