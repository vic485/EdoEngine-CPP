cmake_minimum_required(VERSION 3.14)
project(EdoCore)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

# TODO: Settable options for preprocessors
add_compile_definitions(_EDO_WINDOWS)
add_compile_definitions(_EXPORT_DLL)

add_library(EdoCore SHARED src/Edo.h src/EdoBase.h src/Types/EdoString.cpp src/Types/EdoString.h src/Utils/EdoTextLog.cpp src/Utils/EdoTextLog.h src/EdoMacros.h src/EdoIncludes.h)
