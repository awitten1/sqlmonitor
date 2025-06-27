

#!/bin/bash

set -eux

pushd "$(realpath $(dirname $0))"

install_apt() {
    echo "running with sudo for apt"
    sudo apt update && sudo apt install -y build-essential cmake wget unzip libfmt-dev \
        libgflags-dev
}

install_apt

popd