# Siliconray
The following proyect was made as a homework for my Embeded System's Design class.
It is supposed to be a molecule of silicon- fully rendered using Raytracing and with no external libraries.

The only library used was [stb image](https://github.com/nothings/stb), which is only used to write the image buffer directly
to PNG. The resulting GIF can be created using ffmpeg:

```bash
$ ffmpeg -framerate 30 -i %d.png -i palette.png -lavfi "paletteuse" output.gif
```

## Building
1. Having cmake installed, create a build directory and configure the project.
```bash
$ mkdir build
$ cd build
$ cmake ..
```

2. Use make to build the project.
```bash
$ make
```

3. Enjoy! You can execute the program using:
```bash
$ ./siliconray
```

> NOTE: The following project was only tested on Linux (x86_64).
> However it could possibly run on Windows and OS-X given the correct tools are installed.

## Example GIF
![silicio](https://github.com/foxinuni/siliconray/blob/main/gen/output.gif)
