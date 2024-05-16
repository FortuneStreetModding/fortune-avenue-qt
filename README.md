# fortune-avenue-qt - Fortune Avenue
## Summary
`fortune-avenue-qt` is the cross-platform port of Fortune Avenue. This tool allows modification of Boom Street, Fortune Street, and Itadaki Street Wii board files.
## Table of Contents
* [Summary](#summary)
* [Table of Contents](#table-of-contents)
* [Getting Started](#getting-started)
    * [Installing](#installing)
    * [Using Fortune Avenue](#using-fortune-avenue)
* [Building from Source](#building-from-source)
    * [Build Requirements](#build-requirements)
    * [Build Steps](#initial-steps)
* [Contributing](#contributing)
## Getting Started
### Installing
Go to the [Releases](https://github.com/FortuneStreetModding/fortune-avenue-qt/releases) page and download the latest version for your system.

### Using Fortune Avenue
For help using CSMM, please see the [Fortune Avenue User Manual](https://github.com/FortuneStreetModding/fortunestreetmodding.github.io/wiki/Fortune-Avenue-User-Manual).

## Building from Source
**IMPORTANT: Make sure to use --recursive when cloning this repository, in order to initialize the submodules!**

### Build Requirements

- Qt 6.6.1
- CMake
- MSVC 2019 or later (Windows)

### Build Steps
To start, clone this repository recursively Adding the `--recursive` flag is important as doing so will initialize the linked submodules:

`git clone git@github.com:FortuneStreetModding/fortune-avenue-qt.git --recursive`

Once that's done, you should be able to simply open `CMakeLists.txt` with Qt Creator and build the project with CMake.

## Contributing
We welcome contributions! If you would like to contribute to the development of `fortune-avenue-qt`, please feel free to [submit a PR](https://github.com/FortuneStreetModding/fortune-avenue-qt/pulls) with your changes, create an [Issue](https://github.com/FortuneStreetModding/fortune-avenue-qt/issues) to request a change, or join our [Discord server](https://discord.gg/DE9Hn7T) to further discuss the future of Fortune Street modding!
