#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Create directories
mkdir $HOME/Arduino
mkdir $HOME/Arduino/libraries

# Install arduino IDE
export PATH=$PATH:$GITHUB_WORKSPACE/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
arduino-cli config init
arduino-cli core update-index

# Install arduino cores
arduino-cli core install arduino:avr

# Link arduino library
ln -s $GITHUB_WORKSPACE $HOME/Arduino/libraries/CI_Test_Library

# Build example sketches
arduino-cli compile -b arduino:avr:uno examples/HelloWorld 
