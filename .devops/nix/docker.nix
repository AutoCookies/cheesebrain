{
  lib,
  dockerTools,
  buildEnv,
  cheese-cpp,
  interactive ? true,
  coreutils,
}:

# A tar that can be fed into `docker load`:
#
# $ nix build .#cheesePackages.docker
# $ docker load < result

# For details and variations cf.
# - https://nixos.org/manual/nixpkgs/unstable/#ssec-pkgs-dockerTools-buildLayeredImage
# - https://discourse.nixos.org/t/a-faster-dockertools-buildimage-prototype/16922
# - https://nixery.dev/

# Approximate (compressed) sizes, at the time of writing, are:
#
# .#cheesePackages.docker: 125M;
# .#cheesePackagesCuda.docker: 537M;
# .#legacyPackages.aarch64-linux.cheesePackagesXavier.docker: 415M.

dockerTools.buildLayeredImage {
  name = cheese-cpp.pname;
  tag = "latest";

  contents =
    [ cheese-cpp ]
    ++ lib.optionals interactive [
      coreutils
      dockerTools.binSh
      dockerTools.caCertificates
    ];
}
