#include "JEventProcessor_HMSRawHit.h"
#include <JANA/JException.h>
#include <JANA/JLogger.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace {

struct ChannelHits {
    std::int32_t bar;
    std::vector<const HMSHodoscopeFADCPulseDigiHit*> pulses;
    const HMSHodoscopeFADCWaveformDigiHit* waveform = nullptr;
};

} // namespace

void SetHodADCBranch(TTree* tree, const std::string& prefix, HodADCBranches& data) {
    tree->Branch((prefix + "Counter").c_str(),         &data.counter);
    tree->Branch((prefix + "Ped").c_str(),             &data.ped);
    tree->Branch((prefix + "PedQuality").c_str(),      &data.ped_quality);
    tree->Branch((prefix + "Nhits").c_str(),           &data.nhits);
    tree->Branch((prefix + "Waveform").c_str(),        &data.waveform);
    tree->Branch((prefix + "Integral").c_str(),        &data.integral_sum);
    tree->Branch((prefix + "IntegralQuality").c_str(), &data.integral_quality);
    tree->Branch((prefix + "IntegralNsample").c_str(), &data.integral_nsample);
    tree->Branch((prefix + "CoarseTime").c_str(),      &data.coarse_time);
    tree->Branch((prefix + "FineTime").c_str(),        &data.fine_time);
    tree->Branch((prefix + "TimeQuality").c_str(),     &data.time_quality);
    tree->Branch((prefix + "PulsePeak").c_str(),       &data.pulse_peak);
}

/**
 * @brief Constructor for JEventProcessor_HMSRawHit
 * 
 * Initialize the processor with the appropriate type name, prefix, and callback style.
 */
JEventProcessor_HMSRawHit::JEventProcessor_HMSRawHit() {
    SetTypeName(NAME_OF_THIS);                    // Provide JANA with this class's name
    SetPrefix("jeventprocessor_hmsrawhit");            // Set unique prefix for parameters
    SetCallbackStyle(CallbackStyle::ExpertMode);  // Use expert mode for full control

    // All of these are optional because not all events will have these hits
    m_fadcPulses.SetOptional(true);
    m_fadcWaveforms.SetOptional(true);
}

/**
 * @brief Initialize the processor
 * 
 * Called once at the start of processing. Open the output files and set up
 * any necessary resources for event processing.
 */
void JEventProcessor_HMSRawHit::Init() {
    LOG << "JEventProcessor_HMSRawHit::Init" << LOG_END;
    
    // Open the ROOT output file
    m_root_output_file = new TFile(m_root_output_filename().c_str(), "RECREATE");
    if (m_root_output_file == nullptr || m_root_output_file->IsZombie()) {
        throw JException("Failed to open ROOT output file: " + m_root_output_filename());  
    }

    // Create ROOT tree
    T = new TTree("T", "HMS Raw Hits Tree");

    SetHodADCBranch(T, "H.hod.1x.posAdc", HMSHodADCPos1x);
    SetHodADCBranch(T, "H.hod.1x.negAdc", HMSHodADCNeg1x);
    SetHodADCBranch(T, "H.hod.1y.posAdc", HMSHodADCPos1y);
    SetHodADCBranch(T, "H.hod.1y.negAdc", HMSHodADCNeg1y);
    SetHodADCBranch(T, "H.hod.2x.posAdc", HMSHodADCPos2x);
    SetHodADCBranch(T, "H.hod.2x.negAdc", HMSHodADCNeg2x);
    SetHodADCBranch(T, "H.hod.2y.posAdc", HMSHodADCPos2y);
    SetHodADCBranch(T, "H.hod.2y.negAdc", HMSHodADCNeg2y);

}

/**
 * @brief Process a single event sequentially
 * 
 * Processes FADC250 detector data for a single event. Fills ROOT tree with
 * waveform data and histogram with pulse integral values. This method is 
 * called for each event in the processing pipeline.
 * 
 * @param event Reference to the JANA2 event to process
 */
