# Bad Apple OpenGL

A simple Bad Apple implementation with audio in OpenGL. If you are unfamiliar with Bad Apple, check out the link [here](https://www.youtube.com/watch?v=FtutLA63Cp8).

## Building the Project

### Prerequisites

Ensure you have a C++20 compliant compiler and CMake 3.20+ installed. 

### Building from the command line

1. Generate the build files using the standard CMake CLI.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

2. Compile the project.

```bash
cmake --build build --config Release
```

3. Run the project.

The compiled files will be in the `build` directory:'

```bash
./build/bin/my_executable
# or for Visual Studio/Windows:
.\build\bin\Release\my_executable.exe
```

## Code Overview

This codebase uses only a few dependencies:

* GLFW for windowing
* GLAD for OpenGL extension loading
* Miniaudio for audio playback

CMake is used for building and managing dependencies.

## File Structure

C++ files are organized into the typical structure: source files go in the `src` folder, and header files go in the `include` folder. Dependencies are located in the `vendor` folder. OpenGL shaders are in the `assets/shaders` folder.

The video file is provided in `media/bad_apple.mp4`, and it can be directly replaced. `scripts/encode_video.py` is responsible for decoding that video file into a compressed binary file and an audio wav file in the `assets/generated` folder. 

The `scripts` folder uses the [uv package manager](https://github.com/astral-sh/uv).

The video file is compressed using Run-Length Encoding (RLE). For every byte, the most significant bit (MSB) indicates the color, where 1 indicates white and 0 indicates black. The remaining 7 bits indicate the run-length of that color up to 127. 
