#include <JANA/JApplication.h>
#include <JANA/JService.h>

#include "InitHMSHodoscopeTranslators.h"
#include "JEventService_DetectorTranslatorsMap.h"

#include <memory>

namespace {

class JEventService_HallCTranslatorRegistration final : public JService {
public:
    Service<JEventService_DetectorTranslatorsMap> translators {this};

    void Init() override {
        auto& registry = translators();

        InitHMSHodoscopeTranslators(registry);
    }

};

} // namespace

extern "C" void InitPlugin(JApplication* app) {
    InitJANAPlugin(app);
    app->AddPlugin("evio_common_modules");
    app->AddPlugin("detector_translation");
    app->ProvideService(
        std::make_shared<JEventService_HallCTranslatorRegistration>());
}
