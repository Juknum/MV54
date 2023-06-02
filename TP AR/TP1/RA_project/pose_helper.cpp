#include <visp3/io/vpImageIo.h>
#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>
#include <visp/vpPose.h>

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