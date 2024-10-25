# Use the official Ubuntu 22.04 LTS as the base image
FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update the package list
RUN apt-get update

# Install essential build tools and dependencies, including locales
RUN apt-get install -y build-essential curl tar unzip zip wget ninja-build pkg-config libssl-dev sudo locales devscripts debhelper dh-make cmake pkg-config

# Install additional tools
RUN apt-get install -y git jq wl-clipboard

# Clean up the apt cache to reduce image size
RUN rm -rf /var/lib/apt/lists/*

# Generate and set the locale
RUN locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8

# Set environment variables for locale
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Download and install CMake 3.30.0 manually
RUN CMAKE_VERSION=3.30.0 && \
    CMAKE_DIR=cmake-${CMAKE_VERSION}-linux-x86_64 && \
    curl -L https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_DIR}.tar.gz -o /tmp/cmake.tar.gz && \
    tar -xzvf /tmp/cmake.tar.gz -C /opt && \
    ln -s /opt/${CMAKE_DIR}/bin/cmake /usr/local/bin/cmake && \
    rm /tmp/cmake.tar.gz

# Set build arguments for UID and GID
ARG USER_ID=1000
ARG GROUP_ID=1000

# Create a non-root user with specified UID and GID
RUN groupadd -g ${GROUP_ID} vscode && \
    useradd -m -u ${USER_ID} -g vscode -s /bin/bash vscode && \
    usermod -aG sudo,users vscode

# Ensure that /etc/sudoers.d exists
RUN mkdir -p /etc/sudoers.d

# Configure passwordless sudo for 'vscode'
RUN echo "vscode ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/vscode && \
    chmod 0440 /etc/sudoers.d/vscode

# Switch to the non-root user
USER vscode
WORKDIR /home/vscode

# Add vcpkg to PATH for convenience
ENV PATH="/vcpkg:${PATH}"

# Set environment variables for Wayland
ENV WAYLAND_DISPLAY=wayland-0
ENV XDG_RUNTIME_DIR=/run/user/1000

# This Dockerfile sets up a development environment with CMake 3.30.0 installed from source.
