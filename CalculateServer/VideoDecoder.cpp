#include"VideoDecoder.h"
VideoDecoder* VideoDecoder::m_pInstance = NULL;
VideoDecoder* VideoDecoder::GetInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new VideoDecoder();
	return m_pInstance;
}

bool VideoDecoder::ready()
{
	av_register_all();
	avcodec_register_all();
	avformat_network_init();

	m_pCodecPaser = av_parser_init(AV_CODEC_ID_H264);
	m_pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!m_pAVCodec)
	{
		fprintf(stderr, "Codec not found\n");
		return false;
	}
	m_pCodecCtx = avcodec_alloc_context3(m_pAVCodec);
	if (!m_pCodecCtx)
	{
		fprintf(stderr, "Could not allocate video codec context\n");
		return false;
	}
	avcodec_get_context_defaults3(m_pCodecCtx, m_pAVCodec);
	m_pCodecCtx->thread_count = 4;
	m_pCodecCtx->thread_type = FF_THREAD_FRAME;

	if (avcodec_open2(m_pCodecCtx, m_pAVCodec, nullptr) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		return false;
	}

	m_pFrame = av_frame_alloc();

	if (!m_pFrame)
	{
		fprintf(stderr, "Could not allocate video frame\n");
		return false;
	}

	std::cout << "ffmpeg ready" << std::endl;
	return true;
}


int VideoDecoder::parse(uint8_t * pBuff, int videosize, uint64_t pts)
{
	int paserLength_In = videosize;
	int paserLen;
	int got_picture = 0;
	uint8_t *pFrameBuff = (uint8_t*)pBuff;
	while (paserLength_In > 0)
	{
		AVPacket packet;
		av_init_packet(&packet);

		paserLen = av_parser_parse2(m_pCodecPaser, m_pCodecCtx, &packet.data, &packet.size, pFrameBuff,
			paserLength_In, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

		paserLength_In -= paserLen;
		pFrameBuff += paserLen;

		if (packet.size > 0)
		{
			int got_frame;
			int len = avcodec_decode_video2(m_pCodecCtx, m_pFrame, &got_frame, &packet);
			if (len >= 0 && got_frame)
			{
				cv::Mat img;
				VideoDecoder::avframeToMat(m_pFrame, img);				
				//if (retv == 1) {
					cv::imshow("sender", img);
					cv::waitKey(1);
				//}
				return 1;
			}
		}
		av_free_packet(&packet);
	}

	return 0;
}

int VideoDecoder::parse(uint8_t * buff, int size)
{
	uint8_t audbuffer2[] = { 0x00,0x00,0x00,0x01,0x09,0x10 };
	uint8_t audsize2 = 6;
	uint8_t fillerbuffer2[] = { 0x00,0x00,0x00,0x01,0x0C,0x00,0x00,0x00,0x01,0x09,0x10 };
	uint8_t fillersize2 = 11;
	uint8_t audaudbuffer2[] = { 0x00,0x00,0x00,0x01,0x09,0x10, 0x00,0x00,0x00,0x01,0x09,0x10 };
	uint8_t audaudsize2 = 12;
	int retv = 0;
	uint64_t pts = 0;

	// Removing the aud bytes.
	if (size >= fillersize2 && memcmp(fillerbuffer2, buff + size - fillersize2, fillersize2) == 0)
	{
		parse(buff, size - fillersize2, pts);
	}
	else if (size >= audaudsize2 && memcmp(audaudbuffer2, buff + size - audaudsize2, audaudsize2) == 0)
	{
		parse(buff, size - audaudsize2, pts);
	}
	else if (size >= audsize2 && memcmp(audbuffer2, buff + size - audsize2, audsize2) == 0)
	{
		parse(buff, size - audsize2, pts);
	}
	else
	{
		parse(buff, size, pts);
	}
	return retv;
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