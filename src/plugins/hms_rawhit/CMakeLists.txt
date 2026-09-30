add_jana_plugin(hms_rawhit
    SOURCES
        InitPlugin.cc
        JEventProcessor_hmsrawhit.cc
)

target_link_libraries(hms_rawhit
    PRIVATE
        ${JANA_LIBRARY}
        ROOT::Core
        ROOT::RIO
        ROOT::Tree
        ROOT::Hist
        jana2_common_extensions::evio_parser_data_types  # data-object headers (FADC, TIScaler, MPD, VFTDC, ...)

)
