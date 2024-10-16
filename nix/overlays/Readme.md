![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)


## Installation

### For NixOS Users

To install **RepoToTxt** system-wide on NixOS, follow these steps:

#### 1. Add the Overlay

Overlays allow you to extend or modify the Nixpkgs package set. By including the overlay provided in the repository, you can easily integrate `RepoToTxt` into your system.

1. **Locate the Overlay File:**

   The overlay file is located at `nix/overlays/repototxt-overlay.nix` within the repository.

2. **Create the Overlays Directory (if it doesn't exist):**

   ```bash
   sudo mkdir -p /etc/nixos/overlays
   ```

3. **Copy the Overlay File:**

   ```bash
   sudo cp RepoToTxt/nix/overlays/repototxt-overlay.nix /etc/nixos/overlays/
   ```

   Ensure that the path `/etc/nixos/overlays/repototxt-overlay.nix` matches where you want to place the overlay.

#### 3. Modify Your `configuration.nix`

Edit your NixOS configuration to include the new overlay and add `repototxt` to your system packages.

1. **Open `configuration.nix`:**

   ```bash
   sudo vi /etc/nixos/configuration.nix
   ```

2. **Add the Overlay and Package:**

   Locate the `nixpkgs.overlays` and `environment.systemPackages` sections and modify them as follows:

   ```nix
   { config, pkgs, ... }:

   {
     # Import existing overlays if any
     nixpkgs.overlays = [
       # Include the RepoToTxt overlay
       (import /etc/nixos/overlays/repototxt-overlay.nix)
     ];

     # Add RepoToTxt to your system packages
     environment.systemPackages = with pkgs; [
       # ... other packages ...
       repototxt
     ];

     # ... other configurations ...
   }
   ```

   **Notes:**
   - Ensure that the path `/etc/nixos/overlays/repototxt-overlay.nix` matches the location where you copied the overlay file.
   - If you have existing overlays, make sure to include them in the `nixpkgs.overlays` list to avoid overwriting.

#### 4. Rebuild Your NixOS Configuration

Apply the changes by rebuilding your NixOS system:

```bash
sudo nixos-rebuild switch
```

This command will build and install the `repototxt` package system-wide.

#### 5. Verify the Installation

After the rebuild completes, verify that `RepoToTxt` is installed correctly:

```bash
repototxt --version
```

*Replace `--version` with an appropriate flag or command as per your executable's design.*

