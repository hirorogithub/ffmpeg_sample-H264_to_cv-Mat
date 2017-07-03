#include "H264Decoder.h"
#include <fstream>

unsigned char buf[700000];//should be max enough

int main() {

    H264Decoder decoder;
    for (int j = 1; j < 197; j++) {
        std::ifstream fin("/home/curi/hiro/ibotn/server/ws_streaming_server_java/264rawFrame/outputFrame"
                          + std::to_string(j), std::ios_base::binary);
        fin.seekg(0, std::ios::end);
        int len = fin.tellg();
        fin.seekg(0, std::ios::beg);

        fin.read((char *) buf, len);
        decoder.decode(buf, len);
        decoder.play();
    }
}

