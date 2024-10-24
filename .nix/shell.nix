# /workspaces/RepoToTxt/shell.nix

{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.cmake
    pkgs.gcc
    pkgs.git
    pkgs.ninja
    pkgs.pkg-config
    pkgs.openssl
    pkgs.sudo
    pkgs.coreutils
    pkgs.wget
    pkgs.unzip
    pkgs.curl
    pkgs.jq
    pkgs.wl-clipboard
    pkgs.xclip
    pkgs.ftxui
  ];

  shellHook = ''
    # Set environment variables
    export WAYLAND_DISPLAY=wayland-0
    export XDG_RUNTIME_DIR=/run/user/$(id -u)
    export LANG=en_US.UTF-8
    export LANGUAGE=en_US:en
    export LC_ALL=en_US.UTF-8

    # Remove vcpkg-related environment variables if any
    unset VCPKG_ROOT
    unset VCPKG_DEFAULT_TRIPLET
  '';
}
