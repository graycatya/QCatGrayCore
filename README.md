# QCatGrayCore

QCatGrayCore is a system call library based on qt implementation.

## Submodule

CmakeLists.txt in the QCatGrayCore main directory is responsible for introducing the submodules of the src directory, each of which does not directly depend on each other.

### QCatGrayNetWork

The module provides an http request api.

### QCatGraySerial

Provides cross-platform serial port communication and supports hot swap.

### QCatGrayUniversal

Provides threads and thread pools, software profiles, fonts and other application basic class interfaces.