#ifndef UNITYINTERFACE_H
#define UNITYINTERFACE_H

//#include <boost/property_tree/json_parser.hpp>
#include <curl/curl.h>
#include <visp/vpImage.h>
#include <visp/vpImageConvert.h>



typedef struct MemoryStruct{
    char *memory;
    size_t size;
}MemoryStruct;


typedef struct CameraParameters{
    int width, height;
    float au, av;
    float u0, v0;
}CameraParameters;

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
CURLcode postJson(CURL *curl, std::string json, std::string path);
CURLcode getData(CURL *curl, MemoryStruct &chunk, std::string path);

class unityInterface {

private:
    CURL *curl;
    CURLcode res;
    std::string url, imagePath, configPath, posePath;
    int port;
    MemoryStruct chunk;

public:
    unityInterface(std::string url_="http://localhost", int port_=4649);
    ~unityInterface();

    // Get grayscale image
    bool getImage(vpImage<unsigned char> &I);
    // Get RGB image
    bool getImage(vpImage<vpRGBa> &I);


    // Get/Set camera pose
    bool getCameraPose(vpPoseVector &p);
    bool setCameraPose(vpPoseVector p);

    // Get/Set camera intrinsics
    bool getCameraParameters(CameraParameters &cam);
    bool setCameraParameters(CameraParameters cam);


};


#endif //UNITYINTERFACE_H
