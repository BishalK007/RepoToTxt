self: super: let
  repototxt-src = super.fetchFromGitHub {
    owner = "BishalK007";
    repo = "RepoToTxt";
    rev = "8f7978e2baa76ab8e4e26242a529a251dfdc8633"; # Use a specific tag or commit hash
    sha256 = "sha256-teTBV/T7c567urdIJC13VrpkPp8m4pj/mhaeYYUPJdk="; # Placeholder, will be updated
  };
in {
  repototxt = super.callPackage "${repototxt-src}/.nix/default.nix" {
    pname_arg = "RepoToTxt";
    exename_arg = "repototxt";
    version_arg = "1.1.1";
  };
}
