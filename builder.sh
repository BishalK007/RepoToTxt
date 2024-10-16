#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Default configuration
BUILD_DIR="build"
PROJECT_NAME="MyProject"
EXECUTABLE_NAME="MyExecutable"
PROJECT_VERSION="1.0"
USE_VCPKG=OFF

# ----------- GLOBAL CONFIGURATION ------------- #
# Set these values as desired for your global configuration
GLOBAL_PROJECT_NAME="RepoToTxt"
GLOBAL_EXECUTABLE_NAME="repototxt"
GLOBAL_PROJECT_VERSION="1.0"
IS_SET_CONFIG_GLOBAL=false  # Default to false; can be set via --conf-glob

# Function to display usage
usage() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  --conf, -c         Configure the CMake project"
    echo "  --build, -b        Build the project"
    echo "  --run, -r          Run the executable after building"
    echo "  --zip, -z          Zip project files, excluding certain directories"
    echo "  --exe, -e NAME     Specify the name of the executable"
    echo "  --proj, -p NAME    Specify the project name"
    echo "  --ver, -v VER      Specify the project version"
    echo "  --use-nix          Build without using Vcpkg (for Nix environment)"
    echo "  --conf-glob        Use global configuration settings"
    echo "  --help, -h         Display this help message"
    exit 1
}

# Function to configure the CMake project
configure_project() {
    echo "----------------------------------------"
    echo "Configuring CMake project..."
    echo "----------------------------------------"

    # Create build directory if it doesn't exist
    mkdir -p "$BUILD_DIR"

    # Run CMake configuration
    cmake -B "$BUILD_DIR" -S . \
        -DUSE_VCPKG="$USE_VCPKG" \
        -DEXECUTABLE_NAME="$EXECUTABLE_NAME" \
        -DPROJECT_NAME="$PROJECT_NAME" \
        -DPROJECT_VERSION="$PROJECT_VERSION"

    echo "Configuration complete."
}

# Function to build the CMake project
build_project() {
    echo "----------------------------------------"
    echo "Building CMake project..."
    echo "----------------------------------------"

    cmake --build "$BUILD_DIR"

    echo "Build complete."
}

# Function to run the executable
run_executable() {
    echo "----------------------------------------"
    echo "Running the executable..."
    echo "----------------------------------------"

    EXECUTABLE_PATH="$BUILD_DIR/$EXECUTABLE_NAME"

    if [ -f "$EXECUTABLE_PATH" ]; then
        ./"$EXECUTABLE_PATH"
    else
        echo "Executable $EXECUTABLE_NAME not found in $BUILD_DIR. Please build the project first."
        exit 1
    fi
}

# Function to zip files
zip_files() {
    echo "----------------------------------------"
    echo "Zipping files..."
    echo "----------------------------------------"

    # Create the zip file excluding the specified directories and files
    zip -r files.zip . -x "files.zip" ".*" "$BUILD_DIR/*" "build-nix/*" "vcpkg/*"

    echo "Zipping complete. Created files.zip."
}

# Check if no arguments were provided
if [ "$#" -eq 0 ]; then
    usage
fi

# Parse command line arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --conf|-c)
            CONFIGURE_PROJECT=1
            shift
            ;;
        --build|-b)
            BUILD_PROJECT=1
            shift
            ;;
        --run|-r)
            RUN_EXECUTABLE=1
            shift
            ;;
        --zip|-z)
            ZIP_FILES=1
            shift
            ;;
        --exe|-e)
            if [[ -n "$2" && ! "$2" =~ ^- ]]; then
                if [ "$IS_SET_CONFIG_GLOBAL" = false ]; then
                    EXECUTABLE_NAME="$2"
                fi
                shift 2
            else
                echo "Error: --exe requires a valid executable name."
                exit 1
            fi
            ;;
        --proj|-p)
            if [[ -n "$2" && ! "$2" =~ ^- ]]; then
                if [ "$IS_SET_CONFIG_GLOBAL" = false ]; then
                    PROJECT_NAME="$2"
                fi
                shift 2
            else
                echo "Error: --proj requires a valid project name."
                exit 1
            fi
            ;;
        --ver|-v)
            if [[ -n "$2" && ! "$2" =~ ^- ]]; then
                if [ "$IS_SET_CONFIG_GLOBAL" = false ]; then
                    PROJECT_VERSION="$2"
                fi
                shift 2
            else
                echo "Error: --ver requires a valid project version."
                exit 1
            fi
            ;;
        --use-nix)
            USE_VCPKG=OFF
            BUILD_DIR="build-nix"
            shift
            ;;
        --conf-glob)
            IS_SET_CONFIG_GLOBAL=true
            shift
            ;;
        --help|-h)
            usage
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done

# Apply global configuration if IS_SET_CONFIG_GLOBAL is true
if [ "$IS_SET_CONFIG_GLOBAL" = true ]; then
    PROJECT_NAME="$GLOBAL_PROJECT_NAME"
    EXECUTABLE_NAME="$GLOBAL_EXECUTABLE_NAME"
    PROJECT_VERSION="$GLOBAL_PROJECT_VERSION"
fi

# Execute the requested actions
if [ "$CONFIGURE_PROJECT" == "1" ]; then
    configure_project
fi

if [ "$BUILD_PROJECT" == "1" ]; then
    # Ensure the project is configured before building
    if [ ! -d "$BUILD_DIR" ]; then
        configure_project
    fi
    build_project
fi

if [ "$RUN_EXECUTABLE" == "1" ]; then
    run_executable
fi

if [ "$ZIP_FILES" == "1" ]; then
    zip_files
fi
