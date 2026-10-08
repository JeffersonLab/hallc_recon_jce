#include "JEventProcessor_HMSRawHit.h"

#include <JANA/JApplication.h>
#include <JANA/JEvent.h>
#include <JANA/JException.h>
#include <TFile.h>
#include <TTree.h>

#include <array>
#include <filesystem>
#include <stdexcept>

static void require(bool condition) {
    if (!condition) throw std::runtime_error("HMS raw-hit output check failed");
}

int main() {
    const std::string filename = "hms_rawhit_test.root";
    JApplication app;
    app.SetParameterValue("ROOT_OUT_FILENAME", filename);
    auto* processor = new JEventProcessor_HMSRawHit;
    app.Add(processor);
    app.Initialize();
    processor->DoInit();

    JEvent event(&app);
    event.SetRunNumber(1);
    for (int plane = 1; plane <= 4; ++plane) {
        for (int signal = 0; signal <= 1; ++signal) {
            for (int i = 0; i < 2; ++i) {
                auto* pulse = new HMSHodoscopeFADCPulseDigiHit{};
                pulse->plane = plane;
                pulse->bar = 3;
                pulse->signal = signal;
                pulse->pedestal_sum = 20;
                pulse->integral_sum = 100 + i;
                event.Insert(pulse);
            }
            auto* waveform = new HMSHodoscopeFADCWaveformDigiHit{};
            waveform->plane = plane;
            waveform->bar = 3;
            waveform->signal = signal;
            waveform->waveform = {10, 11, 12};
            event.Insert(waveform);
        }
    }
    processor->DoMap(event);
    processor->DoTap(event);

    JEvent empty(&app);
    empty.SetRunNumber(1);
    processor->DoMap(empty);
    processor->DoTap(empty);

    JEvent duplicate(&app);
    duplicate.SetRunNumber(1);
    for (int i = 0; i < 2; ++i) {
        auto* waveform = new HMSHodoscopeFADCWaveformDigiHit{};
        waveform->plane = 1;
        waveform->bar = 3;
        duplicate.Insert(waveform);
    }
    bool rejected = false;
    try {
        processor->DoMap(duplicate);
        processor->DoTap(duplicate);
    } catch (const JException&) {
        rejected = true;
    }
    require(rejected);
    processor->DoFinalize();

    {
        TFile file(filename.c_str(), "READ");
        require(!file.IsZombie());
        auto* tree = file.Get<TTree>("T");
        require(tree != nullptr && tree->GetEntries() == 2);
        const std::array<std::string, 8> prefixes {
            "H.hod.1x.posAdc", "H.hod.1x.negAdc",
            "H.hod.2x.posAdc", "H.hod.2x.negAdc",
            "H.hod.1y.posAdc", "H.hod.1y.negAdc",
            "H.hod.2y.posAdc", "H.hod.2y.negAdc"
        };
        for (const auto& prefix : prefixes) {
            std::vector<uint32_t>* counters = nullptr;
            std::vector<uint32_t>* counts = nullptr;
            std::vector<uint32_t>* integrals = nullptr;
            std::vector<uint32_t>* samples = nullptr;
            require(tree->SetBranchAddress((prefix + "Counter").c_str(), &counters) >= 0);
            require(tree->SetBranchAddress((prefix + "Nhits").c_str(), &counts) >= 0);
            require(tree->SetBranchAddress((prefix + "Integral").c_str(), &integrals) >= 0);
            require(tree->SetBranchAddress((prefix + "Waveform").c_str(), &samples) >= 0);
            tree->GetEntry(0);
            require(*counters == std::vector<uint32_t>{3});
            require(*counts == std::vector<uint32_t>{2});
            require(*integrals == std::vector<uint32_t>({100, 101}));
            require(*samples == std::vector<uint32_t>({10, 11, 12}));
            tree->GetEntry(1);
            require(counters->empty() && counts->empty() && integrals->empty() && samples->empty());
            tree->ResetBranchAddresses();
            delete counters;
            delete counts;
            delete integrals;
            delete samples;
        }
    }
    std::filesystem::remove(filename);
}
