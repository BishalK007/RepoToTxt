cmake_minimum_required(VERSION 3.14)

# ----------------------------
# Set Default Variables (Passed from builder.sh)
# ----------------------------

# PROJECT_NAME and EXECUTABLE_NAME are set via command-line arguments
set(PROJECT_NAME "" CACHE STRING "Name of the project")
set(EXECUTABLE_NAME "" CACHE STRING "Name of the executable")

# PROJECT_VERSION is set via command-line arguments or read from VERSION file
if(NOT DEFINED PROJECT_VERSION)
    file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION)
endif()

# MAINTAINER_NAME and MAINTAINER_MAIL are set via command-line arguments
set(MAINTAINER_NAME "" CACHE STRING "Name of maintainer")
set(MAINTAINER_MAIL "" CACHE STRING "E-Mail of maintainer")

# HOMEPAGE is set via command-line arguments
set(HOMEPAGE "" CACHE STRING "Project homepage")

# DESCRIPTION is set via command-line arguments
set(DESCRIPTION "" CACHE STRING "Project description")

# ----------------------------
# Optional: Print Configuration (Useful for Debugging)
# ----------------------------

# Display the configuration in the CMake output
message(STATUS "Initial Project Configuration:")
message(STATUS "  Project Name: ${PROJECT_NAME}")
message(STATUS "  Executable Name: ${EXECUTABLE_NAME}")
message(STATUS "  Project Version: ${PROJECT_VERSION}")
message(STATUS "  Maintainer Name: ${MAINTAINER_NAME}")
message(STATUS "  Maintainer Mail: ${MAINTAINER_MAIL}")
message(STATUS "  Homepage: ${HOMEPAGE}")
message(STATUS "  Description: ${DESCRIPTION}")

# ----------------------------
# Option to Use Vcpkg
# ----------------------------

option(USE_VCPKG "Use Vcpkg for dependency management" ON)

# ----------------------------
# Set Toolchain File and Disable Vcpkg Integration if Not Using Vcpkg
# ----------------------------

if(USE_VCPKG)
    # Specify the path to the vcpkg toolchain file
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Path to the vcpkg toolchain file" FORCE)
    message(STATUS "Using Vcpkg for dependency management.")
else()
    # Disable Vcpkg integration
    set(CMAKE_TOOLCHAIN_FILE "" CACHE STRING "Disable Vcpkg toolchain file" FORCE)
    set(CMAKE_DISABLE_VCPKG_INTEGRATION ON CACHE BOOL "Disable Vcpkg integration")
    message(STATUS "Not using Vcpkg. Dependencies should be provided by the system or Nix.")
endif()

# ----------------------------
# Project Configuration
# ----------------------------

# Set the project name and version
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----------------------------
# Source Files
# ----------------------------

# Automatically find all .cpp files in the src directory recursively
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# ----------------------------
# Package Management
# ----------------------------
if(USE_VCPKG)
    find_package(ftxui CONFIG REQUIRED)
else()
    find_package(ftxui REQUIRED)
endif()

# ----------------------------
# Include Directories
# ----------------------------

# Include directories for the project
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the build directory's include directory for generated headers
include_directories(${CMAKE_BINARY_DIR}/include)

# ----------------------------
# Generate version.hpp
# ----------------------------

# Generate version.hpp from version.hpp.in
configure_file(
    "${CMAKE_SOURCE_DIR}/include/utils/version.hpp.in"
    "${CMAKE_BINARY_DIR}/include/utils/version.hpp"
    @ONLY
)

# ----------------------------
# Add Executable
# ----------------------------

# Add the executable with the specified source files
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# ----------------------------
# Link Libraries
# ----------------------------
if(USE_VCPKG)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ftxui::screen ftxui::dom ftxui::component)
else()
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ftxui::screen ftxui::dom ftxui::component)
endif()
# Link ftxui to the executable

# ----------------------------
# Final Configuration Summary
# ----------------------------

