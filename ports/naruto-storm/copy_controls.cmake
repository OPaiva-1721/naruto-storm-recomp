if(NOT EXISTS "${DEST_FILE}")
    configure_file("${SOURCE_FILE}" "${DEST_FILE}" COPYONLY)
endif()
