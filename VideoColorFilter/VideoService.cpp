#include "stdafx.h"
#include "VideoService.h"
extern "C" {
#include "libavutil/imgutils.h"
#include "libyuv.h"
}
#include "flyyuv.h"

//#define PLAY_URL "rtmp://192.168.8.244/live/screen"
//#define PLAY_URL "d:\\video\\transcode01.mp4"


VideoService::VideoService(void)
{
	isStop = true;
	isRunning = false;
	memset(backYuv,0x3F,1280*720);
	memset(backYuv+1280*720,0x80,1280*720/2);
	TRACE("VideoService\n");	
}


VideoService::~VideoService(void)
{
	TRACE("~VideoService\n");
}


void VideoService::start(SDLWindow *mSDLWindow)
{
	this->mSDLWindow = mSDLWindow;
	isStop = false;	
	pid_ffplay = CreateThread(NULL, 0, &VideoService::ffplayThread, this, CREATE_SUSPENDED, NULL);
	if (NULL!= pid_ffplay) {  
		ResumeThread(pid_ffplay);  
	}
}


DWORD CALLBACK VideoService::ffplayThread(LPVOID lp)
{
	TRACE("VideoService ffplayThread start \n");	
	VideoService *mPtr = (VideoService *)lp;	
	isRunning = true;
	while(isStop==false)
	{
	    TRACE("loop read next frame.\n");
		mPtr->ffplay();
		Sleep(40);		
	}
	isRunning = false;
	TRACE("VideoService ffplayThread exit \n");	
	return 0;
}

int VideoService:: interrupt_cb(void *ctx)
{
	if(isStop){
		TRACE("VideoService interrupt_cb \n");	
		return 1;
	}
    return 0;
}

