#include"ImageCommunicator.h"
#include "VideoDecoder.h"


int  parse(uint8_t* pBuff, int videosize, uint64_t pts, AVCodecParserContext* m_pCodecPaser, AVCodec* m_pAVCodec, AVCodecContext* m_pCodecCtx, AVFrame* m_Frame)
{
	ImageCommunicator* imgCommunicator = ImageCommunicator::GetInstance();
	int paserLength_In = videosize;
	int paserLen;
	int decode_data_length;
	int got_picture = 0;
	uint8_t *pFrameBuff = (uint8_t*)pBuff;
	while (paserLength_In > 0)
	{
		AVPacket packet;
		av_init_packet(&packet);

		paserLen = av_parser_parse2(m_pCodecPaser, m_pCodecCtx, &packet.data, &packet.size, pFrameBuff,
			paserLength_In, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

		//LOGD("paserLen = %d",paserLen);
		paserLength_In -= paserLen;
		pFrameBuff += paserLen;

		if (packet.size > 0)
		{
			int got_frame;
			int len = avcodec_decode_video2(m_pCodecCtx, m_Frame, &got_frame, &packet);
			if (len >= 0 && got_frame)
			{
				cv::Mat img;
				VideoDecoder::avframeToMat(m_Frame, img);
				imshow("img", img);
				cv::waitKey(1);
				ImageCommunicator* imgCmr = ImageCommunicator::GetInstance();
				imgCmr->sender.send(img);
			}
		}
		av_free_packet(&packet);
	}

	return 0;
}

bool parse(uint8_t* buffer, int size, AVCodecParserContext* m_pCodecPaser, AVCodec* m_pAVCodec, AVCodecContext* m_pCodecCtx, AVFrame* m_Frame)
{
	int retV = 0;
	static uint8_t audbuffer2[] = { 0x00,0x00,0x00,0x01,0x09,0x10 };
	static uint8_t audsize2 = 6;
	static uint8_t fillerbuffer2[] = { 0x00,0x00,0x00,0x01,0x0C,0x00,0x00,0x00,0x01,0x09,0x10 };
	static uint8_t fillersize2 = 11;
	static uint8_t audaudbuffer2[] = { 0x00,0x00,0x00,0x01,0x09,0x10, 0x00,0x00,0x00,0x01,0x09,0x10 };
	static uint8_t audaudsize2 = 12;

	uint8_t* buff = buffer;
	uint64_t pts = 0;

	// Removing the aud bytes.
	if (size >= fillersize2 && memcmp(fillerbuffer2, buff + size - fillersize2, fillersize2) == 0)
	{
		parse(buff, size - fillersize2, pts, m_pCodecPaser, m_pAVCodec, m_pCodecCtx, m_Frame);
	}
	else if (size >= audaudsize2 && memcmp(audaudbuffer2, buff + size - audaudsize2, audaudsize2) == 0)
	{
		parse(buff, size - audaudsize2, pts, m_pCodecPaser, m_pAVCodec, m_pCodecCtx, m_Frame);
	}
	else if (size >= audsize2 && memcmp(audbuffer2, buff + size - audsize2, audsize2) == 0)
	{
		parse(buff, size - audsize2, pts, m_pCodecPaser, m_pAVCodec, m_pCodecCtx, m_Frame);
	}
	else
	{
		parse(buff, size, pts, m_pCodecPaser, m_pAVCodec, m_pCodecCtx, m_Frame);
	}
	return retV;
}

#define INBUF_SZIE 32768
ImageCommunicator* ImageCommunicator::m_pInstance = nullptr;

void IMGCommunicatorMain(SOCKET client)
{
	ImageCommunicator* imgCommunicator = ImageCommunicator::GetInstance();
	/*VideoDecoder* decoder = VideoDecoder::GetInstance();
	if (decoder->ready())
		std::cout << "#############################HELLLO#####################" << std::endl;
	else
		exit(-1);*/
	uint8_t buffer[INBUF_SZIE+ AV_INPUT_BUFFER_PADDING_SIZE];
	char* lenBuffer[4];
	memset(buffer, 0, INBUF_SZIE+ AV_INPUT_BUFFER_PADDING_SIZE);
	int len = 0;
	int recvLen = 0;
#pragma region FFMPEG_READY
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
	AVCodecParserContext* m_pCodecPaser = av_parser_init(AV_CODEC_ID_H264);
	AVCodec* gVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!gVideoCodec)
	{
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}
	AVCodecContext *gContext = avcodec_alloc_context3(gVideoCodec);
	if (!gContext)
	{
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	avcodec_get_context_defaults3(gContext, gVideoCodec);
	gContext->thread_count = 4;
	gContext->thread_type = FF_THREAD_FRAME;

	if (avcodec_open2(gContext, gVideoCodec, nullptr) < 0)
	{
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}

	AVFrame* pVFrame = av_frame_alloc();

	if (!pVFrame)
	{
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}

	std::cout << "ffmpeg ready" << std::endl;
#pragma endregion
	while (true) {
		recvLen = 0;

#pragma region RECV_LENGTH
		while (recvLen < 4) {
			recvLen += recv(client, (char*)lenBuffer + recvLen, 4 - recvLen, 0);
		}
		memcpy(&len, lenBuffer, 4);
#pragma endregion

#pragma region RECV_PACKET
		int input_len = 0;
		while (input_len < len) {
			recvLen = recv(client, (char*)buffer + input_len, len - input_len, 0);
			input_len += recvLen;
		}
#pragma endregion
		
		parse(buffer, len, m_pCodecPaser, gVideoCodec, gContext, pVFrame);

		//int retv = decoder->parse(buffer, len);
		memset(buffer, 0, len);
	}
}

bool ImageCommunicator::isReady()
{
	return true;
}
