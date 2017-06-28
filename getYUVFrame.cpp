#define __STDC_CONSTANT_MACROS
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <fstream>
// Opencv
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>

extern "C" {
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
//新版里的图像转换结构需要引入的头文件
#include "libswscale/swscale.h"
};

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc  avcodec_alloc_frame
#endif


using namespace cv;
using namespace std;

#define INBUF_SIZE 65536

const char *filename, *outfilename;
const AVCodec *codec;
AVCodecContext *c= NULL;
int frame_count;
FILE *f;
AVFrame *frame;
uint8_t inbuf[INBUF_SIZE];
AVPacket avpkt;
AVFrame * pFrameBGR;

int BGRsize;
uint8_t *out_buffer = nullptr;

struct SwsContext *img_convert_ctx;
cv::Mat pCvMat ;

//int H264_Init() {
//    /* must be called before using avcodec lib*/
//    //avcodec_init();
//    /* register all the codecs */
//    avcodec_register_all();
//
//    /* find the h264 video decoder */
//    pCodec = avcodec_find_decoder(CODEC_ID_H264);
//    if (!pCodec) {
//        fprintf(stderr, "codec not found\n");
//    }
//    pCodecCtx = avcodec_alloc_context3(pCodec);
//
//    /* open the coderc */
//    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0) {
//        fprintf(stderr, "could not open codec\n");
//    }
//    // Allocate video frame
//    pFrame = avcodec_alloc_frame();
//    if (pFrame == NULL)
//        return -1;
//    // Allocate an AVFrame structure
//    pFrameBGR = avcodec_alloc_frame();
//    if (pFrameBGR == NULL)
//        return -1;
//
//    int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,
//                                  pCodecCtx->height);
//    uint8_t *out_buffer = (uint8_t *) av_malloc(size);
//    avpicture_fill((AVPicture *) pFrameBGR, out_buffer, AV_PIX_FMT_BGR24,
//                   pCodecCtx->width, pCodecCtx->height);
//    packet = (AVPacket *) malloc(sizeof(AVPacket));
//
//    return 0;
//
//}
//
//int H264_2_RGB(unsigned char *inputbuf) {
//
//    int av_result;
//
//    printf("Video decoding\n");
//
//    struct SwsContext *img_convert_ctx;
//    img_convert_ctx =
//            sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                           pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,
//                           NULL);
//
////opencv
//    cv::Mat pCvMat;
//    pCvMat.create(cv::Size(pCodecCtx->width, pCodecCtx->height), CV_8UC3);
//
//    int got_picture;
//
//    cvNamedWindow("RGB", 1);
//
//
////    av_result = avcodec_decode_video(pCodecCtx, pFrame, &decode_size, inputbuf, frame_size);
//    av_result = avcodec_decode_video2(pCodecCtx,pFrame,&got_picture,packet);
////    if (av_result < 0) {
////        printf("decode failed: inputbuf = 0x%x , input_framesize = %d\n", inputbuf, frame_size);
////        return -1;
////    }
//
//    if(got_picture){
//        sws_scale(img_convert_ctx, (const uint8_t *const *)pFrame->data,
//                  pFrame->linesize, 0, pCodecCtx->height, pFrameBGR->data, pFrameBGR->linesize);
//
//        memcpy(pCvMat.data, out_buffer, size);
//
//        imshow("RGB", pCvMat);
//        waitKey(33);
//    }
//    av_free_packet(packet);
//    return 0;
//}
//
//void H264_Release(void) {
//    avcodec_close(pCodecCtx);
//    av_free(pCodecCtx);
//    av_free(pFrame);
//    av_free(pFrameBGR);
//    sws_freeContext(img_convert_ctx);
//    cvDestroyWindow("RGB");
//}


void my_init() {

    avcodec_register_all();
    av_init_packet(&avpkt);

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;

    //存储解码后转换的RGB数据
    pFrameBGR = av_frame_alloc();

    c->width = 1280;
    c->height = 720;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    c->time_base.num = 1;
    c->frame_number = 1;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->bit_rate = 2000000;
    c->time_base.den = 25;


//    // 保存BGR，opencv中是按BGR来保存的
//    BGRsize = avpicture_get_size(AV_PIX_FMT_BGR24, c->width,
//                                  c->height);
//    out_buffer = (uint8_t *) av_malloc(BGRsize);
//    avpicture_fill((AVPicture *) pFrameBGR, out_buffer, AV_PIX_FMT_BGR24,
//                   c->width, c->height);
//
//    img_convert_ctx =
//            sws_getContext(c->width, c->height, c->pix_fmt,
//                           c->width, c->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,
//                           NULL);
//
//    pCvMat.create(cv::Size(c->width, c->height), CV_8UC3);



}

void my_264_2_mat(unsigned char *inputbuf, size_t size){
    avpkt.size = size;
    if(avpkt.size == 0)
        return;

    avpkt.data = inputbuf;

    int len, got_frame;
    char buf[1024];

    len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);

    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", frame_count);
        return ;
    }
    if(out_buffer == nullptr){
        BGRsize = avpicture_get_size(AV_PIX_FMT_BGR24, c->width,
                                     c->height);
        out_buffer = (uint8_t *) av_malloc(BGRsize);
        avpicture_fill((AVPicture *) pFrameBGR, out_buffer, AV_PIX_FMT_BGR24,
                       c->width, c->height);

        img_convert_ctx =
                sws_getContext(c->width, c->height, c->pix_fmt,
                               c->width, c->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,
                               NULL);
        pCvMat.create(cv::Size(c->width, c->height), CV_8UC3);

    }
    if (got_frame) {
        sws_scale(img_convert_ctx, (const uint8_t *const *)frame->data,
                  frame->linesize, 0, c->height, pFrameBGR->data, pFrameBGR->linesize);

        memcpy(pCvMat.data, out_buffer, BGRsize);

        cv::imshow("RGB", pCvMat);
        waitKey(33);

        frame_count++;
    }
    if (avpkt.data) {
        avpkt.size -= len;
        avpkt.data += len;
    }


}

int main(){
    my_init();
    unsigned char buf[690030];
    for(int j = 1;j<70;j++){
        std::ifstream  fin("/home/curi/hiro/ibotn/server/ws_streaming_server_java/264rawFrame/outputFrame"
                           +std::to_string(j),std::ios_base::binary);
        fin.seekg(0,std::ios::end);
        int len = fin.tellg();
        fin.seekg(0,std::ios::beg);

        fin>>buf;
        my_264_2_mat(buf,len);

    }



}
