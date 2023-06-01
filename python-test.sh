#!/usr/bin/env bash
device_label=$1 # "nanos", "nanox", "nanosp"
cp build/${device_label}/bin/app.elf bin/aelf_${device_label}.elf
mkdir tests/elfs
cp bin/*.elf tests/elfs

pytest -s tests/python/ -v --tb=short --device ${device_label}