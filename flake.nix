{
  description = "loo dev shell";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShells.${system}.default = pkgs.mkShell {
        # tools that run at build time
        nativeBuildInputs = with pkgs; [
          mold-wrapped # wrapped so -fuse-ld=mold sees nix's -L / rpath flags
          pkg-config
          wayland-scanner
        ];

        # libraries we link / include
        buildInputs = with pkgs; [
          wayland
          libX11
          vulkan-headers
          vulkan-loader
        ];

        # nix's default hardening (_FORTIFY_SOURCE, pie, ...) clashes with -O0 -no-pie -Werror
        hardeningDisable = [ "all" ];

        # symlink headers into the project so 4coder can index them
        shellHook = ''
          mkdir -p .deps
          ln -sfn ${pkgs.wayland.dev}/include      .deps/wayland
          ln -sfn ${pkgs.vulkan-headers}/include/vulkan .deps/vulkan
          ln -sfn ${pkgs.libX11.dev}/include/X11   .deps/X11
        '';
      };
    };
}
