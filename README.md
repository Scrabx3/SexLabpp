# SexLab P+

A high performance and stability patch for SexLab for Skyrim SE.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE/tree/dev)
	* You need to build from the powerof3/dev branch

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/Scrabx3/SexLabpp.git
cd SexLabpp
# pull commonlib
git submodule init
# to update submodules to checked-out build (warning, pulling newer verisons may result in build problems)
git submodule update
```

### Skyrim SE 1.5
```
cmake --preset vs2022-windows-vcpkg-se
cmake --build build/vs2022-SE --config Release
```
### Skyrim SE 1.6
```
cmake --preset vs2022-windows-vcpkg-ae
cmake --build build/vs2022-AE --config Release
```
