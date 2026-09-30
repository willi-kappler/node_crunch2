{ pkgs ? import <nixpkgs> {} }:

# pkgs.mkShell.override { stdenv = pkgs.llvmPackages_23.stdenv; } {
# pkgs.mkShell.override { stdenv = pkgs.llvmPackages.stdenv; } {
# pkgs.mkShell.override { stdenv = pkgs.gcc16Stdenv; } {
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    gnumake
    meson
    ninja
    pkg-config
    vcpkg
  ];

  buildInputs = with pkgs; [
    argparse
    asio
    fmt
    lz4
    nlohmann_json
    openssl
    spdlog
  ];

  shellHook = ''
    echo "Development environment loaded for c++!"
  '';
}