DWORD VideoService::ffplay()
{
	TRACE("VideoService ffplay thread start\n");
	u_char *sps;
	u_char *pps;
	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx_video;
	AVCodecContext *pCodecCtx_audio;
	AVPacket *packet;
	AVFrame *frame;
	AVFrame *fly_frame;
	struct SwrContext* swr_cxt;
	u_char *audio_buf;

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();	
	pFormatCtx->interrupt_callback.callback = interrupt_cb;
	pFormatCtx->interrupt_callback.opaque = pFormatCtx;
	AVDictionary* avdic = NULL;
	av_dict_set(&avdic, "probesize", "32", 0);
	av_dict_set(&avdic, "max_analyze_duration", "100000", 0);
	int ret = avformat_open_input(&pFormatCtx, playUrl, nullptr, &avdic);	
	av_dict_free(&avdic);
	if (ret != 0) {
		TRACE("VideoService Couldn't open url=%s, (ret:%d)\n", playUrl, ret);
		return -1;
	}
	int totalSec = static_cast<int>(pFormatCtx->duration / AV_TIME_BASE);
	TRACE("VideoService video time  %dmin:%dsec\n", totalSec / 60, totalSec % 60);

	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
		TRACE("VideoService Could't find stream infomation\n");
		return -1;
	}

	int videoStream = -1;
	int audioStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1) {
		TRACE("VideoService no find vedio_stream\n");
		return -1;
	}	

	AVCodecParameters *pCodecPar_video = pFormatCtx->streams[videoStream]->codecpar;
	AVCodec *pCodec_video = avcodec_find_decoder(pCodecPar_video->codec_id);
	if (pCodec_video == nullptr) {
		TRACE("VideoService not found decodec.\n");
		return -1;
	}
	pCodecCtx_video = avcodec_alloc_context3(pCodec_video);
	ret = avcodec_parameters_to_context(pCodecCtx_video, pCodecPar_video);
	if (ret < 0) {
		TRACE("VideoService avcodec_parameters_to_context() failed %d\n", ret);
		return -1;
	}
	if (avcodec_open2(pCodecCtx_video, pCodec_video, nullptr) < 0) {
		TRACE("VideoService Could not open decodec.\n");
		return -1;
	}

	int vfps = (int) ((double) (pFormatCtx->streams[videoStream]->avg_frame_rate.num) /(double) (pFormatCtx->streams[videoStream]->avg_frame_rate.den));
	int width = pCodecCtx_video->width;
	int height = pCodecCtx_video->height;
	TRACE("VideoService fps = %d\n",vfps);

	mSDLWindow->init(720,1280,vfps);	
	pCodecCtx_video->flags |=CODEC_FLAG_LOW_DELAY;

	////��Ƶʹ����⣬��ʼ����Ƶ���룬��Ƶ���Ǳ������
	//for (int i = 0; i < pFormatCtx->nb_streams; i++) {
	//	if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
	//		audioStream = i;
	//		AVCodecParameters *pCodecPar_audio = pFormatCtx->streams[i]->codecpar;
	//		AVCodec *pCodec_audio = avcodec_find_decoder(pCodecPar_audio->codec_id);
	//		if (pCodec_audio != nullptr) {
	//			pCodecCtx_audio = avcodec_alloc_context3(pCodec_audio);
	//			ret = avcodec_parameters_to_context(pCodecCtx_audio, pCodecPar_audio);
	//			if (ret >= 0) {
	//				if (avcodec_open2(pCodecCtx_audio, pCodec_audio, nullptr) >= 0) {
	//					TRACE("VideoService find audioStream = %d, sampleRateInHz = %d, channelConfig=%d, audioFormat=%d\n", i, pCodecCtx_audio->sample_rate, pCodecCtx_audio->channels,						pCodecCtx_audio->sample_fmt);
	//					swr_cxt = swr_alloc();
	//					swr_alloc_set_opts(
	//						swr_cxt,
	//						out_channelConfig,						
	//						(AVSampleFormat) out_audioFormat,
	//						out_sampleRateInHz,
	//						pCodecCtx_audio->channel_layout,
	//						pCodecCtx_audio->sample_fmt,
	//						pCodecCtx_audio->sample_rate,
	//						0,
	//						nullptr);
	//					swr_init(swr_cxt);
	//					audio_buf = (uint8_t *) av_malloc(out_sampleRateInHz * 8);
	//					//callBack->javaOnAudioInfo(out_sampleRateInHz, out_channelConfig, out_audioFormat);
	//				} else {
	//					avcodec_close(pCodecCtx_audio);
	//					TRACE("VideoService init audio codec failed 2!\n");
	//				}
	//			} else {
	//				TRACE("VideoService init audio codec failed 3!\n");
	//			}
	//		} else {
	//			TRACE("VideoService init audio codec failed 1!\n");
	//		}
	//
	//		break;
	//	}
	//}

	frame = av_frame_alloc();	
	packet = (AVPacket *) av_malloc(sizeof(AVPacket)); //����һ��packet
	TRACE("xxxxxxx 1\n");
	int buf_size;
	uint8_t *buffer;
	static struct SwsContext *sws_ctx;
	fly_frame = av_frame_alloc();
	buf_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,width,height,1);
	buffer = (uint8_t *)av_malloc(buf_size);
	av_image_fill_arrays(
		fly_frame->data,           // dst data[]
		fly_frame->linesize,       // dst linesize[]
		buffer,                    // src buffer
		AV_PIX_FMT_YUV420P,        // pixel format
		pCodecCtx_video->width,        // width
		pCodecCtx_video->height,       // height
		1                          // align
		);
	TRACE("xxxxxxx 2\n");
	sws_ctx = sws_getContext(
		width,    // src width
		height,   // src height
		pCodecCtx_video->pix_fmt,  // src format
		720,    // dst width
		1280,   // dst height
		AV_PIX_FMT_YUV420P,    // dst format
		SWS_BICUBIC,           // flags
		NULL,                  // src filter
		NULL,                  // dst filter
		NULL                   // param
		);          

	TRACE("xxxxxxx 3\n");
	while (!isStop ) {	
		ret = av_read_frame(pFormatCtx, packet);
		if(ret<0){
			TRACE("VideoService av_read_frame read error, ret=%d\n",ret);
			break;		
		}
		if (packet->stream_index == videoStream) {
			//�����Ƶ
			//TODO::ʱ��ͬ��
			ret = avcodec_send_packet(pCodecCtx_video, packet);
			while (ret >= 0) {
				ret = avcodec_receive_frame(pCodecCtx_video, frame);
				if (ret >= 0) {
					sws_scale(sws_ctx,                                  // sws context
						(const uint8_t *const *)frame->data,  // src slice
						frame->linesize,                      // src stride
						0,                                        // src slice y
						pCodecCtx_video->height,                      // src slice height
						fly_frame->data,                          // dst planes
						fly_frame->linesize                       // dst strides
						);
					fly_frame->width = frame->width;
					fly_frame->height = frame->height;
					auto * ok_buf = (u_char *) malloc((1280 * 720* 3 / 2) * sizeof(u_char));
					auto * video_buf = (u_char *) malloc((width*height* 3 / 2) * sizeof(u_char));
					int start = 0;
					memcpy(video_buf,fly_frame->data[0],width*height);
					start=start+width*height;
					memcpy(video_buf + start,fly_frame->data[1],width*height/4);
					start=start+width*height/4;
					memcpy(video_buf + start,fly_frame->data[2],width*height/4);					
					memcpy(ok_buf,video_buf,width*height* 3 / 2);
					FlyYuv::I420TO720P(ok_buf,width,height);
					FlyYuv::I420Filter(ok_buf,backYuv,width,height,mapFilter);
					mSDLWindow->pushYUV(ok_buf);		
					free(video_buf);
				}
			}
		} else if (packet->stream_index == audioStream) {
			//ret = avcodec_send_packet(pCodecCtx_audio, packet);
			//while (ret >= 0) {
			//	ret = avcodec_receive_frame(pCodecCtx_audio, frame);
			//	if (ret >= 0) {
			//		int len = swr_convert(
			//			swr_cxt,
			//			&audio_buf,
			//			frame->nb_samples,
			//			(const uint8_t **) frame->data,
			//			frame->nb_samples);
			//		int out_buf_size = av_samples_get_buffer_size(
			//			NULL,
			//			av_get_channel_layout_nb_channels(out_channelConfig),
			//			frame->nb_samples,
			//			(AVSampleFormat) out_audioFormat,
			//			0);
			//		int size = out_buf_size * out_sampleRateInHz / frame->sample_rate;
			//	}
			//}
		}
		av_packet_unref(packet);
	}

	//	av_free(audio_buf);	
	av_free(packet);
	av_frame_free(&frame);
	avcodec_close(pCodecCtx_video);
	//	avcodec_close(pCodecCtx_audio);
	avformat_close_input(&pFormatCtx);	
	TRACE("VideoService ffplay thread exit\n");
	return 0;
}

