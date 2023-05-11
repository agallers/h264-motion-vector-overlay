# h264-motion-vector-overlay
Simple example of extracting and drawing an overlay of motion vectors

![](example-screenshot.png?raw=true)

## Credits
main.cpp is a copy of [FFmpeg's example extract_mvs.c](https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/extract_mvs.c)

## Build Instructions
g++ --std=c++11 main.cpp OverlayMotion.cpp `pkg-config --cflags --libs libavutil` `pkg-config --cflags --libs libavformat` `pkg-config --cflags --libs opencv4`

## Run Instructions
./a.out input.mp4
open output.mp4

## Dependencies
[OpenCV4](https://github.com/opencv/opencv)
[FFmpeg](http://ffmpeg.org/)
