{
  description = "Strange - C++ streaming range library";
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixos-23.05;
    flake-utils.url = github:numtide/flake-utils;
  };
  outputs = {self, nixpkgs, flake-utils} :
  flake-utils.lib.eachDefaultSystem(system: let
    pkgs = import nixpkgs { inherit system; };
  in
    {
      devShell = pkgs.mkShell {
        packages = [ pkgs.cmake pkgs.catch2_3 pkgs.fmt_9 ];
      };
      packages.default = pkgs.stdenv.mkDerivation {
        name = "strange";
        src = ./.;
        nativeBuildInputs = [ pkgs.cmake ];
        buildInputs = [ pkgs.catch2_3 pkgs.fmt_9 ];
      };
    });
}
