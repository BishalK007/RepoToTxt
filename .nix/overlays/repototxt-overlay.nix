self: super: let
  repototxt-src = super.fetchFromGitHub {
    owner = "BishalK007";
    repo = "RepoToTxt";
    rev = "f087fa753a43b621dfc2f3c85d50473c67305a53"; # Use a specific tag or commit hash
    sha256 = "sha256-Rii6GBVSE+Oxugx97SrG4YE3BcD6qg7wLNUEXug/Js4="; # Placeholder, will be updated
  };
in {
  repototxt = super.callPackage "${repototxt-src}/.nix/default.nix" {
    pname_arg = "RepoToTxt";
    exename_arg = "repototxt";
    version_arg = "1.0";
  };
}
