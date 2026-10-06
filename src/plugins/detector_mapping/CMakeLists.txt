add_subdirectory(HMS/Hodoscope)

add_jana_plugin(hallc_detector_mapping
    SOURCES
        InitPlugin.cc
    PUBLIC_HEADER
        ${HMS_TRANSLATOR_HEADERS}
)

target_link_libraries(hallc_detector_mapping
    PRIVATE
        ${JANA_LIBRARY}
        jana2_common_extensions::detector_mapping_api
        hms_hodoscope_translators_init
)

add_library(hallc_detector_data_types INTERFACE)
target_link_libraries(hallc_detector_data_types INTERFACE
    jana2_common_extensions::evio_common_modules_data_types
    jana2_common_extensions::detector_mapping_api
)
foreach(_dir IN LISTS HMS_TRANSLATOR_INCLUDE_DIRS)
    target_include_directories(hallc_detector_data_types INTERFACE
        $<BUILD_INTERFACE:${_dir}>
    )
endforeach()
target_include_directories(hallc_detector_data_types INTERFACE
    $<INSTALL_INTERFACE:include/${INSTALL_NAMESPACE}/plugins/detector_mapping>
)
install(TARGETS hallc_detector_data_types
    EXPORT hallc_reconTargets
)
