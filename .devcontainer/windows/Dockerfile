# Use the official Microsoft Windows Server Core as the base image
FROM mcr.microsoft.com/windows:10.0.17763.6414

# Set environment variables to avoid prompts during installation
ENV ChocolateyUseWindowsCompression=false

# Install Chocolatey package manager
RUN powershell -NoProfile -ExecutionPolicy Bypass -Command `[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; `iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install essential tools using Chocolatey
RUN choco install -y git cmake ninja wget unzip curl openssl

# Clean up Chocolatey cache to reduce image size
RUN choco clean --all

# Set up user
ARG USERNAME=vscode
ARG USER_PASSWORD=Password123!
RUN net user %USERNAME% %USER_PASSWORD% /add && `net localgroup Administrators %USERNAME% /add

# Switch to the non-root user
USER %USERNAME%
WORKDIR C:/Users/%USERNAME%

# Set environment variables
ENV PATH="C:/Program Files/Git/bin;C:/Program Files/CMake/bin;C:/Program Files/Ninja;%PATH%"

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git C:/vcpkg && `C:/vcpkg/bootstrap-vcpkg.bat

# Add vcpkg to PATH
ENV PATH="C:/vcpkg:${PATH}"

# This Dockerfile sets up a Windows development environment with essential build tools and vcpkg installed.