void JEventProcessor_HMSRawHit::ProcessSequential(const JEvent &event) {
    // Route order matches the plane numbers in the detector mapping: 1x, 2x,
    // 1y, 2y. Within each plane, signal 0 is positive and 1 is negative.
    const std::array<HodADCBranches*, 8> branches {
        &HMSHodADCPos1x, &HMSHodADCNeg1x,
        &HMSHodADCPos2x, &HMSHodADCNeg2x,
        &HMSHodADCPos1y, &HMSHodADCNeg1y,
        &HMSHodADCPos2y, &HMSHodADCNeg2y
    };
    std::array<std::unordered_map<std::int32_t, std::size_t>, 8> channelIndices;
    std::array<std::vector<ChannelHits>, 8> groupedChannels;
    for (auto* branch : branches) {
        branch->clear();
    }

    // Find or create one channel entry for (plane, bar, signal). Store pointers
    // during grouping; copy measurements only once into the final ROOT buffers.
    const auto channel = [&](std::int32_t plane, std::int32_t bar,
                             std::int32_t signal) -> ChannelHits& {
        if (plane < 1 || plane > 4 || bar < 1 || signal < 0 || signal > 1) {
            throw JException(
                "Invalid HMS hodoscope plane/bar/signal: %d/%d/%d",
                plane, bar, signal);
        }
        const auto route = static_cast<std::size_t>((plane - 1) * 2 + signal);
        auto& channels = groupedChannels[route];
        auto [it, inserted] = channelIndices[route].try_emplace(
            bar, channels.size());
        if (inserted) {
            channels.push_back(ChannelHits{bar});
        }
        return channels[it->second];
    };

    for (const auto* pulse : m_fadcPulses()) {
        channel(pulse->plane, pulse->bar, pulse->signal).pulses.push_back(pulse);
    }

    for (const auto* waveform : m_fadcWaveforms()) {
        auto& hit = channel(waveform->plane, waveform->bar, waveform->signal);
        if (hit.waveform != nullptr) {
            throw JException(
                "Multiple HMS hodoscope waveforms for plane/bar/signal %d/%d/%d",
                waveform->plane, waveform->bar, waveform->signal);
        }
        hit.waveform = waveform;
    }

    for (std::size_t route = 0; route < branches.size(); ++route) {
        auto& output = *branches[route];
        for (const auto& hit : groupedChannels[route]) {
            output.counter.push_back(static_cast<std::uint32_t>(hit.bar));
            output.nhits.push_back(static_cast<std::uint32_t>(hit.pulses.size()));
            output.ped.push_back(
                hit.pulses.empty() ? 0 : hit.pulses.front()->pedestal_sum);
            output.ped_quality.push_back(
                hit.pulses.empty() ? 0 : hit.pulses.front()->pedestal_quality);

            for (const auto* pulse : hit.pulses) {
                output.integral_sum.push_back(pulse->integral_sum);
                output.integral_quality.push_back(pulse->integral_quality);
                output.integral_nsample.push_back(
                    pulse->nsamples_above_threshold);
                output.coarse_time.push_back(pulse->coarse_time);
                output.fine_time.push_back(pulse->fine_time);
                output.time_quality.push_back(pulse->time_quality);
                output.pulse_peak.push_back(pulse->pulse_peak);
            }

            if (hit.waveform != nullptr) {
                output.waveform.insert(output.waveform.end(),
                    hit.waveform->waveform.begin(), hit.waveform->waveform.end());
            }
        }
    }

    T->Fill();

}

/**
 * @brief Finish processing and cleanup
 * 
 * Called once at the end of processing. Close the output file and perform
 * any necessary cleanup operations.
 */
void JEventProcessor_HMSRawHit::Finish() {
    LOG << "JEventProcessor_HMSRawHit::Finish" << LOG_END;

    // Write ROOT objects and close ROOT file
    if (m_root_output_file) {
	T->Write();
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }

}
