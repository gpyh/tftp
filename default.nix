with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "tftp";
  buildInputs = [ stdenv ];
  shellHook = ''
    export CPATH=$CPATH:$(pwd)/include:$(pwd)/AdresseInternet/include:$(pwd)/sudp/include
    '';
}
