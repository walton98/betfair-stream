{
  description = "Betfair Stream";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        pkg = pkgs.stdenv.mkDerivation rec {
          name = "betfair-stream";
          src = ./.;
          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = with pkgs; [
            boost
            openssl
            nlohmann_json
          ];
        };
      in {
        devShells.default = pkgs.mkShell {
          name = "devenv";
          inputsFrom = [ pkg ];
        };
        packages.default = pkg;
      }
    );
}
