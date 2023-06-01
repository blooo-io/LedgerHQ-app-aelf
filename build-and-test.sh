#!/usr/bin/env bash

./docker-make clean
docker build -t ledger-app-builder:latest .
docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ledger-app-builder:latest /bin/bash -c "BOLOS_SDK=/opt/nanos-secure-sdk make && sh ./python-test.sh nanos"
docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ledger-app-builder:latest /bin/bash -c "BOLOS_SDK=/opt/nanox-secure-sdk make && sh ./python-test.sh nanox"
docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ledger-app-builder:latest /bin/bash -c "BOLOS_SDK=/opt/nanosplus-secure-sdk make && mv build/nanos2 build/nanosp && sh ./python-test.sh nanosp"
