FIND_PATH(GMOCK_INCLUDE_DIR gmock/gmock.h
	PATH
	    $ENV{GMOCK_ROOT}
	    ${GMOCK_ROOT}
    HINTS
        $ENV{GMOCK_ROOT}/include
        ${GMOCK_ROOT}/include
)
MARK_AS_ADVANCED(GMOCK_INCLUDE_DIR)

FIND_LIBRARY(GMOCK_LIBRARY_DEBUG
    NAMES gmock gmock.lib
    PATH
        $ENV{GMOCK_ROOT}
        ${GMOCK_ROOT}
    HINTS
	    $ENV{GMOCK_ROOT}/lib
	    ${GMOCK_ROOT}/lib
        $ENV{GMOCK_ROOT}/Debug
        ${GMOCK_ROOT}/Debug
)

FIND_LIBRARY(GMOCK_LIBRARY_RELEASE
    NAMES gmock gmock.lib
    PATH
        $ENV{GMOCK_ROOT}
        ${GMOCK_ROOT}
    HINTS
		$ENV{GMOCK_ROOT}/lib
	    ${GMOCK_ROOT}/lib
        $ENV{GMOCK_ROOT}/Release
        ${GMOCK_ROOT}/Release
)
        
IF(GMOCK_INCLUDE_DIR AND GMOCK_LIBRARY_DEBUG AND GMOCK_LIBRARY_RELEASE)
    SET(GMock_FOUND TRUE)
ENDIF()

IF(GMock_FOUND)
    IF (NOT GMock_FIND_QUIETLY)
        MESSAGE(STATUS "Found GMock")
    ENDIF()
ELSE()
    IF (GMock_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find GMock")
    ENDIF()
ENDIF()
