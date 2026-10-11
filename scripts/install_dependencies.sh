#!/bin/bash

# Installs everything needed to build the project (see Dockerfile and CMakeLists.txt files).
# Also installs format tools used by scripts/check_format.sh.
# Supported systems: Arch Linux, Debian 13 (trixie).

set -e

# run package manager as root
SUDO=""
if [ "$(id -u)" != "0" ]; then
    SUDO="sudo"
fi

if [ ! -f /etc/os-release ]; then
    echo "Can't detect system: /etc/os-release not found"
    exit 1
fi
. /etc/os-release

install_arch() {
    if ! command -v yay >/dev/null 2>&1; then
        echo "ERROR: yay not found, install it first: https://github.com/Jguer/yay#installation"
        exit 1
    fi
    if [ "$(id -u)" = "0" ]; then
        echo "ERROR: yay must not be run as root, run this script as regular user"
        exit 1
    fi

    # clang package also contains clang-format, cmake-format is from AUR
    # util-linux-libs provides uuid, boost provides headers and cmake configs for boost-libs
    # yay calls sudo itself when needed
    yay -Syu --needed --noconfirm \
        base-devel cmake git pkgconf \
        clang lld cmake-format \
        openssl asio boost boost-libs util-linux-libs sqlite zlib
}

install_debian() {
    if [ "$VERSION_ID" != "13" ]; then
        echo "WARNING: Debian $VERSION_ID detected, script is made for Debian 13 - package names may differ"
    fi

    $SUDO apt-get update
    $SUDO env DEBIAN_FRONTEND=noninteractive apt-get install -y \
        build-essential cmake git pkg-config \
        clang lld clang-format cmake-format \
        libssl-dev libasio-dev uuid-dev libsqlite3-dev zlib1g-dev \
        libboost-dev libboost-program-options-dev libboost-thread-dev libboost-filesystem-dev
}

case "$ID" in
arch) install_arch ;;
debian) install_debian ;;
*)
    echo "Unsupported system: $PRETTY_NAME (supported: Arch Linux, Debian 13)"
    exit 1
    ;;
esac

# build.sh uses /usr/bin/clang++
if [ ! -x /usr/bin/clang++ ]; then
    echo "ERROR: /usr/bin/clang++ not found after installation"
    exit 1
fi

# vendor libraries are git submodules - fresh clone without --recursive has them empty
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
REPO_DIR=$(dirname "$SCRIPT_DIR")
if [ -d "$REPO_DIR/.git" ]; then
    git -C "$REPO_DIR" submodule update --init --recursive
fi

echo "All dependencies are installed, now you can run ./scripts/build.sh"
