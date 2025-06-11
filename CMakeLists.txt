cmake_minimum_required(VERSION 3.16)
project(DVSEventReader VERSION 1.0 LANGUAGES CXX)

# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders in Visual Studio for organization
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Important for Boost
add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10

# Find packages from vcpkg
find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS 
    nowide 
    stacktrace_windbg 
    callable_traits 
    endian 
    property_tree
)

# Add compression/decompression libraries
find_package(lz4 REQUIRED)
find_package(zstd REQUIRED)

# Include paths for manually built libcaer
set(LIBCAER_PATH "C:/Users/grein/Documents/libraries/_installed/libcaer")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBCAER_INCLUDE_DIR "${LIBCAER_PATH}/debug/include")
    set(LIBCAER_LIB_DIR "${LIBCAER_PATH}/debug/lib")
else()
    set(LIBCAER_INCLUDE_DIR "${LIBCAER_PATH}/release/include")
    set(LIBCAER_LIB_DIR "${LIBCAER_PATH}/release/lib")
endif()

link_directories(${LIBCAER_LIB_DIR})

# Add your executable
add_executable(DVSEventReader 	src/main.cpp
				"src/EventReader.cpp"
				src/SerialInterface.cpp)

# Supress macro definition for max from the Windows headers
target_compile_definitions(DVSEventReader PRIVATE NOMINMAX)

#Supress iterator warnings
target_compile_definitions(DVSEventReader PRIVATE _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)


# Add include paths for just this target
target_include_directories(DVSEventReader PRIVATE
    ${LIBCAER_INCLUDE_DIR}
    "C:/Users/grein/Documents/libraries/dv-processing/include"
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(DVSEventReader
    PRIVATE
        caer
        ${OpenCV_LIBS}
        fmt::fmt
        Eigen3::Eigen
        Boost::nowide
        Boost::stacktrace_windbg
        Boost::callable_traits
        Boost::endian
        Boost::property_tree
        lz4::lz4
        zstd::libzstd_shared
)


