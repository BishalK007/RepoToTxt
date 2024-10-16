{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "repototxt";
  version = "1.0";

  src = ./.;

  nativeBuildInputs = [
    pkgs.cmake
    pkgs.pkg-config
  ];

  buildInputs = [
    pkgs.ftxui
  ];

  cmakeFlags = [
    "-DUSE_VCPKG=OFF"
    "-DPROJECT_NAME=${pname}"
    "-DEXECUTABLE_NAME=${pname}"
    "-DPROJECT_VERSION=${version}"
  ];

  #Set build directory
  cmakeBuildDir = "build-nix";

  configurePhase = ''
    bash builder.sh -c --conf-glob --use-nix
  '';

  buildPhase = ''
    cd ${cmakeBuildDir}
    pwd
    make
    mkdir -p $out/bin
    cp ./${pname} $out/bin/
  '';

  installPhase = ''
    echo "Nothing to do in Install Phase"
  '';

  meta = {
    description = "A tool to convert repositories to text.";
    homepage = "https://github.com/BishalK007/RepoToTxt";
    license = pkgs.lib.licenses.mit;
    maintainers = with pkgs.lib.maintainers; [ BishalK007 ];
  };
}
