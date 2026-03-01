{ inputs, ... }:
{
  perSystem =
    {
      config,
      system,
      lib,
      pkgsCuda,
      ...
    }:
    {
      legacyPackages =
        let
          caps.cheesePackagesXavier = "7.2";
          caps.cheesePackagesOrin = "8.7";
          caps.cheesePackagesTX2 = "6.2";
          caps.cheesePackagesNano = "5.3";

          pkgsFor =
            cap:
            import inputs.nixpkgs {
              inherit system;
              config = {
                cudaSupport = true;
                cudaCapabilities = [ cap ];
                cudaEnableForwardCompat = false;
                inherit (pkgsCuda.config) allowUnfreePredicate;
              };
            };
        in
        builtins.mapAttrs (name: cap: (pkgsFor cap).callPackage ./scope.nix { }) caps;

      packages = lib.optionalAttrs (system == "aarch64-linux") {
        jetson-xavier = config.legacyPackages.cheesePackagesXavier.cheese-cpp;
        jetson-orin = config.legacyPackages.cheesePackagesOrin.cheese-cpp;
        jetson-nano = config.legacyPackages.cheesePackagesNano.cheese-cpp;
      };
    };
}
