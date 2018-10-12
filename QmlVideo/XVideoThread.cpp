#include "XVideoThread.h"

#include <stdio.h>

#include <QDebug>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

//static double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) {

//    double frame_delay;

//    if (pts != 0) {
//        /* if we have pts, set video clock to it */
//        is->video_clock = pts;
//    } else {
//        /* if we aren't given a pts, set it to the clock */
//        pts = is->video_clock;
//    }
//    /* update the video clock */
//    frame_delay = av_q2d(is->video_st->codec->time_base);
//    qDebug() << "===frame_delay:" << frame_delay << src_frame->repeat_pict;
//    /* if we are repeating a frame, adjust clock accordingly */
//    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
//    is->video_clock += frame_delay;
//    return pts;
//}


VideoPlayer::VideoPlayer()
{
    mPlayerState = Pause;
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::startPlay()
{
    ///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
    if( mPlayerState == Pause || mPlayerState == Stop ) {
        mPlayerState = Playing;
        if( !this->isRunning() ) {
            this->start();
        }
        qDebug() << "Playing...";
    } else if( mPlayerState == Playing ) {
        mPlayerState = Stop;
        qDebug() << "Stop...";
    }
}

void VideoPlayer::pausePlay()
{
    mPlayerState = Pause;
    if( this->isRunning() ) {
        this->wait();
    }
    qDebug() << "Pause...";
}

void VideoPlayer::run()
{
    memset(&mVideoState,0,sizeof(VideoState));
    char *file_path = (char *)mFileName.toStdString().c_str();

    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

    av_register_all(); //初始化FFMPEG  调用了这个才能正常使用编码器和解码器

    //Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, mFileName.toStdString().c_str(), NULL, NULL) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Could't find stream infomation.\n");
        return;
    }

    videoStream = -1;

    ///循环查找视频中包含的流信息，
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }

    {
        qint64 uSec = pFormatCtx->duration;
        qint64 Sec = uSec/1000000;
        QString mStr = QString("00%1").arg(Sec/60);
        QString sStr = QString("00%1").arg(Sec%60);

        QString str = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
        emit sig_totalTimeChanged(str, Sec);
        qDebug() << "totalTime:" << str << uSec;
    }

    ///查找视频解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec == NULL) {
        printf("PCodec not found.\n");
        return;
    }

    ///打开视频解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open video codec.\n");
        return;
    }

    mVideoState.video_st = pFormatCtx->streams[videoStream];
    int frame = 1 / av_q2d(mVideoState.video_st->time_base);
    qDebug() << "time_base:" << 1 / av_q2d(mVideoState.video_st->time_base);

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
            pCodecCtx->width, pCodecCtx->height);

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    av_dump_format(pFormatCtx, 0, file_path, 0); //输出视频信息

    int64_t start_time = av_gettime();
    qDebug() << "start_time:" << start_time;
    int64_t pts = 0; //当前视频的pts
    bool b_hasStopped = false;
//    int one = 1;
    while (1)
    {
        if( mPlayerState == Pause ) {
            emit sig_GetOneFrame(QImage());  //发送信号
            break;
        }
        if( mPlayerState == Stop ) {
            b_hasStopped = true;
            msleep(10);
            continue;
        }

        //暂停重新开启，重置启动时间
        if( b_hasStopped ) {
            start_time = av_gettime() - pts;
            qDebug() << "b_hasStopped:" << pts;
        }

//        if( one == 1 ) {
//            av_seek_frame(pFormatCtx, videoStream,  AV_TIME_BASE, AVSEEK_FLAG_BYTE);
//            start_time -= AV_TIME_BASE;
//            one = 0;
//        }

        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }

        int64_t realTime = av_gettime() - start_time; //主时钟时间

        while(pts > realTime)
        {
            msleep(10);
            realTime = av_gettime() - start_time; //主时钟时间
            qDebug() << "pts:" << pts << "realTime:" << realTime;
        }

        if (packet->stream_index == videoStream)
        {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);
            if (packet->dts == AV_NOPTS_VALUE && pFrame->opaque&& *(uint64_t*) pFrame->opaque != AV_NOPTS_VALUE)
            {
                pts = *(uint64_t *) pFrame->opaque;
            }
            else if (packet->dts != AV_NOPTS_VALUE)
            {
                pts = packet->dts;
            }
            else
            {
                pts = 0;
            }
            qDebug() << "---pts:" << pts << 1000000 * av_q2d(mVideoState.video_st->time_base);

            pts *= 1000000 * av_q2d(mVideoState.video_st->time_base);

            {
                qint64 Sec = pts/1000000;
                QString mStr = QString("00%1").arg(Sec/60);
                QString sStr = QString("00%1").arg(Sec%60);

                QString str = QString("%1:%2").arg(mStr.right(2)).arg(sStr.right(2));
                emit sig_currentTimeChanged(str, Sec);
            }

            if (got_picture) {
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sig_GetOneFrame(image);  //发送信号
                b_hasStopped = false;
//                qDebug() << "pts:" << pts / 1000000 << "realTime:" << realTime / 1000000;
            }

            av_free_packet(packet);
        }
        else
        {
            // Free the packet that was allocated by av_read_frame
            av_free_packet(packet);
        }
    }

    emit sig_GetOneFrame(QImage());
    pausePlay();

    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
