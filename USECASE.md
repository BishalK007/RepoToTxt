# Use Cases for RepoToTxt

## Introduction

**RepoToTxt** is a versatile tool designed to simplify the process of configuring, building, running, and packaging your C++ projects. This document outlines various use cases and provides step-by-step instructions to help you effectively utilize the `builder.sh` script and other project functionalities.

## Table of Contents

- [Use Case 1: Configuring the Project](#use-case-1-configuring-the-project)
- [Use Case 2: Building the Project](#use-case-2-building-the-project)
- [Use Case 3: Running the Executable](#use-case-3-running-the-executable)
- [Use Case 4: Building a Debian Package](#use-case-4-building-a-debian-package)
- [Use Case 5: Zipping Project Files](#use-case-5-zipping-project-files)
- [Use Case 6: Updating Nix Overlays](#use-case-6-updating-nix-overlays)
- [Use Case 7: Entering the Nix Shell](#use-case-7-entering-the-nix-shell)
- [Troubleshooting](#troubleshooting)

## Use Case 1: Configuring the Project

**Objective:** Set up the project configuration with default or custom settings.

### Steps:

1. **Default Configuration:**

   Configure the project using default settings:

   ```bash
   ./builder.sh --conf
   ```

2. **Custom Configuration:**

   Configure the project with a custom project name, executable name, and version:

   ```bash
   ./builder.sh --conf --proj MyCustomProject --exe MyExecutable --ver 2.0
   ```

   **Parameters:**
   - `--proj` or `-p`: Specify the project name.
   - `--exe` or `-e`: Specify the name of the executable.
   - `--ver` or `-v`: Specify the project version.

## Use Case 2: Building the Project

**Objective:** Compile the project using the configured settings.

### Steps:

1. **Build with Default Settings:**

   ```bash
   ./builder.sh --build
   ```

2. **Build with Custom Settings:**

   If you have customized the project name, executable name, or version, ensure to include them:

   ```bash
   ./builder.sh --build --proj MyCustomProject --exe MyExecutable --ver 2.0
   ```

3. **Build Without Vcpkg (Using Nix):**

   To build the project without using vcpkg, typically for NixOS environments:

   ```bash
   ./builder.sh --build --use-nix
   ```

## Use Case 3: Running the Executable

**Objective:** Execute the built application directly from the command line.

### Steps:

1. **Run with Default Settings (Ubuntu with Vcpkg):**

   ```bash
   ./builder.sh --run
   ```

2. **Run with Custom Settings:**

   If you have specified custom project configurations:

   ```bash
   ./builder.sh --run --proj MyCustomProject --exe MyExecutable --ver 2.0
   ```

3. **Run Using Nix (NixOS):**

   ```bash
   ./builder.sh --run --use-nix
   ```

   **Note:** Ensure the project is built using Nix before running.

## Use Case 4: Building a Debian Package

**Objective:** Package the project into a Debian `.deb` file for easy distribution and installation.

### Steps:

1. **Build the `.deb` Package:**

   ```bash
   ./builder.sh --build-deb
   ```

2. **Locate the Package:**

   After successful execution, the `.deb` package will be located in the `build` directory:

   ```bash
   ls build/*.deb
   ```

3. **Install the Package:**

   ```bash
   sudo apt install ./build/RepoToTxt-1.1.1-Linux.deb
   ```

4. **Verify Installation:**

   ```bash
   repototxt --version
   ```

## Use Case 5: Zipping Project Files

**Objective:** Create a zip archive of the project files, excluding specific directories and files.

### Steps:

1. **Create the Zip Archive:**

   ```bash
   ./builder.sh --zip
   ```

   **What It Does:**
   - Packages the project into `files.zip`.
   - Excludes directories like `build`, `build-nix`, `vcpkg`, and files like `README.md`.

2. **Locate the Zip File:**

   The zip file `files.zip` will be available in the project's root directory:

   ```bash
   ls files.zip
   ```

## Use Case 6: Updating Nix Overlays

**Objective:** Update the Nix overlay file with the latest commit hash and SHA256 hash for reproducible builds.

### Steps:

1. **Update the Overlay:**

   ```bash
   ./builder.sh --update-overlay
   ```

   **What It Does:**
   - Fetches the latest commit hash from the GitHub repository.
   - Computes the corresponding SHA256 hash.
   - Updates the `.nix/overlays/repototxt-overlay.nix` file with the new values.

2. **Verify the Update:**

   Check the overlay file to ensure the latest commit and SHA256 are correctly updated.

   ```bash
   cat .nix/overlays/repototxt-overlay.nix
   ```

## Use Case 7: Entering the Nix Shell

**Objective:** Access the Nix shell environment for NixOS users to perform various development tasks.

### Steps:

1. **Enter the Nix Shell:**

   ```bash
   ./builder.sh --nix-shell
   ```

   **Alternative Command:**

   ```bash
   nix-shell .nix/shell.nix
   ```

2. **Perform Development Tasks:**

   Within the Nix shell, you can configure, build, run, and package the project using the available scripts and tools.

## Troubleshooting

- **Unmet Dependencies When Installing `.deb` Package:**

  - **Issue:** Installing the Debian package results in unmet dependencies like `libc6` and `libstdc++6`.
  
  - **Solution:**
    - Ensure that the package architecture is correctly set to `amd64` in `CMakeLists.txt`.
    - Update your system packages:
    
      ```bash
      sudo apt update
      sudo apt upgrade
      ```
    - Rebuild the `.deb` package after fixing architecture settings:
    
      ```bash
      ./builder.sh --build-deb
      ```

- **Build Failures:**

  - **Issue:** Errors during the build process.
  
  - **Solution:**
    - Verify that all dependencies are correctly installed and managed via vcpkg or Nix.
    - Check the build logs for specific error messages.
    - Ensure that the `VERSION` file is present and correctly formatted.

- **Executable Not Found After Build:**

  - **Issue:** The executable is not present in the expected build directory.
  
  - **Solution:**
    - Confirm that the build process completed successfully.
    - Check if the correct build directory was used (`build` for vcpkg, `build-nix` for Nix).
    - Ensure that the `builder.sh` script was run with appropriate flags.

- **Docker Dev Container Issues:**

  - **Issue:** Problems accessing or setting up the Docker Dev Container.
  
  - **Solution:**
    - Ensure Docker is installed and running.
    - Verify that you have the necessary permissions to run Docker commands.
    - Check the `.devcontainer/Dockerfile` and related configuration files for errors.

- **Script Permissions:**

  - **Issue:** `builder.sh` script is not executable.
  
  - **Solution:**
    - Make the script executable:
    
      ```bash
      chmod +x builder.sh
      ```

- **Nix Build Errors:**

  - **Issue:** Errors specific to Nix builds.
  
  - **Solution:**
    - Ensure that the Nix expressions in `.nix/default.nix` are correctly specified.
    - Update Nix and related packages to the latest versions.
    - Refer to Nix documentation for resolving complex build issues.

## Additional Resources

- **CMake Documentation:** [https://cmake.org/documentation/](https://cmake.org/documentation/)
- **vcpkg Documentation:** [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)
- **Nix Documentation:** [https://nixos.org/manual/nix/](https://nixos.org/manual/nix/)
- **Docker Documentation:** [https://docs.docker.com/](https://docs.docker.com/)
- **ftxui Library:** [https://github.com/ArthurSonzogni/ftxui](https://github.com/ArthurSonzogni/ftxui)

For further assistance, please refer to the project's [GitHub Issues](https://github.com/BishalK007/RepoToTxt/issues) page or contact the maintainer at [bishal@bishal.pro](mailto:bishal@bishal.pro).

