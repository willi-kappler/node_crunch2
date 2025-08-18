{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [
      cmake
      xmake
      gcc15

      #llvmPackages_21.libcxx
      #llvmPackages_21.clangUseLLVM
      #llvmPackages_21.clang-tools

      #llvmPackages.libcxxClang
      #llvmPackages.libcxxStdenv
      #clang-tools
      #libcxx
      #clang
    ];
}