void VideoService::stop()
{
	isStop = true;
	Sleep(1);
	while (isRunning){
		TRACE("Can't stop VideoService, because is Running...\n");
		Sleep(1000);
	}
}

void VideoService::setMapFilter(u_char *yuvFilter)
{
	memset(mapFilter,0,768);
    for (int i = 0; i <= yuvFilter[1]; i++) {
        mapFilter[(yuvFilter[0] + i) & 0x00FF] = 1;
        mapFilter[(yuvFilter[0] - i) & 0x00FF] = 1;
    }
    for (int i = 0; i <= yuvFilter[3]; i++) {
        mapFilter[256 + ((yuvFilter[2] + i) & 0x00FF)] = 1;
        mapFilter[256 + ((yuvFilter[2] - i) & 0x00FF)] = 1;
    }
    for (int i = 0; i <= yuvFilter[5]; i++) {
        mapFilter[512 + ((yuvFilter[4] + i) & 0x00FF)] = 1;
        mapFilter[512 + ((yuvFilter[4] - i) & 0x00FF)] = 1;
    }
}

void VideoService::setPlayUrl(char *url)
{
	if(url==nullptr||sizeof(url)==0){
		sprintf_s(playUrl,"%s","d:\\video\\test1080.mp4");
	}else{
		sprintf_s(playUrl,"%s",url);
	}
}



