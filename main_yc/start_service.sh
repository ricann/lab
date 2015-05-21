#!/bin/sh
./arm_energy_v3 >> /dev/null &
./arm_Route >> /dev/null &
./camctrl -i 1.2.3.4 -c 4 >> /dev/null &
