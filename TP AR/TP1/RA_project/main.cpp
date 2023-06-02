#include <iostream>

#include <visp/vpIoTools.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpDisplayX.h>

#include <visp3/core/vpConfig.h>
#include <visp3/blob/vpDot2.h>

#include <visp3/io/vpImageIo.h>
#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>

#include <visp/vpPose.h>

#ifdef VISP_HAVE_MODULE_SENSOR
#include <visp3/sensor/vp1394CMUGrabber.h>
#include <visp3/sensor/vp1394TwoGrabber.h>
#endif

#include "unityInterface.h"

using namespace cv;
using namespace std;

void computePose(std::vector<vpPoint>& point, const std::vector<vpImagePoint>& ip, const vpCameraParameters& cam, vpHomogeneousMatrix& cMo) {
    vpPose pose;

    double x = 0, y = 0;
    for (unsigned int i = 0; i < point.size(); i++) {
        vpPixelMeterConversion::convertPoint(cam, ip[i], x, y);
        point[i].set_x(x);
        point[i].set_y(y);
        pose.addPoint(point[i]);
    }

    pose.computePose(vpPose::LAGRANGE, cMo);
}

int main(int argc, char **argv) {

    unityInterface Camera;
    CameraParameters camParam{640, 480, 600, 600, 320, 240};
    Camera.setCameraParameters(camParam);

    vpImage<vpRGBa> Ivirtual(camParam.height, camParam.width);
    vpImage<vpRGBa> Ireal(camParam.height, camParam.width);
    vpImage<vpRGBa> Ifinal(camParam.height, camParam.width);
    vpImage<unsigned char> IGray(camParam.height, camParam.width);

    cv::VideoCapture g(0, cv::CAP_DSHOW);
    if (!g.isOpened()) {
        std::cout << "Failed to open the camera" << std::endl;
        return -1;
    }
    
    cv::Mat frame;
    g >> frame; 

    vpImageConvert::convert(frame, Ireal);
    vpDisplayOpenCV dispIreal(IGray, 0, 0, "Real camera view");
    vpDisplayOpenCV dispIfinal(Ifinal, 0, 0, "Final camera view");
    vpCameraParameters cam(600, 600, 320, 240);
    vpHomogeneousMatrix cMo(0, 0, 0.5, M_PI / 2, M_PI / 2, -M_PI / 2);

    std::vector<vpDot2> dot(4);
    std::vector<vpImagePoint> ip(4);
    std::vector<vpPoint> point;

    point.push_back(vpPoint(-0.06, -0.06, 0));
    point.push_back(vpPoint( 0.06, -0.06, 0));
    point.push_back(vpPoint( 0.06,  0.06, 0));
    point.push_back(vpPoint(-0.06,  0.06, 0));

    std::vector<bool> inits_done(4);
    bool global_init = true;

    for (int i = 0; i < 4; i++) {
        vpDot2 blob;
        blob.setGraphics(true);
        blob.setGraphicsThickness(2);
        dot[i] = blob;

        vpImagePoint germ;
        ip[i] = germ;

        bool init_done = false;
        inits_done[i] = init_done;
    }

    while (1) {

        try {
            g >> frame; // get a new frame from camera
            vpImageConvert::convert(frame, Ireal);
            vpImageConvert::convert(Ireal, IGray);
            vpImageConvert::convert(Ireal, Ifinal);

            vpDisplay::display(IGray);
            vpDisplay::display(Ifinal);

            for (int i = 0; i < 4; i++) {
                if (!inits_done[i]) {
                    vpDisplay::displayText(IGray, vpImagePoint(10 * (double)i, 10), "Click in the blob to init the tracker", vpColor::red);

                    if (vpDisplay::getClick(IGray, ip[i], false)) {
                        dot[i].initTracking(IGray, ip[i]);
                        inits_done[i] = true;
                    }
                }

                else {
                    dot[i].track(IGray);
                    ip[i] = dot[i].getCog();
                }
            }

            if (inits_done[0] && inits_done[1] && inits_done[2] && inits_done[3]) {
                computePose(point, ip, cam, cMo);
                vpDisplay::displayFrame(IGray, cMo, cam, 0.05, vpColor::none);

                for (int i = 0; i < Ireal.getHeight(); i++) {
                    for (int j = 0; j < Ireal.getWidth(); j++) {
                        if (Ivirtual[i][j].R == 0 && Ivirtual[i][j].G == 255 && Ivirtual[i][j].B == 0) {
                            Ifinal[i][j].R = Ireal[i][j].R;
                            Ifinal[i][j].G = Ireal[i][j].G;
                            Ifinal[i][j].B = Ireal[i][j].B;
                        }

                        else {
                            Ifinal[i][j].R = Ivirtual[i][j].R;
                            Ifinal[i][j].G = Ivirtual[i][j].G;
                            Ifinal[i][j].B = Ivirtual[i][j].B;
                        }
                    }
                }
            }

            vpDisplay::flush(IGray);
            vpDisplay::flush(Ifinal);
        }
        catch (...) {
            for (int i = 0; i < 4; i++) {
                inits_done[i] = false;
            }
        }
  
        Camera.setCameraPose(vpPoseVector(cMo));
        Camera.getImage(Ivirtual);

        if (vpDisplay::getClick(Ireal, false))
            break;

    }
   

    return 0;
}
