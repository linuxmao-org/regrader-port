# Regrader

[![Build Status](https://travis-ci.com/linuxmao-org/regrader.svg?branch=master)](https://travis-ci.com/linuxmao-org/regrader)

A delay effect where the repeats degrade in resolution

## About

This is a port of the Regrader plugin created by Igorski.
It is based on the DISTRHO plugin framework (DPF) to provide support of
GNU/Linux and other platforms.

Based on the upstream revision: 534fee5

## Downloads

**Development**

- All: download from [Automatic builds](https://github.com/linuxmao-org/regrader/releases/tag/automatic).

## Build instructions

1. Obtain prerequisites

Install needed packages:

- `git`
- `build-essential`
- `pkg-config`
- `libx11-dev`
- `libcairo2-dev`
- `libjack-jackd2-dev` or `libjack-dev`
- `mesa-common-dev`

2. Check out the repository and submodules

```
git clone https://github.com/linuxmao-org/regrader.git
cd regrader
git submodule update --init
```

3. Compile

```
make
```

4. Install

```
sudo make install  # to install in system directories, or
make install-user  # to install in the home directory
```
