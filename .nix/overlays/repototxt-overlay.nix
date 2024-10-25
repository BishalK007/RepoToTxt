self: super: let
  repototxt-src = super.fetchFromGitHub {
    owner = "BishalK007";
    repo = "RepoToTxt";
    rev = "69ee28a8d183f4e6bc67a4f71bb1f4c0ee966198"; # Use a specific tag or commit hash
    sha256 = "sha256-dQ0qXxFScMx/9qpqYBid14G2+H9IvdBDtBFsW0gjnL8="; # Placeholder, will be updated
  };
in {
  repototxt = super.callPackage "${repototxt-src}/.nix/default.nix" {
    pname_arg = "repo-to-txt";
    exename_arg = "repototxt";
    version_arg = "1.1.1";
    description = "A C++ CLI tool for developers that allows you to copy and display repository content.";
    maintainer_name = "Bishal Karmakar";
    maintainer_email = "bishal@bishal.pro";
    maintainer_github = "BishalK007";
    homepage = "https://github.com/BishalK007/RepoToTxt";
  };
}
