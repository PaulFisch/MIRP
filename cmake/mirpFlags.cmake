set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS Off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS Off)

################################
# C++ COMPILE FLAGS
################################
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Intel")

    # CMake doesn't support Intel CXX standard until cmake 3.6
    if("${CMAKE_VERSION}" VERSION_LESS "3.6")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w3")

    # Globally-disabled warnings
    #   981  : operands are evaluated in unspecified order
    #   383: value copied to temporary, reference to temporary used
    # 11074: Inlining inhibited by limit max-size
    # 11076: To get full report use -qopt-report=4 -qopt-report-phase ipo
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd981 -wd383 -wd11074 -wd11076")

    #  2338  : switch statement does not have a default clause
    #list(APPEND CMAKE_CXX_FLAGS "-ww2338")

    if(MIRP_STATIC)
        message(FATAL_ERROR "MIRP_STATIC not available with the Intel compiler")
    endif()

elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    if(MIRP_STATIC)
        set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    endif()

elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    if(MIRP_STATIC)
        message(FATAL_ERROR "MIRP_STATIC not available with the Clang compiler")
    endif()
endif()
