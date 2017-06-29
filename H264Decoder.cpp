//
// Created by curi on 17-6-28.
//

#include "H264Decoder.h"


void H264Decoder::init() {

    matReady = false;

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


}

void H264Decoder::decode(unsigned char *inputbuf, size_t size){

    avpkt.size = size;
    if(avpkt.size == 0)
        return;

    avpkt.data = inputbuf;

    int len, got_frame;


    len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);

    if (len < 0) {
        matReady = false;
        fprintf(stderr, "Error while decoding frame %d\n", frame_count);
        frame_count++;

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
        matReady = true;
        sws_scale(img_convert_ctx, (const uint8_t *const *)frame->data,
                  frame->linesize, 0, c->height, pFrameBGR->data, pFrameBGR->linesize);

        memcpy(pCvMat.data, out_buffer, BGRsize);

//        printf("decoding frame: %d\n",frame_count);
        frame_count++;
    }
    else{
        matReady = false;
    }
    if (avpkt.data) {
        avpkt.size -= len;
        avpkt.data += len;
    }




}

void H264Decoder::play() {
    if(matReady){
        cv::imshow("decode",pCvMat);
        cv::waitKey(1);
    }
}

H264Decoder::H264Decoder() {
    init();
}

cv::Mat H264Decoder::getMat() {
    if(matReady){
        return pCvMat;
    }
    else{
        return cv::Mat();
    }
}



