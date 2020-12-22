{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell rec {
  name = "modbus-env";

  buildInputs = with pkgs; [
    gcc
    cmake
    cmakeCurses
    doxygen
    libnet
  ];
}
