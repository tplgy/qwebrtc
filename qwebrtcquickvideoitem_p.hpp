#pragma once

#include "qwebrtcquickvideoitem.hpp"
#include <webrtc/common_video/video_render_frames.h>
#include <webrtc/media/base/videosinkinterface.h>
#include "qwebrtcmediatrack_p.hpp"
#include <mutex>

class QWebRTCQuickVideoItem_p : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    explicit QWebRTCQuickVideoItem_p(QWebRTCQuickVideoItem*);
    virtual void OnFrame(const webrtc::VideoFrame& frame) override;

    std::shared_ptr<webrtc::VideoFrame> m_frame;
    QWebRTCMediaTrack_impl* m_track;
    QWebRTCQuickVideoItem *q_ptr;
    std::recursive_mutex videoMutex;
    QSize m_sourceSize;
};
