{ pkgs ? import <nixpkgs> {} }:
  #pkgs.mkShell.override { stdenv = pkgs.llvmPackages.libcxxStdenv; } {
  pkgs.llvmPackages.libcxxStdenv.mkDerivation {
    name = "cpp-shell";
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [
      cmake
      xmake
      #libcxx
      #libllvm
      #clang-tools
      #clang
    ];
}
