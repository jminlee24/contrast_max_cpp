{ pkgs ? import <nixpkgs> {} }:

let
  hasPackagesDir = builtins.pathExists ./packages;
in
pkgs.mkShell {
  # Tooling you’ll want in the dev shell
  nativeBuildInputs = with pkgs; [
    cmake
    ninja
    pkg-config
    gdb
    clang-tools
    git
  ];

  # Libraries available to your build (CMake can find these)
  buildInputs = with pkgs; [
    gcc
    eigen          # Eigen 3.4
    nlohmann_json  # nlohmann/json with CMake config files
  ];

  # Environment setup
  shellHook = ''
    if [ -z "''${CMAKE_BUILD_TYPE:-}" ]; then
      export CMAKE_BUILD_TYPE=Release
    fi

    export CXXFLAGS="-O3 -DNDEBUG ''${CXXFLAGS:-}"

    export CMAKE_PREFIX_PATH="${pkgs.eigen}:${pkgs.nlohmann_json}:''${CMAKE_PREFIX_PATH:-}"

    mkdir -p build

    if [ ! -f ./build/CMakeCache.txt ]; then
      cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
    fi
    
    if [ ! -f ./build/main ]; then
      echo "run \"cmake --build ./build/\""
    fi
  '';
}