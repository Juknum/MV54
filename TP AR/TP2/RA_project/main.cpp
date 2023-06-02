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

#include <visp3/core/vpImage.h>
#include <visp3/vision/vpKeyPoint.h>

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
    pose.setUseParallelRansac(1);
    pose.setNbParallelRansacThreads(12);
    pose.setRansacNbInliersToReachConsensus(point.size() * .6);
    pose.setRansacThreshold(0.05);
    double x = 0, y = 0;
    for (unsigned int i = 0; i < point.size(); i++) {
        vpPixelMeterConversion::convertPoint(cam, ip[i], x, y);
        point[i].set_x(x);
        point[i].set_y(y);
        pose.addPoint(point[i]);
    }

    pose.computePose(vpPose::RANSAC, cMo);
}

int main(int argc, char** argv) {

    unityInterface Camera;
    CameraParameters camParam{ 640, 480, 600, 600, 320, 240 };
    Camera.setCameraParameters(camParam);

    cv::VideoCapture g(0, cv::CAP_DSHOW);
    if (!g.isOpened()) {
        std::cout << "Failed to open the camera" << std::endl;
        return -1;
    }

    cv::Mat frame;

    vpImage<vpRGBa> Ivirtual(camParam.height, camParam.width);
    vpImage<vpRGBa> Ifinal(camParam.height, camParam.width);
    vpImage<vpRGBa> Ireal(camParam.height, camParam.width);
    vpImage<unsigned char> Icurrent(camParam.height, camParam.width);
    vpImage<unsigned char> Ireference(camParam.height, camParam.width);
    vpImage<unsigned char> Idisp;

    Idisp.resize(camParam.height, camParam.width * 2);

    vpKeyPoint::vpFilterMatchingType filterType = vpKeyPoint::ratioDistanceThreshold;
    vpKeyPoint keypoint("ORB", "ORB", "BruteForce-Hamming", filterType);

    std::string filename("D:/jconstan/Temp/UTBM.ppm");

    vpDisplayOpenCV dispIreal(Idisp, 0, 0, "Reference into Real value");
    vpDisplayOpenCV dispIfinal(Ifinal, 0, 0, "Final camera view");

    vpCameraParameters cam(600, 600, 320, 240);
    vpHomogeneousMatrix cMo(0, 0, 0.5, M_PI / 2, M_PI / 2, -M_PI / 2);



    try {
        vpImageIo::read(Ireference, filename);

        std::cout << "Photo trouvée" << std::endl;
    }
    catch (...) { // La photo n'a pas encore été prise
       
        while (1) {
            try {
                g >> frame; // get a new frame from camera
                vpImageConvert::convert(frame, Icurrent);
                vpDisplay::display(Icurrent);
                vpDisplay::flush(Icurrent);
            }
            catch (...) {
            }

            if (vpDisplay::getClick(Icurrent, false))
                break;
        }

        vpImageIo::write(Icurrent, filename); // Write the image in a PGM P5 image file format
        vpImageIo::read(Ireference, filename);
        std::cout << "Photo prise" << std::endl;
    }

    keypoint.buildReference(Ireference);
    Idisp.insert(Ireference, vpImagePoint(0, 0));

    while (1) {
        try {
            g >> frame;

            vpImageConvert::convert(frame, Icurrent);
            vpImageConvert::convert(frame, Ifinal);
            vpImageConvert::convert(frame, Ireal);

            Idisp.insert(Icurrent, vpImagePoint(0, Icurrent.getWidth()));

            vpDisplay::display(Idisp);
            
            std::vector<vpImagePoint> ip;
            std::vector<vpPoint> point;
            unsigned int nbMatch = keypoint.matchPoint(Icurrent);
            std::cout << "Matches : " << nbMatch << std::endl;

            vpImagePoint iPref, iPcur;
            for (unsigned int i = 0; i < nbMatch; i++) {
                keypoint.getMatchedPoints(i, iPref, iPcur);
                vpDisplay::displayLine(Idisp, iPref, iPcur + vpImagePoint(0, Icurrent.getWidth()), vpColor::green);

                ip.push_back(iPcur);
                point.push_back(vpPoint(.297 * iPref.get_i() / 640 - (.297 / 2), .21 * iPref.get_j() / 480 - (.21 /2), 0));
            }

            computePose(point, ip, cam, cMo);

            Camera.setCameraPose(vpPoseVector(cMo));
            Camera.getImage(Ivirtual);

            for (int i = 0; i < Ireal.getHeight(); i++) {
                for (int j = 0; j < Ireal.getWidth(); j++) {
                    Ifinal[i][j] = Ivirtual[i][j].R == 0 && Ivirtual[i][j].G == 255 && Ivirtual[i][j].B == 0
                        ? Ireal[i][j]
                        : Ivirtual[i][j];
                }
            }

            vpDisplay::display(Ifinal);
            vpDisplay::flush(Ifinal);
            vpDisplay::flush(Idisp);


            if (vpDisplay::getClick(Idisp, false))
                break;
        }
        catch (...) {

        }
    }


    return 0;
}
