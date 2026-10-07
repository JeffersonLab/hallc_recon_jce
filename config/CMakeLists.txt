# These files are read by JCE when JCE_CONFIG_DIR points to this installation.
set(HALLC_CONFIG_DESTINATION config)
if(INSTALL_NAMESPACE)
    string(APPEND HALLC_CONFIG_DESTINATION "/${INSTALL_NAMESPACE}")
endif()

install(FILES
    default_plugins.db
    filter.db
    mapping.db
    DESTINATION ${HALLC_CONFIG_DESTINATION}
)

# TRANSLATION:DIRECTORY must point at this directory, which contains the
# detector catalog manifest.map and its per-detector manifests and maps.
install(
    DIRECTORY evio_parser/detector_mappings/
    DESTINATION ${HALLC_CONFIG_DESTINATION}/evio_parser/detector_mappings
    FILES_MATCHING PATTERN "*.map"
)
