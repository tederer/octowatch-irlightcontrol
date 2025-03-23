# InfraredLight Service
This repository contains the source code of the InfraredLight service, which is part of the [Underwater Camera Project](https://underwater-camera-project.github.io).

## Features

The service provides the ability to dim the infrared LEDs in the underwater camera by using pulse width modulation (PWM).

## Installation

The installation of the Video Service consists of two parts: firstly, all the necessary dependencies must be made available, and then the service can be compiled.

### Installing the Dependencies

The following commands can be used to install the required dependencies.

```bash
apt update

apt install git meson pkg-config cmake g++ clang ninja-build \
    libyuv libyaml-dev python3-yaml python3-ply python3-jinja2 \
    git meson pkg-config cmake g++ clang ninja-build libyaml-dev \
    python3-yaml python3-ply python3-jinja2 wget libjpeg-dev zip

# pigpio
cd
wget https://github.com/joan2937/pigpio/archive/master.zip
unzip master.zip
cd pigpio-master
make
make install

# boost
cd
wget https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz
tar -xf boost_1_82_0.tar.gz
``` 

### Compiling the Video Service

The following commands can be used to compile the Video Service:

```bash
cd
git clone https://github.com/tederer/octowatch-irlightcontrol.git
cd octowatch-irlightcontrol
./compile.sh
```

## Starting the service

Execute `start.sh`.

## Infrared Light Control Interface

The interface for controlling the intensity of the infrared LEDs was implemented as a line-based TCP connection, which listens on port 8886. Sending the new intensity in percent (range: 0.0 - 1.0) followed by a carriage return causes the InfraredLight Service to adapt the PWM signal accordingly.
