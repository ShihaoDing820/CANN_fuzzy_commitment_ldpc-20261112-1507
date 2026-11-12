#!/usr/bin/env bash

set -euo pipefail

source /home/dsh/cann-dev/activate.sh

cmake \
    --fresh \
    -S /home/dsh/cann-dev/fuzzy-commitment \
    -B /home/dsh/cann-dev/fuzzy-commitment/build \
    -DCMAKE_ASC_RUN_MODE=cpu \
    -DCMAKE_ASC_ARCHITECTURES=dav-2201

cmake --build /home/dsh/cann-dev/fuzzy-commitment/build --parallel 2

/home/dsh/cann-dev/fuzzy-commitment/build/fuzzy_ldpc_test
