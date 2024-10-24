self: super: let
  repototxt-src = super.fetchFromGitHub {
    owner = "BishalK007";
    repo = "RepoToTxt";
    rev = "ed4eb4d6b416de720abc561963a8bc8e8300bd37"; # Use a specific tag or commit hash
    sha256 = "sha256-8has/uHIzv5I6HPzHqsum+HizvMthhDZtMdpR83H77c="; # Placeholder, will be updated
  };
in {
  repototxt = super.callPackage "${repototxt-src}" { };
}
