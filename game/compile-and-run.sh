#!/bin/bash
set -euxo pipefail

./compile.sh
ymir-sdl3 --disc game.iso
exit 0
