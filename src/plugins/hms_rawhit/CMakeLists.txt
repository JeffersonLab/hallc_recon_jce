add_jana_plugin(hms_rawhit
    SOURCES
        InitPlugin.cc
        JEventProcessor_HMSRawHit.cc
)

target_link_libraries(hms_rawhit
    PRIVATE
        ${JANA_LIBRARY}
        ROOT::Core
        ROOT::RIO
        ROOT::Tree
        ROOT::Hist
        hallc_detector_data_types
)
