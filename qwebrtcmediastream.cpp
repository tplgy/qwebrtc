#include "qwebrtcmediastream_p.hpp"
#include "qwebrtcmediatrack_p.hpp"

QWebRTCMediaStream_impl::QWebRTCMediaStream_impl(const rtc::scoped_refptr<webrtc::MediaStreamInterface>& stream)
    : m_nativeStream(stream)
{
    for (auto videoTrack : stream->GetVideoTracks()) {
        m_tracks.append(std::make_shared<QWebRTCMediaTrack_impl>(videoTrack, nullptr));
    }
    for (auto audioTrack : stream->GetAudioTracks()) {
        m_tracks.append(std::make_shared<QWebRTCMediaTrack_impl>(audioTrack));
    }
}

void QWebRTCMediaStream_impl::addTrack(const std::shared_ptr<QWebRTCMediaTrack>& track)
{
    auto trackImpl = std::static_pointer_cast<QWebRTCMediaTrack_impl>(track);
    if (track->trackType() == QWebRTCMediaTrack::Type::Audio) {
        m_nativeStream->AddTrack(trackImpl->audioTrack);
    } else {
        m_nativeStream->AddTrack(trackImpl->videoTrack);
    }
    m_tracks.append(track);
}

void QWebRTCMediaStream_impl::removeTrack(const std::shared_ptr<QWebRTCMediaTrack>& track)
{
    auto trackImpl = std::static_pointer_cast<QWebRTCMediaTrack_impl>(track);
    if (track->trackType() == QWebRTCMediaTrack::Type::Audio) {
        m_nativeStream->RemoveTrack(trackImpl->audioTrack);
    } else {
        m_nativeStream->RemoveTrack(trackImpl->videoTrack);
    }
    m_tracks.removeOne(track);
}

QList<std::shared_ptr<QWebRTCMediaTrack>> QWebRTCMediaStream_impl::tracks()
{
    return m_tracks;
}
