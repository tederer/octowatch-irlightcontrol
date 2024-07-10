# InfraredLight Service
This repository contains the source code of the InfraredLight service, which is part of the [Underwater Camera Project](https://underwater-camera-project.github.io).

## Features

The service provides the ability to dim the infrared LEDs in the underwater camera. To change the LEDs brightness, connect to the service on port 8886 and send the new brightness value followed by a new line character. Float values in the range [0,1] are allowed.

## Installation

The following steps describe how to install the monitoring service on your Raspberry Pi.

1.  Clone this repository
2.  Execute `compile.sh`

## Starting the service

Execute `start.sh`.
