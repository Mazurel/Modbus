{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  name = "modbus-env";
  buildInputs = with pkgs; [
    gcc
    cmake
    cmakeCurses
    doxygen
  ];

  shellHook = ''
    alias build="rm -rf ./build;mkdir build && cd build && cmake .."
    alias build-docs="doxygen"
  '';
}
