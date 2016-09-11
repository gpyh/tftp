with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "tftp";
  buildInputs = [ stdenv ];
  shellHook = ''
    export CPATH=$CPATH:$(pwd)/include:$(pwd)/AdrInet/include:$(pwd)/sudp/include
    '';
}
