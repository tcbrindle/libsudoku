
find_package(doxygen)

if(NOT DOXYGEN_FOUND)
    return()
endif()

configure_file(libsudoku.doxyfile.in libsudoku.doxyfile @ONLY)

add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} libsudoku.doxyfile
        comment "Generating Doxygen documentation" verbatim)
