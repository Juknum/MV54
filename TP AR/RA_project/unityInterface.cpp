#include "unityInterface.h"

unityInterface::unityInterface(std::string url_, int port_){
    url=url_;
    port=port_;

    imagePath="image";
    configPath="config";
    posePath="pose";

    chunk.memory = (char*)malloc(1); // will be grown as needed by the realloc above
    chunk.size = 0; // no data at this point

    curl=curl_easy_init();
}

unityInterface::~unityInterface(){
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    if(chunk.memory){
        free(chunk.memory);
    }
}


bool unityInterface::getImage(vpImage<vpRGBa> &I) {
    res = getData(curl, chunk, url + ":" + std::to_string(port) + "/" + imagePath);

    if (res != CURLE_OK) {
        fprintf(stderr, "getImage() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return false;
    } else {
        int *chunkint = (int*)chunk.memory;
        int w,h;
        w = chunkint[0];
        h = chunkint[1];
        I.resize(h,w);

        char *chunkfloat = (char*)(chunk.memory+16);
        for(int v=0;v<h;v++){
            for(int u=0;u<w;u++) {
                I[h-v-1][u].R = chunkfloat[(u+v*w)<<2];
                I[h-v-1][u].G = chunkfloat[((u+v*w)<<2)+1];
                I[h-v-1][u].B = chunkfloat[((u+v*w)<<2)+2];
            }
        }

    }

    return true;
}

bool unityInterface::getImage(vpImage<unsigned char> &I) {
    bool getimage_state;
    vpImage<vpRGBa> Irgb;
    getimage_state = this->getImage(Irgb);
    vpImageConvert::convert(Irgb, I);
    return getimage_state;
}


bool unityInterface::setCameraPose(vpPoseVector p) {
    //Format :  {"tx":-13.9,"ty":-140,"tz":-75,"rx":-1.20919955,"ry":-1.20919955,"rz":-1.20919955}
    std::string json = "{\"tx\":"+std::to_string(p[0])
                      +",\"ty\":"+std::to_string(p[1])
                      +",\"tz\":"+std::to_string(p[2])
                      +",\"rx\":"+std::to_string(p[3])
                      +",\"ry\":"+std::to_string(p[4])
                      +",\"rz\":"+std::to_string(p[5])+"}";

    res = postJson(curl, json, url + ":" + std::to_string(port) + "/" + posePath);

    if (res != CURLE_OK) {
        fprintf(stderr, "setCameraPose() failed: %s\n", curl_easy_strerror(res));
        return false;
    }

    return true;
}

bool unityInterface::getCameraPose(vpPoseVector &p) {

    res = getData(curl, chunk, url + ":" + std::to_string(port) + "/" + posePath);

    if (res != CURLE_OK) {
        fprintf(stderr, "getCameraPose() failed: %s\n", curl_easy_strerror(res));
        return false;
    }
    else {
        std::string s(std::string(chunk.memory));
        for (int i = 0; i < 5; i++) {
            size_t first = s.find(":") + 1;
            size_t last = s.find(",");
            std::string x = s.substr(first, last - first);
            s[first - 1] = '#';
            s[last] = '#';
            p[i] = std::stof(x);
        }
        size_t first = s.find(":") + 1;
        size_t last = s.find("}");
        std::string x = s.substr(first, last - first);
        p[5] = std::stof(x);
    }

    return true;
}


bool unityInterface::setCameraParameters(CameraParameters cam) {
    //Format :  {"width":992,"height":649,"au":340.9122,"av":339.228546,"u0":352.017731,"v0":351.341553,"xi":1.083933}
    std::string json = "{\"width\":"+std::to_string(cam.width)+
                       ",\"height\":"+std::to_string(cam.height)+
                       ",\"au\":"+std::to_string(cam.au)+
                       ",\"av\":"+std::to_string(cam.av)+
                       ",\"u0\":"+std::to_string(cam.u0)+
                       ",\"v0\":"+std::to_string(cam.v0)+"}";

    res = postJson(curl, json, url + ":" + std::to_string(port) + "/" + configPath);

    if (res != CURLE_OK) {
        fprintf(stderr, "setCameraPose() failed: %s\n", curl_easy_strerror(res));
        return false;
    }

    return true;
}

bool unityInterface::getCameraParameters(CameraParameters &cam) {
    res = getData(curl, chunk, url + ":" + std::to_string(port) + "/" + configPath);

    if (res != CURLE_OK) {
        fprintf(stderr, "getCameraParameters() failed: %s\n", curl_easy_strerror(res));
        return false;
    }
    else {
        std::string s(std::string(chunk.memory));

        //TODO: string parsing
    }

    return true;
}




//**************************************************************
//**************** CURL related functions **********************
//**************************************************************

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

CURLcode postJson(CURL *curl, std::string json, std::string path){
    curl_easy_reset(curl);
    struct curl_slist *slist;
    slist = NULL;
    slist = curl_slist_append(slist, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    return(curl_easy_perform(curl));
}

CURLcode getData(CURL *curl, MemoryStruct &chunk, std::string path){
    curl_easy_reset(curl);
    chunk.size = 0;
    chunk.memory = (char*)realloc(chunk.memory, chunk.size);
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void * )&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    return(curl_easy_perform(curl));
}


