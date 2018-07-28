if(NOT DEFINED bf_FIND_COMPONENTS)
    set(bf_FIND_COMPONENTS "c")
endif()

foreach(COMPONENT ${bf_FIND_COMPONENTS})
    if(NOT "${COMPONENT}" IN_LIST "c")
        set(bf_FOUND FALSE)
        set(bf_NOT_FOUND_MESSAGE "Unsupported component: ${COMPONENT}")
    endif()
endif()

if("c" IN_LIST bf_FIND_COMPONENTS)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/bf-c.cmake")
        include("${CMAKE_CURRENT_LIST_DIR}/bf-c.cmake")
    else()
        set(bf_FOUND FALSE)
        set(bf_NOT_FOUND_MESSAGE "Unavailable component: c")
    endif()
endif()