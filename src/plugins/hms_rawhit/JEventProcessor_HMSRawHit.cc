#include "JEventProcessor_HMSRawHit.h"
#include <JANA/JLogger.h>
#include <map>
#include <tuple>

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
    
    // Clear previous event data
    HMSHodADCPos1x={};
    HMSHodADCPos1y={};
    HMSHodADCPos2x={};
    HMSHodADCPos2y={};

    HMSHodADCNeg1x={};
    HMSHodADCNeg1y={};
    HMSHodADCNeg2x={};
    HMSHodADCNeg2y={};

    using Key = std::tuple<int32_t, int32_t, int32_t>; // plane, bar, signal
    std::map<Key, HodADCRawHit> HMSHodGroup;
    for(const auto& pulse_hit : m_fadcPulses()) {
        auto& hit = HMSHodGroup[{pulse_hit->plane, pulse_hit->bar, pulse_hit->signal}];

        hit.counter.push_back( pulse_hit->bar );
        hit.ped.push_back( pulse_hit->pedestal_sum );
        hit.ped_quality.push_back( pulse_hit->pedestal_quality );
        hit.integral_sum.push_back( pulse_hit->integral_sum );
        hit.integral_quality.push_back( pulse_hit->integral_quality );
        hit.integral_nsample.push_back( pulse_hit->nsamples_above_threshold );
        hit.coarse_time.push_back( pulse_hit->coarse_time );
        hit.fine_time.push_back( pulse_hit->fine_time );
        hit.time_quality.push_back( pulse_hit->time_quality );
        hit.pulse_peak.push_back( pulse_hit->pulse_peak );
    }

    for (const auto* waveform : m_fadcWaveforms()) {
        auto& hit = HMSHodGroup[{waveform->plane, waveform->bar, waveform->signal}];
        hit.waveform = waveform->waveform;
    }

    for( auto& [key, hit] : HMSHodGroup) {
         const auto [plane, bar, signal] = key;
         hit.nhits = static_cast<uint32_t>(hit.integral_sum.size());
        
         if( plane==1 && signal==0 ) HMSHodADCPos1x.push_back(std::move(hit));
         if( plane==1 && signal==1 ) HMSHodADCNeg1x.push_back(std::move(hit));
         if( plane==2 && signal==0 ) HMSHodADCPos2x.push_back(std::move(hit));
         if( plane==2 && signal==1 ) HMSHodADCNeg2x.push_back(std::move(hit));

         if( plane==3 && signal==0 ) HMSHodADCPos1y.push_back(std::move(hit));
         if( plane==3 && signal==1 ) HMSHodADCNeg1y.push_back(std::move(hit));
         if( plane==4 && signal==0 ) HMSHodADCPos2y.push_back(std::move(hit));
         if( plane==4 && signal==1 ) HMSHodADCNeg2y.push_back(std::move(hit));
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
	m_tree->Write();
        m_root_output_file->Close();     // Close ROOT file
        delete m_root_output_file;       // Free memory
        m_root_output_file = nullptr;
    }

}
