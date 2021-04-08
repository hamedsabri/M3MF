# Building

## Getting and Building the Code

You can either use CMake commands directly or opt into the supplied **build.py** script that does the work for you. This script builds the project and installs all of the necessary libraries and plug-ins for you. Follow the instructions below to learn how to use the script.

#### 1. Tools and System Prerequisites

Before building the project, consult the following table to ensure you use the recommended version of compiler, operating system, cmake, etc. 

|        Required       | ![](images/windows.png)   |                            ![](images/mac.png)               |   ![](images/linux.png)     |
|:---------------------:|:-------------------------:|:------------------------------------------------------------:|:---------------------------:|
|    Operating System   |         Windows 10        | Catalina (10.15), Mojave (10.14), High Sierra (10.13), Big Sur (11.2.2)        |       CentOS 7/8           |
|   Compiler Requirement| <br>Maya 2019/2020/2021 (VS 2017/2019) | <br>Maya 2019/2020/2021 (Xcode version 11/12) | <br>Maya 2019/2020/2021 (GCC 9) |
| Minimum Cmake Version |           3.15            |                             3.15                             |         3.15                |
|    Build generator    | Visual Studio, Ninja (Recommended)    |  XCode, Ninja (Recommended)                      |    Ninja (Recommended)      |
|    Command processor  | Visual Studio X64 2017,2019 command prompt  |                     bash                |             bash            |
| Supported 3DSMax Version|    2019, 2020, 2021, 2022           |                      N/A                 |        N/A     |
| Supported Maya Version|      2019, 2020, 2022           |                      2019, 2020, 2021                 |        2019, 2020, 2021     |

#### 2. Download the source code

Start by cloning the repository:
```
git clone https://github.com/hamedsabri/M3MF.git
cd M3MF
```

## Build

#### 3. How To Use CMake Commands

```
cmake                                           \
-DMAXSDK_LOCATION=/path/to/max_sdk_location     \
-DMAYA_LOCATION=/path/to/maya_runtime_location  \
/path/to/top_level_cmake_lists

cmake --build . --target install 
```

#### How To Use build.py Script

##### Arguments

There are three arguments that must be passed to the script: 

| Flags                 | Description                                                                           |
|--------------------   |-------------------------------------------------------------------------------------- |
|  --maya-location      | directory where Maya is installed.                                                    |
|  --maxsdk-location    | directory where 3DSMax SDK is installed.                                              |
| workspace_location    | directory where the project use as a workspace to build and install.                  |

e.g
```
Linux:
➜ M3MF python build.py --maya-location <path_to_maya> /usr/local/workspace

MacOSX:
➜ M3MF python build.py --maya-location <path_to_maya> /opt/local/workspace

Windows:
c:\M3MF> python build.py --maya-location <path_to_maya> --maxsdk-location <path_to_3dsmax_sdk> C:\workspace
```

##### Repository Layout

| Location      | Description                                                                                   |
|-------------  |---------------------------------------------------------------------------------------------  |
| plugin/3dsmax | The Autodesk 3DSMax plugin.                                                                    |
|  plugin/maya  | The Autodesk Maya plugin.                                                                      |
|  thirdParty   | Lib3MF 2.1.0 source code.                                                                     |

***NOTE:*** Top level CMake in Lib3MF source code has been modified to build as a `static library`. There are number of issues with the way Lib3MF CMake is setup which makes it impossible to levergae FetchContent to build it.

##### Build Arguments

| Flag                  | Description                                                                           |
|--------------------   |---------------------------------------------------------------------------------------|
|   --build-args        | comma-separated list of cmake variables can be also passed to build system.           |

e.g
```
--build-args="-DBUILD_3DSMAX_PLUGIN=ON,-DBUILD_MAYA_PLUGIN=OFF,-DBUILD_STRICT_MODE=OFF"
```

##### CMake Options

Name                        | Description                                                | Default
---                         | ---                                                        | ---
BUILD_3DSMAX_PLUGIN         | builds 3MF plugin for Autodesk 3DSMAX.                     | ON
BUILD_MAYA_PLUGIN           | builds 3MF plugin for Autodesk Maya.                 		 | ON
BUILD_STRICT_MODE           | enforces all warnings as errors.                           | ON

##### Stages

| Flag                 | Description                                                                           |
|--------------------  |--------------------------------------------------------------------------------------------------- |
|   --stages           | comma-separated list of stages can also be passed to the build system. By default "clean, configure, build, install" stages are executed if this argument is not set. |

| Options       | Description                                                                                   |
|-----------    |---------------------------------------------------                                            |
| clean         | clean build                                                                                   |
| configure     | call this stage every time a cmake file is modified                                           |
| build         | builds the project                                                                            |
| install       | installs all the necessary plug-ins and libraries                                             |
| package       | bundles up all the installation files as a zip file inside the package directory              |

e.g
```
--stages=configure,build,install
```
***NOTE:*** All the flags can be followed by either ```space``` or ```=```

##### CMake Generator

It is up to the user to select the CMake Generator of choice, but the use of the Ninja generator is encouraged. To use the Ninja Generator, you need to first install the Ninja binary from https://ninja-build.org/

You then need to set the generator to ```Ninja``` and the ```CMAKE_MAKE_PROGRAM``` variable to the Ninja binary you downloaded.
```
python build.py --generator Ninja --build-args=-DCMAKE_MAKE_PROGRAM='path to ninja binary'
```

##### Build and Install locations

By default, the build and install directories are created inside the **workspace** directory. However, you can change these locations by setting the ```--build-location``` and ```--install-location``` flags. 

##### Build Log

By default the build log is written into ```build_log.txt``` inside the build directory. If you want to redirect the output stream to the console instead, you can pass ```--redirect-outstream-file``` and set it to false.

##### Additional flags and options

Run the script with the ```--help``` parameter to see all the possible flags and short descriptions.

