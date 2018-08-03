cmake_minimum_required(VERSION 3.10)

project(bf
    VERSION 0.1.0
    LANGUAGES C)

set(BF_APPS ON CACHE BOOL "Build bf applications")

set(PDC_FONT_PATH "" CACHE FILEPATH "Path to *.ttf font file")
set(PDC_PROVIDER "" CACHE STRING "The PDCurses implementaion provider (src, sys)")
set(PDC_DLL_BUILD ON CACHE BOOL "True if building a Windows DLL")

include(GNUInstallDirs)

add_library(bfh INTERFACE)

target_include_directories(bfh
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
install(FILES bf.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS bfh EXPORT bfh
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT bfh DESTINATION ${CMAKE_INSTALL_DATADIR}/bf)
export(EXPORT bfh)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses QUIET MODULE)

if(Curses_FOUND)
    add_library(bfnc bf.c)
    if(EXISTS "${CURSES_INCLUDE_DIR}/ncurses/curses.h")
        set(CURSES_INCLUDE_DIR "${CURSES_INCLUDE_DIR}/ncurses")
    endif()
    target_include_directories(bfnc PRIVATE ${CURSES_INCLUDE_DIR})
    target_link_libraries(bfnc
        PUBLIC bfh
        PRIVATE ${CURSES_LIBRARIES})
    message("CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
    if(BUILD_SHARED_LIBS)
        set_target_properties(bfnc PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    install(TARGETS bfnc EXPORT bfnc
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(EXPORT bfnc DESTINATION ${CMAKE_INSTALL_DATADIR}/bf)
    export(EXPORT bfnc)
endif()

if(PDC_PROVIDER MATCHES "[Ss][Yy][Ss]"
        OR NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/pdcurses")

    include(SelectLibraryConfigurations)
    find_path(PDC_INCLUDE_DIR NAMES pdcurses/curses.h curses.h pdcurses.h)
    if(EXISTS "${PDC_INCLUDE_DIR}/pdcurses/curses.h")
        set(PDC_INCLUDE_DIR "${PDC_INCLUDE_DIR}/pdcurses")
    endif()
    mark_as_advanced(PDC_INCLUDE_DIR)
    find_library(PDC_LIBRARY_RELEASE NAMES pdcurses pdcurses.dll)
    mark_as_advanced(PDC_LIBRARY_RELEASE)
    find_library(PDC_LIBRARY_DEBUG NAMES pdcursesd pdcursesd.dll pdcurses pdcurses.dll)
    mark_as_advanced(PDC_LIBRARY_DEBUG)
    select_library_configurations(PDC)

    if(PDC_LIBRARY_RELEASE OR PDC_LIBRARY_DEBUG)
        add_library(bfpdc bf.c)
        target_link_libraries(bfpdc
            PRIVATE ${PDC_LIBRARY})
        if(PDC_DLL_BUILD)
            target_compile_definitions(bfpdc PRIVATE PDC_DLL_BUILD)
        endif()
        if(EXISTS "${PDC_INCLUDE_DIR}")
            target_include_directories(bfpdc PRIVATE "${PDC_INCLUDE_DIR}")
        endif()
    endif()

elseif(PDC_PROVIDER MATCHES "[Ss][Rr][Cc]"
        OR EXISTS "${CMAKE_CURRENT_LIST_DIR}/pdcurses")

add_library(bfpdc bf.c)

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

target_include_directories(bfpdc
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/pdcurses>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

foreach(FILENAME
    addch addchstr addstr attr beep
    bkgd border clear color delch deleteln
    deprec getch getstr getyx inch inchstr
    initscr inopts insch insstr instr kernel
    keyname mouse move outopts overlay pad
    panel printw refresh scanw scr_dump scroll
    slk termattr terminfo touch util window
    debug)
    target_sources(bfpdc PRIVATE pdcurses/pdcurses/${FILENAME}.c)
endforeach()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "MSYS")
    set(WIN32 1)
endif()

foreach(FILENAME
    pdcclip pdcdisp pdcgetsc pdckbd pdcscrn
    pdcsetsc pdcutil)
    if(EMSCRIPTEN)
        target_sources(bfpdc PRIVATE pdcurses/sdl2/${FILENAME}.c)
    elseif(WIN32)
        target_sources(bfpdc PRIVATE pdcurses/wincon/${FILENAME}.c)
    endif()
endforeach()

set_target_properties(bfpdc PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

if(EMSCRIPTEN)
    set_target_properties(bfpdc PROPERTIES
        COMPILE_FLAGS "-s USE_SDL=2"
        LINK_FLAGS "-s USE_SDL=2")
    if(EXISTS "${PDC_FONT_PATH}")
        set_property(TARGET bfpdc APPEND_STRING PROPERTY COMPILE_FLAGS " -s USE_SDL_TTF=2")
        set_property(TARGET bfpdc APPEND_STRING PROPERTY LINK_FLAGS " -s USE_SDL_TTF=2")
    endif()
endif()

target_compile_definitions(bfpdc PRIVATE PDC_FORCE_UTF8=1)

if(EXISTS "${PDC_FONT_PATH}" OR WIN32)
    target_compile_definitions(bfpdc PRIVATE PDC_WIDE=1)
endif()

if(EXISTS "${PDC_FONT_PATH}")
    set(PDC_FONT_NAME "font.ttf")
    #get_filename_component(PDC_FONT_NAME "${PDC_FONT_PATH}" NAME)
    configure_file("${PDC_FONT_PATH}" "${PDC_FONT_NAME}" COPYONLY)
    target_compile_definitions(bfpdc
        PRIVATE
            PDC_FONT_PATH="${PDC_FONT_NAME}")
endif()

endif() # EXISTS pdcurses

if(NOT "${CMAKE_CURRENT_LIST_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(FILE bf-config.cmake)
        configure_file(${FILE} ${FILE} COPYONLY)
    endforeach()
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/bf-config-version.cmake"
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES bf-config.cmake "${CMAKE_CURRENT_BINARY_DIR}/bf-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bf)

if(TARGET bfpdc)

target_link_libraries(bfpdc PUBLIC bfh)

install(TARGETS bfpdc EXPORT bfpdc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT bfpdc DESTINATION ${CMAKE_INSTALL_DATADIR}/bf)

export(EXPORT bfpdc)

if(BUILD_SHARED_LIBS)
    set_target_properties(bfpdc PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(BF_APPS)

add_executable(main main.c)

target_link_libraries(main PRIVATE bfpdc)

if(EMSCRIPTEN)
    add_executable(hello examples/hello.c)
    target_link_libraries(hello PRIVATE bfpdc)
    set_target_properties(hello PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/html")
    foreach(EXE main hello)
        set_target_properties(${EXE} PROPERTIES
            COMPILE_FLAGS "-s USE_SDL=2"
            LINK_FLAGS "-s USE_SDL=2 --emrun"
            SUFFIX ".html")
        if(EXISTS "${PDC_FONT_PATH}")
            set_property(TARGET ${EXE} APPEND_STRING PROPERTY COMPILE_FLAGS " -s USE_SDL_TTF=2")
            set_property(TARGET ${EXE} APPEND_STRING PROPERTY
                LINK_FLAGS " -s USE_SDL_TTF=2 --preload-file ${PDC_FONT_NAME}")
        endif()
    endforeach()
endif()

endif() # BF_APPS

endif() # TARGET bfpdc
