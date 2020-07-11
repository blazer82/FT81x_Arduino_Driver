#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

# Create directories
mkdir $HOME/Arduino
mkdir $HOME/Arduino/libraries

# Install arduino IDE
echo -e "\n########################################################################";
echo -e "${YELLOW}INSTALLING ARDUINO CLI"
echo "########################################################################";
export PATH=$PATH:$GITHUB_WORKSPACE/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
arduino-cli config init
arduino-cli core update-index

# Install arduino cores
echo -e "\n########################################################################";
echo -e "${YELLOW}INSTALLING ARDUINO AVR CORE"
echo "########################################################################";
arduino-cli core install arduino:avr
if [ $? -ne 0 ]; then echo -e "${RED}\xe2\x9c\x96"; else echo -e "${GREEN}\xe2\x9c\x93"; fi

# Link arduino library
ln -s $GITHUB_WORKSPACE $HOME/Arduino/libraries/CI_Test_Library

# Build example sketches
echo -e "\n########################################################################";
echo -e "${YELLOW}BUILDING EXAMPLE SKETCHES"
echo "########################################################################";
for d in examples/* ; do
    echo -e "\n########################################################################";
    echo -e "${YELLOW}BUILD ${d}"
    echo "########################################################################";
    arduino-cli compile -b arduino:avr:uno $d
    if [ $? -ne 0 ]; then echo -e "${RED}\xe2\x9c\x96"; else echo -e "${GREEN}\xe2\x9c\x93"; fi
done
