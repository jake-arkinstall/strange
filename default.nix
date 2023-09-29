{ lib, stdenv, cmake, catch2_3, fmt_9, build_tests ? false }:
let
  # only include the source files, tests and cmakelists.txt files
  # in the src derivation, so that we don't have to worry about
  # readme changes etc triggering rebuilds of downstream projects.
  filtered-src = lib.cleanSourceWith {
    src = ./.;
    filter = path: type:
      lib.hasPrefix (toString ./include) (toString path) ||
      lib.hasPrefix (toString ./test) (toString path) ||
      lib.hasPrefix (toString ./CMakeLists.txt) (toString path);
  };
in
  stdenv.mkDerivation {
    pname = "strange";
    version = "0.1";
    src = filtered-src;
    nativeBuildInputs = [ cmake ];
    cmakeFlags = [
      "-DBUILD_TESTING=${if build_tests then "ON" else "OFF"}"
    ];
    enableParallelBuilding = true;
    buildInputs = [ catch2_3 fmt_9 ];
    doCheck = build_tests;
    checkPhase = ''
      ctest -V
    '';
    meta = with lib; {
      description = "A strange C++ streaming range library utilising continuation passing";
      license = licenses.mit;
      platforms = platforms.all;
      homepage = "https://github.com/jake-arkinstall/strange";
      downloadPage = "https://github.com/jake-arkinstall/strange";
      maintainers = [{
        name = "Jake Arkinstall";
        github = "jake-arkinstall";
        githubId = 65358059;
      }];
    };
  }