# Display the final configuration in the CMake output
message(STATUS "Final Project Configuration:")
message(STATUS "  Project Name: ${PROJECT_NAME}")
message(STATUS "  Executable Name: ${EXECUTABLE_NAME}")
message(STATUS "  Project Version: ${PROJECT_VERSION}")
message(STATUS "  Source Files: ${SOURCE_FILES}")
message(STATUS "  Use Vcpkg: ${USE_VCPKG}")

# ----------------------------
# CPack Configuration
# ----------------------------

if(WIN32)
    # CPack configuration for Windows (NSIS)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${DESCRIPTION}")
    set(CPACK_PACKAGE_VENDOR "${MAINTAINER_NAME}")
    set(CPACK_PACKAGE_CONTACT "${MAINTAINER_MAIL}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
    set (CPACK_RESOURCE_FILE_LICENSE  "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

    # Enable Unicode in NSIS Installer
    set(CPACK_NSIS_ENABLE_UNICODE ON)

    # Include the EnVar plugin
    # Tell NSIS to include plugins from the $PLUGINSDIR and set Unicode mode
    set(CPACK_NSIS_EXTRA_PREDEFINES "
        Unicode true
        !addplugindir '\$PLUGINSDIR'
    ")

    # Find makensis.exe
    find_program(NSIS_MAKENSIS_EXECUTABLE makensis)

    if(NOT NSIS_MAKENSIS_EXECUTABLE)
        message(FATAL_ERROR "makensis not found. Please install NSIS and ensure makensis is in your PATH.")
    endif()

    # Determine NSIS install directory
    get_filename_component(NSIS_INSTALL_DIR "${NSIS_MAKENSIS_EXECUTABLE}" DIRECTORY)


    # Path to EnVar.dll in NSIS plugins directory
    set(ENVAR_DLL_SOURCE_UNICODE "${NSIS_INSTALL_DIR}/Plugins/x86-unicode/EnVar.dll")

    message(STATUS "DLL SOURCE::  ${ENVAR_DLL_SOURCE_UNICODE}")

    if(EXISTS "${ENVAR_DLL_SOURCE_UNICODE}")
        set(ENVAR_DLL_SOURCE "${ENVAR_DLL_SOURCE_UNICODE}")
        message(STATUS "Using Unicode EnVar.dll at ${ENVAR_DLL_SOURCE}")
    else()
        message(FATAL_ERROR "Unicode EnVar.dll not found in NSIS plugins directory. Ensure the EnVar plugin is installed in x86-unicode.")
    endif()

    # Copy EnVar.dll to build directory
    configure_file(
        "${ENVAR_DLL_SOURCE}"
        "${CMAKE_CURRENT_BINARY_DIR}/EnVar.dll"
        COPYONLY
    )

    # Include EnVar.dll in the installer
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/EnVar.dll" DESTINATION "$PLUGINSDIR")

    set(PATH_INSTALL_COMMANDS "
        EnVar::SetHKCU
        EnVar::Check \\\"Path\\\" \\\"\\\$INSTDIR\\\\bin\\\"
        Pop \\\$0
        \\\${If} \\\$0 = 0
            DetailPrint \\\"Path Already there\\\"
        \\\${Else}
            EnVar::AddValue \\\"Path\\\" \\\"$InstDir\\\\bin\\\"
            Pop \\\$0 ; 0 on success
        \\\${EndIf}
    ")
    set(PATH_UNINSTALL_COMMANDS "
        EnVar::SetHKCU
        EnVar::DeleteValue \\\"Path\\\" \\\"$INSTDIR\\\\bin\\\"
        Pop \\\$0
    ")



    # Set the extra install and uninstall commands
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${PATH_INSTALL_COMMANDS}")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${PATH_UNINSTALL_COMMANDS}")

    # Installation directories
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
    install(FILES VERSION Readme.md DESTINATION doc)
elseif(UNIX)
    # CPack configuration for Unix-like systems (DEB)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_CONTACT "${MAINTAINER_MAIL}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${MAINTAINER_NAME}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.7), libstdc++6 (>= 5)")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${HOMEPAGE}")

    # Installation directories
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
    install(FILES VERSION Readme.md DESTINATION "share/doc/${PROJECT_NAME}")
endif()

# Include CPack module
include(CPack)
