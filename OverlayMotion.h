#include <string>

extern "C" {
#include <libavutil/motion_vector.h>
}

#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;

// Demo for drawing 
class OverlayMotion {
    public:
        OverlayMotion(const std::string& _filename);
        ~OverlayMotion();
        void nextFrame(const AVMotionVector* vectors, const int cnt);
    private:
        VideoCapture capture;
        VideoWriter outputVideo;
};

