# default.nix
# enter with "nix-shell"

with import <nixpkgs> {};
stdenv.mkDerivation {
    name = "cpp_dev";
    buildInputs = [ pkg-config zlib llvmPackages_21.libcxx llvmPackages_21.libcxxClang xmake cmake ];
}
