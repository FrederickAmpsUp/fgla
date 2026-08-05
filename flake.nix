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

  shellHook = ''
    cat > .clangd <<EOF
    CompileFlags:
      CompilationDatabase: build
      Add:
        - "-isystem"
        - "${pkgs.gcc}/include/c++/${pkgs.gcc.version}"
        - "-isystem"
        - "${pkgs.gcc}/include/c++/${pkgs.gcc.version}/x86_64-unknown-linux-gnu"
    EOF
  '';
}
