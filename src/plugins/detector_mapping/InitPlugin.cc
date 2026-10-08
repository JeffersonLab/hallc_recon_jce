#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include "InitHMSHodoscopeTranslators.h"
#include "JEventService_DetectorMappingCatalogs.h"
#include "JEventService_DetectorTranslatorsMap.h"

#include <memory>
#include <string>

class JEventService_HallCTranslatorRegistration final : public JService {
public:
    Service<JEventService_DetectorMappingCatalogs> catalogs {this};
    Service<JEventService_DetectorTranslatorsMap> translators {this};
    Parameter<std::string> config_directory {
        this, "hallc:CONFIG_DIR", HALLC_RECON_CONFIG_DIR,
        "Root directory for Hall C configuration", true
    };

    void Init() override {
        catalogs->addCatalog(
            "hallc_detector_mapping",
            config_directory() + "/hallc_detector_mapping/detector_mappings");
        InitHMSHodoscopeTranslators(translators());
    }
};

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->AddPlugin("detector_translation");
    app->ProvideService(std::make_shared<JEventService_HallCTranslatorRegistration>());
}
