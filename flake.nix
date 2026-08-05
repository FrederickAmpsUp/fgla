{
  description = "fgla development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
      };
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          gcc
          clang
          clang-tools
          cmake
          ninja

          glfw
          vulkan-loader
          vulkan-headers
          vulkan-tools
          vulkan-validation-layers
        ];
      };
    };
}
