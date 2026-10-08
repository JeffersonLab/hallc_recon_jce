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

if(BUILD_TESTING)
    add_executable(hms_rawhit_tests
        tests/HMSRawHitTests.cc
        JEventProcessor_HMSRawHit.cc
    )
    target_include_directories(hms_rawhit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(hms_rawhit_tests PRIVATE
        ${JANA_LIBRARY} ROOT::Core ROOT::RIO ROOT::Tree hallc_detector_data_types
    )
    add_test(NAME hms_rawhit_tests COMMAND hms_rawhit_tests)
endif()
