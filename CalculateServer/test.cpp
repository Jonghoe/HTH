#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable:4996)
///> Include FFMpeg

#include"opencv2/opencv.hpp"

extern "C" {

	#include <libavcodec/avcodec.h>
	#include<libswscale/swscale.h>
	#include<libavformat/avio.h>
	#include<libavformat/avformat.h>
	#include<libavcodec/avcodec.h>
}
void readYUV(int frame_height, int frame_width, unsigned char* buffer, cv::Mat& out);
void avframeToMat(const AVFrame * frame, cv::Mat& image)
{
	int width = frame->width;
	int height = frame->height;
	
	// Allocate the opencv mat and store its stride in a 1-element array
	if (image.rows != height || image.cols != width || image.type() != CV_8UC3) 
		image = cv::Mat(height, width, CV_8UC3);	
	cv::cvtColor(image, image, CV_YUV2BGR);
	int cvLinesizes[1];
	cvLinesizes[0] = image.step1();
	// Convert the colour format and write directly to the opencv matrix
	SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat)frame->format, width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
	sws_freeContext(conversion);
}

int main(void)
{
	const char *szFilePath = "C:\\Users\\rnwhd\\Downloads\\serenity_hd_dvd-trailer\\test.mp4";

	av_register_all();
	cv::Mat img;
	int ret;
	AVFormatContext *pFmtCtx = NULL;

	///> Open an input stream and read the header. 
	ret = avformat_open_input(&pFmtCtx, szFilePath, NULL, NULL);
	
	if (ret != 0) {
		av_log(NULL, AV_LOG_ERROR, "File [%s] Open Fail (ret: %d)\n", ret);
		exit(-1);
	}
	av_log(NULL, AV_LOG_INFO, "File [%s] Open Success\n", szFilePath);
	av_log(NULL, AV_LOG_INFO, "Format: %s\n", pFmtCtx->iformat->name);

	///> Read packets of a media file to get stream information. 
	ret = avformat_find_stream_info(pFmtCtx, NULL);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Fail to get Stream Information\n");
		exit(-1);
	}
	av_log(NULL, AV_LOG_INFO, "Get Stream Information Success\n");

	///> Find Video Stream
	int nVSI = -1;
	int nASI = -1;
	int i;
	for (i = 0; i < pFmtCtx->nb_streams; i++) {
		if (nVSI < 0 && pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			nVSI = i;
		}
		else if (nASI < 0 && pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			nASI = i;
		}
	}

	if (nVSI < 0 && nASI < 0) {
		av_log(NULL, AV_LOG_ERROR, "No Video & Audio Streams were Found\n");
		exit(-1);
	}

	///> Find Video Decoder
	AVCodec *pVideoCodec = avcodec_find_decoder(pFmtCtx->streams[nVSI]->codec->codec_id);
	if (pVideoCodec == NULL) {
		av_log(NULL, AV_LOG_ERROR, "No Video Decoder was Found\n");
		exit(-1);
	}

	///> Initialize Codec Context as Decoder
	if (avcodec_open2(pFmtCtx->streams[nVSI]->codec, pVideoCodec, NULL) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Fail to Initialize Decoder\n");
		exit(-1);
	}

	AVCodecContext *pVCtx = pFmtCtx->streams[nVSI]->codec;

	AVPacket pkt;
	AVFrame* pVFrame;

	int bGotPicture = 0;	// flag for video decoding

	pVFrame = av_frame_alloc();


	while (av_read_frame(pFmtCtx, &pkt) >= 0) {
		///> Decoding
		if (pkt.stream_index == nVSI) {
			if (avcodec_decode_video2(pVCtx, pVFrame, &bGotPicture, &pkt) >= 0) {
				if (bGotPicture) {
					//write_I420_frame(szSaveDecodedVideoFilePath, pVFrame);
					cv::Mat img;
					avframeToMat(pVFrame, img);
					imshow("img", img);
					cv::waitKey(10);
				}
			}
			// else ( < 0 ) : Decoding Error
		}
		///> Free the packet that was allocated by av_read_frame
		av_free_packet(&pkt);
	}

	av_free(pVFrame);

	///> Close an opened input AVFormatContext. 
	avformat_close_input(&pFmtCtx);

	return 0;
}
