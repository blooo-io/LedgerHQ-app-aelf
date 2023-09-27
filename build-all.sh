#!/usr/bin/env bash

export BOLOS_SDK=$NANOS_SDK && make -j
cp bin/app.elf tests/elfs/aelf_nanos.elf

export BOLOS_SDK=$NANOX_SDK && make -j
cp bin/app.elf tests/elfs/aelf_nanox.elf

export BOLOS_SDK=$NANOSP_SDK && make -j
cp bin/app.elf tests/elfs/aelf_nanosp.elf

# Uncomment the following lines if you wish to generate snapshots
# pytest tests/python/ --tb=short -v --device nanos --golden_run
# pytest tests/python/ --tb=short -v --device nanox --golden_run
# pytest tests/python/ --tb=short -v --device nanosp --golden_run

# Uncomment the following lines if you wish to run functional tests
pytest tests/python/ --tb=short -v --device nanos
pytest tests/python/ --tb=short -v --device nanox
pytest tests/python/ --tb=short -v --device nanosp