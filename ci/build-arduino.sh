#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Define cores and platforms
CORES=(arduino:avr arduino:sam esp8266:esp8266 esp32:esp32)
PLATFORMS=(arduino:avr:uno arduino:sam:arduino_due_x_dbg esp8266:esp8266:nodemcu esp32:esp32:nodemcu-32s)

# Define additional URLs for arduino-cli
ADDITIONAL_URLS=http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

# Make sure we are inside the github workspace
cd $GITHUB_WORKSPACE

# Create directories
mkdir $HOME/Arduino
mkdir $HOME/Arduino/libraries

# Install dependencies
echo -e "\n########################################################################";
echo -e "${YELLOW}INSTALLING DEPENDENCIES"
echo "########################################################################";
# Install ESP32 dependencies
pip install pyserial

# Install Arduino IDE
echo -e "\n########################################################################";
echo -e "${YELLOW}INSTALLING ARDUINO CLI"
echo "########################################################################";
export PATH=$PATH:$GITHUB_WORKSPACE/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
arduino-cli config init
arduino-cli core update-index --additional-urls "${ADDITIONAL_URLS}"

# Install Arduino cores
for c in ${CORES[*]} ; do
    echo -e "\n########################################################################";
    echo -e "${YELLOW}INSTALLING ARDUINO CORE: ${c}"
    echo "########################################################################";
    arduino-cli core install $c --additional-urls "${ADDITIONAL_URLS}"
    if [ $? -ne 0 ]; then echo -e "${RED}\xe2\x9c\x96"; else echo -e "${GREEN}\xe2\x9c\x93"; fi
done

# Link Arduino library
ln -s $GITHUB_WORKSPACE $HOME/Arduino/libraries/CI_Test_Library

# Build example sketches
for p in ${PLATFORMS[*]} ; do
    for d in examples/* ; do
        echo -e "\n########################################################################";
        echo -e "${YELLOW}BUILD ${d} FOR ${p}"
        echo "########################################################################";
        arduino-cli compile -b $p $d
        if [ $? -ne 0 ]; then echo -e "${RED}\xe2\x9c\x96"; else echo -e "${GREEN}\xe2\x9c\x93"; fi
    done
done
