#pragma once

#include <cstdint>
#include <vector>

struct HMSHodoscopeFADCWaveformDigiHit {
    std::int32_t plane;
    std::int32_t bar;
    std::int32_t signal;

    std::uint32_t rocid;
    std::uint32_t slot;
    std::uint32_t channel;

    std::vector<std::uint32_t> waveform;
};
