
#ifndef _JEventProcessor_HMSRawHit_h_
#define _JEventProcessor_HMSRawHit_h_

#include <TFile.h>
#include <TTree.h>
#include <cstdint>
#include <string>
#include <vector>

#include <JANA/JEventProcessor.h>
#include "HMSHodoscopeFADCPulseDigiHit.h"
#include "HMSHodoscopeFADCWaveformDigiHit.h"

struct HodADCBranches {
    // One element per bar in this plane and signal end.
    std::vector<uint32_t> counter;
    std::vector<uint32_t> ped;
    std::vector<uint32_t> ped_quality;
    std::vector<uint32_t> nhits;

    std::vector<uint32_t> waveform;
    std::vector<uint32_t> integral_sum;
    std::vector<uint32_t> integral_quality;
    std::vector<uint32_t> integral_nsample;
    std::vector<uint32_t> coarse_time;
    std::vector<uint32_t> fine_time;
    std::vector<uint32_t> time_quality;
    std::vector<uint32_t> pulse_peak;

    void clear() {
        counter.clear();
        ped.clear();
        ped_quality.clear();
        nhits.clear();
        waveform.clear();
        integral_sum.clear();
        integral_quality.clear();
        integral_nsample.clear();
        coarse_time.clear();
        fine_time.clear();
        time_quality.clear();
        pulse_peak.clear();
    }
};


/**
 * @class JEventProcessor_HMSRawHit
 * @brief This processor gives HMS detector hits information before calibration
 * 
 */
class JEventProcessor_HMSRawHit : public JEventProcessor {

private:
    // Declare Inputs
    Input<HMSHodoscopeFADCPulseDigiHit> m_fadcPulses {this};
    Input<HMSHodoscopeFADCWaveformDigiHit> m_fadcWaveforms {this};

    /**
     * @brief ROOT output filename parameter
     * 
     * This parameter allows users to specify the ROOT output filename via JANA2 configuration.
     * The parameter constructor takes the following arguments:
     * - owner: Pointer to this component (for parameter registration)
     * - name: "ROOT_OUT_FILENAME" - the parameter name used in configuration files/command line
     * - default_value: "evio_processor.root" - default filename if not specified
     * - description: "Output file name for root data" - help text for the parameter
     * - is_shared: if true, the parameter name is used as-is;  
     *              if false (default), the component's prefix (set in the constructor) is prepended to the name.
     */
    Parameter<std::string> m_root_output_filename {this, "ROOT_OUT_FILENAME", "HMS_rawhits.root", "Output file name for ROOT data", true};

    // ROOT Tree variables 
    HodADCBranches HMSHodADCPos1x, HMSHodADCNeg1x;
    HodADCBranches HMSHodADCPos2x, HMSHodADCNeg2x;
    HodADCBranches HMSHodADCPos1y, HMSHodADCNeg1y;
    HodADCBranches HMSHodADCPos2y, HMSHodADCNeg2y;

    // ROOT output objects
    TFile *m_root_output_file = nullptr;
    TTree *T = nullptr;
    
public:

    JEventProcessor_HMSRawHit();
    virtual ~JEventProcessor_HMSRawHit() = default;

    void Init() override;
    void ProcessSequential(const JEvent& event) override;
    void Finish() override;

};

#endif // _JEventProcessor_HMSRawHit_h_
