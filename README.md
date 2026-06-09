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

## Replacing the Video

To use a different video, replace `media/bad_apple.mp4` with your own `.mp4` file. Then run the video encoding script to regenerate the files used by the program:

```bash
uv run scripts/encode_video.py
```

The script decodes the video into two generated assets:

* a compressed binary video file
* a `.wav` audio file

Both files are written to the assets/generated folder. The OpenGL program reads from this generated folder at runtime, so the video must be re-encoded whenever media/bad_apple.mp4 is replaced.

The encoded video uses Run-Length Encoding (RLE). Each byte stores one run of pixels: the most significant bit represents the color, where 1 is white and 0 is black, while the remaining 7 bits store the run length, up to a maximum of 127 pixels.

## Code Overview

This codebase uses only a few dependencies:

* GLFW for windowing
* GLAD for OpenGL extension loading
* Miniaudio for audio playback

CMake is used for building and managing dependencies.

## File Structure

C++ files are organized into the typical structure: source files go in the `src` folder, and header files go in the `include` folder. Dependencies are located in the `vendor` folder. OpenGL shaders are in the `assets/shaders` folder.

The `scripts` folder uses the [uv package manager](https://github.com/astral-sh/uv).
