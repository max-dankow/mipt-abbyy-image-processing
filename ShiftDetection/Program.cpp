#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

struct Shift {
    Shift(int vertical, int horisontal) : vertical(vertical), horisontal(horisontal) { }

    int vertical;
    int horisontal;
};

template <typename T>
void Print(T value, const std::string& message = "") {
    std::cout << message << ": " << value << std::endl;
}

class ShiftDetector {
public:
    static Shift DetectShift(InputArray baseImage, InputArray image) {
        cv::Mat baseImageGrey;
        cv::cvtColor(baseImage, baseImageGrey, CV_BGR2GRAY);
        baseImageGrey.convertTo(baseImageGrey, CV_32FC1);

        cv::Mat imageGrey;
        cv::cvtColor(image, imageGrey, CV_BGR2GRAY);
        imageGrey.convertTo(imageGrey, CV_32FC1);

        vector<Mat> refChannels;
        split(baseImageGrey, refChannels);

        vector<Mat> refChannels2;
        split(imageGrey, refChannels2);

        Print(baseImageGrey.channels(), "grey channels");
        Print(baseImageGrey.type(), "grey type");

        Point2d shift = phaseCorrelate(baseImageGrey, imageGrey);
        return Shift(shift.y, shift.x);
    }
};

Mat translateImg(Mat &img, int offsetx, int offsety)
{
    Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img, img, trans_mat, img.size());
    return img;
}

int main(int argc, char** argv)
{
    string baseImageName;
    string imageName;

    baseImageName = argv[1];
    imageName = argv[2];

    Mat baseImage;
    baseImage = imread(baseImageName.c_str(), IMREAD_COLOR); // Read the file
    if (baseImage.empty())                                   // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    Mat image;
    image = imread(imageName.c_str(), IMREAD_COLOR);     // Read the file
    if (image.empty())                                       // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.



    Shift shift = ShiftDetector::DetectShift(baseImage, image);

    std::cout << shift.horisontal << ' ' << shift.vertical << std::endl;

    cv::Mat out;
    cv::Mat out1;
    cv::Mat out2;
    //cv::vconcat(baseImage, image, out);
    cv::add(baseImage, image, out1);
    out1 = out1 / 2;
    cv::add(baseImage, translateImg(image, -shift.horisontal, -shift.vertical), out2);
    out2 = out2 / 2;
    cv::hconcat(out1, out2, out);
    imshow("Display window", out); // Show our image inside it.
    waitKey(0);                    // Wait for a keystroke in the window
    return 0;
}