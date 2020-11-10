{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  name = "modbus-env";
  buildInputs = with pkgs; [
    gcc
    cmake
    cmakeCurses
  ];
  shellHook = ''
    rm -rf ./build 
    mkdir build && cd build && cmake ..
  '';
}
