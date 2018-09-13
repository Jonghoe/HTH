#include"VideoDecoder.h"

#define INBUF_SIZE 4096

static AVPacket *gPkt;
static const AVCodec *gCodec;
static AVCodecParserContext *gParser;
static AVCodecContext *gContext = NULL;
static AVFormatContext *gFContext = NULL;
static AVFrame *gFrame;
static AVCodec *gVideoCodec;
static uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];

static int nVSI = -1;
static int nASI = -1;

bool VideoDecoder::ready(const char* ip)
{
	av_register_all();
	int ret = avformat_open_input(&gFContext, ip, NULL, NULL);

	if (ret != 0) {
		av_log(NULL, AV_LOG_ERROR, "File [%s] Open Fail (ret: %d)\n", ret);
		return false;
	}
	ret = avformat_find_stream_info(gFContext, NULL);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Fail to get Stream Information\n");
		exit(-1);
	}
	int i;
	for (i = 0; i < gFContext->nb_streams; i++) {
		if (nVSI < 0 && gFContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			nVSI = i;
		}
		else if (nASI < 0 && gFContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			nASI = i;
		}
	}

	if (nVSI < 0 && nASI < 0) {
		av_log(NULL, AV_LOG_ERROR, "No Video & Audio Streams were Found\n");
		return false;
	}

	gVideoCodec = avcodec_find_decoder(gFContext->streams[nVSI]->codec->codec_id);
	if (gVideoCodec == NULL) {
		av_log(NULL, AV_LOG_ERROR, "No Video Decoder was Found\n");
		return false;
	}

	///> Initialize Codec Context as Decoder
	if (avcodec_open2(gFContext->streams[nVSI]->codec, gVideoCodec, NULL) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Fail to Initialize Decoder\n");
		return false;
	}

	return true;
}

void VideoDecoder::Decode(ImageProcess* processor)
{
	AVCodecContext *pVCtx = gFContext->streams[nVSI]->codec;

	AVPacket pkt;
	AVFrame* pVFrame;
	int bGotPicture = 0;	// flag for video decoding

	pVFrame = av_frame_alloc();


	while (av_read_frame(gFContext, &pkt) >= 0) {
		///> Decoding
		if (pkt.stream_index == nVSI) {
			if (avcodec_decode_video2(pVCtx, pVFrame, &bGotPicture, &pkt) >= 0) {
				if (bGotPicture) {
					//write_I420_frame(szSaveDecodedVideoFilePath, pVFrame);
					cv::Mat out;
					avframeToMat(pVFrame, out);
					processor->process(out);
					cv::waitKey(1);
				}
			}
			// else ( < 0 ) : Decoding Error
		}
		///> Free the packet that was allocated by av_read_frame
		av_free_packet(&pkt);
	}

	av_free(pVFrame);
	///> Close an opened input AVFormatContext. 
	avformat_close_input(&gFContext);
}


void VideoDecoder::avframeToMat(const AVFrame * frame, cv::Mat& image)
{
	int width = frame->width;
	int height = frame->height;

	// Allocate the opencv mat and store its stride in a 1-element array
	if (image.rows != height || image.cols != width || image.type() != CV_8UC3) image = cv::Mat(height, width, CV_8UC3);	int cvLinesizes[1];
	cvLinesizes[0] = image.step1();
	// Convert the colour format and write directly to the opencv matrix
	SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat)frame->format, width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
	sws_freeContext(conversion);
}