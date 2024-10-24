{ pkgs ? import <nixpkgs> {}, pname_arg ? "MyProject", exename_arg ? "MyExecutable", version_arg ? "1.0" }:

pkgs.stdenv.mkDerivation rec {
  pname = pname_arg;
  exename = exename_arg;
  version = version_arg;

  src = pkgs.lib.cleanSource ../.;

  nativeBuildInputs = [
    pkgs.cmake
    pkgs.ninja
    pkgs.git
    pkgs.pkg-config
    pkgs.bash
  ];

  buildInputs = [
    pkgs.ftxui
    pkgs.wl-clipboard
    pkgs.xclip
  ];

  cmakeFlags = [
    "-DUSE_VCPKG=OFF"
    "-DPROJECT_NAME=${pname}"
    "-DEXECUTABLE_NAME=${exename}"
    "-DPROJECT_VERSION=${version}"
  ];

  # Set build directory
  cmakeBuildDir = "build-nix";

  configurePhase = ''
    bash builder.sh -c --use-nix --proj ${pname} --exe ${exename} --ver ${version}
  '';

  buildPhase = ''
    cmake --build ${cmakeBuildDir}
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp ${cmakeBuildDir}/${exename} $out/bin/
  '';

  meta = with pkgs.lib; {
    description = "A C++ CLI tool for developers";
    homepage = "https://github.com/BishalK007/RepoToTxt";
    license = licenses.mit;
    maintainers = [ 
      {
        name = "Bishal Karmakar";
        email = "bishal@bishal.pro";
        github = "bishalk007";
      }
    ];
  };
}