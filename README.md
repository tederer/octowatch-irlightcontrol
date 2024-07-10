# InfraredLight Service
This repository contains the source code of the InfraredLight service, which is part of the [Underwater Camera Project](https://underwater-camera-project.github.io).

## Features

The service provides the ability to dim the infrared LEDs in the underwater camera. To change the LEDs brightness, connect to the service on port 8886 and send the new brightness value followed by a new line character. Float values in the range [0,1] are allowed.

## Installation

The following steps describe how to install the monitoring service on your Raspberry Pi.

1. Execute `sudo apt-get update`.
2. Execute `sudo apt install nodejs npm` to install the JavaScript runtime environment.
5.  Clone this repository
6.  Execute `build.sh`

## Starting the service

Execute `start.sh`.
