cmake_minimum_required(VERSION 3.11)

project(bf
    VERSION 0.1.0
    LANGUAGES C)

add_library(bf bf.h bf.c)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

include(GNUInstallDirs)

target_include_directories(bf
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pdcurses>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

if(EMSCRIPTEN)
    set_target_properties(bf PROPERTIES
        COMPILE_FLAGS "-s USE_SDL=2"
        LINK_FLAGS "-s USE_SDL=2 -lsdl")
endif()

foreach(FILENAME
    addch addchstr addstr attr beep
    bkgd border clear color delch deleteln
    deprec getch getstr getyx inch inchstr
    initscr inopts insch insstr instr kernel
    keyname mouse move outopts overlay pad
    panel printw refresh scanw scr_dump scroll
    slk termattr terminfo touch util window
    debug)
    target_sources(bf PRIVATE pdcurses/pdcurses/${FILENAME}.c)
endforeach()

foreach(FILENAME
    pdcclip pdcdisp pdcgetsc pdckbd pdcscrn
    pdcsetsc pdcutil)
    if(EMSCRIPTEN)
        target_sources(bf PRIVATE pdcurses/sdl2/${FILENAME}.c)
    elseif(WIN32)
        target_sources(bf PRIVATE pdcurses/wincon/${FILENAME}.c)
    endif()
endforeach()

set_target_properties(bf PROPERTIES
    PUBLIC_HEADER bf.h
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    EXPORT_NAME "c")

install(TARGETS bf EXPORT c
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT c NAMESPACE "bf::" FILE "bf-c.cmake"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/bf)

export(EXPORT c NAMESPACE "bf::" FILE "${CMAKE_CURRENT_BINARY_DIR}/bf-c.cmake")

if(NOT "${CMAKE_CURRENT_LIST_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(FILE bf-config.cmake)
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()
endif()

install(FILES bf-config.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bf)

add_executable(main main.c)

target_link_libraries(main
    PRIVATE bf)

if(EMSCRIPTEN)
    set_target_properties(main PROPERTIES
        COMPILE_FLAGS "-s USE_SDL=2"
        LINK_FLAGS "-s USE_SDL=2 --emrun"
        SUFFIX ".html")
endif()
