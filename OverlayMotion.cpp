#include "OverlayMotion.h"

#include <cmath>
#include <iostream>
#include <map>

using namespace cv;

// altered video written here
#define OUTPUT_FILE "output.mp4"

// how big of an arrow tip to draw for the motion vectors
#define MOTION_VECTOR_TIP_LENGTH 1.0

// this is reasonable for 1280x720 demo, but should be dynamic based on resolution/etc.
#define MOTION_VECTOR_MINIMUM_MAGNITUDE 100

// reduce thickness of motion vector arrows for 8x8 macroblocks
#define WEIGHT_REDUCTION_8X8_MACROBLOCK 0.5

// how thick of an arrow to draw for the motion vectors (magnitude of MV => thickness)
static const std::map<int, int> MOTION_VECTOR_ARROW_WIDTH_MAP {
  {400, 6}, {250, 5}, {200, 4}, {150, 3}, {100, 2}, {75, 1}, {INT_MAX, 7}
};

OverlayMotion::OverlayMotion(const std::string& src_filename)
{
    // Open input
    capture.open(src_filename);
    if (!capture.isOpened()) {
        throw std::ios_base::failure("Could not open the input video: " + src_filename);
    }

    // Write to this file
    Size S = Size((int)capture.get(CAP_PROP_FRAME_WIDTH), (int)capture.get(CAP_PROP_FRAME_HEIGHT));
    outputVideo.open(OUTPUT_FILE, static_cast<int>(capture.get(CAP_PROP_FOURCC)), capture.get(CAP_PROP_FPS), S, true);
    if (!outputVideo.isOpened()) {
        throw std::ios_base::failure("Could not open output video for write: " + std::string(OUTPUT_FILE));
    }
}

OverlayMotion::~OverlayMotion()
{
    if (capture.isOpened()) {
        capture.release();
    }
    if (outputVideo.isOpened()) {
        outputVideo.release();
    }
}

void OverlayMotion::nextFrame(const AVMotionVector* vectors, const int cnt)
{    
    Mat frame;
    capture >> frame;
    if (frame.empty()) {
        return;
    }
    
    // draw motion vector arrows atop the frame
    for (int i=0; i<cnt; ++i) {
        const AVMotionVector *mv = vectors + i;

        // arrow overlay magnitude
        double xfactor = (mv->w <= 8 ? WEIGHT_REDUCTION_8X8_MACROBLOCK : 1);
        double yfactor = (mv->h <= 8 ? WEIGHT_REDUCTION_8X8_MACROBLOCK : 1);
        double mag = std::sqrt(mv->motion_x * mv->motion_x * xfactor + mv->motion_y * mv->motion_y * yfactor);
        if (mag < MOTION_VECTOR_MINIMUM_MAGNITUDE) { 
            continue; // too noisy if draw a bunch of little lines
        }
        
        // arrow overlay thickness
        int thickness = MOTION_VECTOR_ARROW_WIDTH_MAP.upper_bound(mag)->second;

        // arrow overlay direction (todo check VUI for out-of-frame MV)
        cv::Point pt_start(mv->src_x, mv->src_y);
        auto stop_x = mv->src_x + (mv->motion_x > 0 ? mv->w : -mv->w);
        auto stop_y = mv->src_y + (mv->motion_y > 0 ? mv->h : -mv->h);
        cv::Point pt_stop(stop_x, stop_y);

        // draw arrow
        cv::arrowedLine(
            frame,
            pt_start,
            pt_stop,
            cv::Scalar(0, 0, 255, 125),
            thickness,
            8, // line_type
            0, // shift
            MOTION_VECTOR_TIP_LENGTH );
    }

    // save file
    outputVideo << frame;
    // imshow("Frame",frame);
    // waitKey(20);
}