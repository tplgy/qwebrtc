#include <memory>
#include "qwebrtcpeerconnection.hpp"
#include "qwebrtcpeerconnection_p.hpp"
#include "qwebrtcmediastream_p.hpp"
#include "qwebrtcdatachannel_p.hpp"
#include "qwebrtctypes_p.hpp"
#include "qwebrtcicecandidate.hpp"
#include <webrtc/api/peerconnectioninterface.h>
#include <webrtc/api/datachannelinterface.h>
#include <webrtc/api/mediaconstraintsinterface.h>
#include <webrtc/api/jsepicecandidate.h>
#include <QVariantMap>
#include <QDebug>
#include <assert.h>

#include "webrtc/modules/video_capture/video_capture_factory.h"

void QWebRTCCreateSessionDescriptionObserver_p::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    invokeHandler(QSharedPointer<QWebRTCSessionDescription_impl>(new QWebRTCSessionDescription_impl(desc)));
}

void QWebRTCCreateSessionDescriptionObserver_p::OnFailure(const std::string& error)
{
    qWarning() << "Could not create session description " << QByteArray::fromStdString(error);
    invokeHandler(QSharedPointer<QWebRTCSessionDescription>());
}

void QWebRTCCreateSessionDescriptionObserver_p::invokeHandler(const QSharedPointer<QWebRTCSessionDescription>& result)
{
//    auto sImpl = impl.lock();
    if (m_completionHandler) {
        m_completionHandler(result);
    }
//    if (sImpl) {
//        // remove adapter from observer list
//        for (auto it = sImpl->m_createObservers.begin(); it!= sImpl->m_createObservers.end(); ++it) {
//            if ((*it).get() == this) {
//                sImpl->m_createObservers.erase(it);
//                return;
//            }
//        }
//    }
}

void QWebRTCSetSessionDescriptionObserver_p::OnSuccess()
{
    qDebug() << "description set";
    invokeHandler(true);
}

void QWebRTCSetSessionDescriptionObserver_p::OnFailure(const std::string& error)
{
    qWarning() << "Could not set session description " << QByteArray::fromStdString(error);
    invokeHandler(false);
}

void QWebRTCSetSessionDescriptionObserver_p::invokeHandler(bool success)
{
//    auto sImpl = impl.lock();
    if (m_completionHandler) {
        m_completionHandler(success);
    }
//    if (sImpl) {
//        // remove adapter from observer list
//        for (auto it = sImpl->m_setObservers.begin(); it!= sImpl->m_setObservers.end(); ++it) {
//            if ((*it).get() == this) {
//                sImpl->m_setObservers.erase(it);
//                return;
//            }
//        }
//    }
}

webrtc::PeerConnectionInterface::RTCOfferAnswerOptions mapToRTCOfferAnserOptions(const QVariantMap& map)
{
    auto result = webrtc::PeerConnectionInterface::RTCOfferAnswerOptions();
    if (map.contains("receiveVideo")) {
        result.offer_to_receive_video = map["receiveVideo"].toInt();
    }
    if (map.contains("receiveAudio")) {
        result.offer_to_receive_audio = map["receiveAudio"].toInt();
    }
    if (map.contains("voiceActivityDetection")) {
        result.voice_activity_detection = map["voiceActivityDetection"].toBool();
    }
    if (map.contains("iceRestart")) {
        result.ice_restart = map["iceRestart"].toBool();
    }
    if (map.contains("useRtpMux")) {
        result.use_rtp_mux = map["useRtpMux"].toBool();
    }
    return result;
}

QVariantMap RTCOfferAnserOptionsToMap(const webrtc::PeerConnectionInterface::RTCOfferAnswerOptions& options)
{
    QVariantMap result;
    result["receiveVideo"] = options.offer_to_receive_video;
    result["receiveAudio"] = options.offer_to_receive_audio;
    result["voiceActivityDetection"] = options.voice_activity_detection;
    result["iceRestart"] = options.ice_restart;
    result["useRtpMux"] = options.use_rtp_mux;
    return result;
}

void QWebRTCPeerConnection::close()
{
    if(m_impl->_conn) {
        m_impl->_conn->Close();
    }
}

void QWebRTCPeerConnection::createOfferForConstraints(const QVariantMap& constraints,
        std::function<void(const QSharedPointer<QWebRTCSessionDescription>&)> completionHandler)
{
    auto observer = new rtc::RefCountedObject<QWebRTCCreateSessionDescriptionObserver_p>();
    observer->m_completionHandler = completionHandler;
    //observer->impl = m_impl;
    //m_impl->m_createObservers.append(new rtc::RefCountedObject<QWebRTCCreateSessionDescriptionObserver_p>());
    m_impl->_conn->CreateOffer(observer, mapToRTCOfferAnserOptions(constraints));
}

void QWebRTCPeerConnection::createAnswerForConstraints(const QVariantMap& constraints,
        std::function<void(const QSharedPointer<QWebRTCSessionDescription>&)> completionHandler)
{
    auto observer = new rtc::RefCountedObject<QWebRTCCreateSessionDescriptionObserver_p>();
    observer->m_completionHandler = completionHandler;
    //observer->impl = m_impl;
    //m_impl->m_createObservers.append(new rtc::RefCountedObject<QWebRTCCreateSessionDescriptionObserver_p>());
    m_impl->_conn->CreateAnswer(observer, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void QWebRTCPeerConnection::addStream(const QSharedPointer<QWebRTCMediaStream>& stream)
{
    assert(m_impl);
    assert(m_impl->_conn);
    if (!stream) {
        return;
    }
    auto streamImpl = qSharedPointerCast<QWebRTCMediaStream_impl>(stream);
    m_impl->_conn->AddStream(streamImpl->m_nativeStream);
}

void QWebRTCPeerConnection::removeStream(const QSharedPointer<QWebRTCMediaStream>& stream)
{
    assert(m_impl);
    assert(m_impl->_conn);
    if (!stream) {
        return;
    }
    auto streamImpl = qSharedPointerCast<QWebRTCMediaStream_impl>(stream);
    m_impl->_conn->RemoveStream(streamImpl->m_nativeStream);
}

void QWebRTCPeerConnection::setConfiguration()
{
}

void QWebRTCPeerConnection::setLocalDescription(const QSharedPointer<QWebRTCSessionDescription>& description,
        std::function<void(bool)> completionHandler)
{
    qDebug() << "setting local description";
    assert(m_impl);
    assert(m_impl->_conn);
    if (description->isValid()) {
        m_impl->m_localDescription = description;
        auto observer = new rtc::RefCountedObject<QWebRTCSetSessionDescriptionObserver_p>();
        observer->m_completionHandler = completionHandler;
        //observer->impl = m_impl;
        observer->m_remoteDescription = false;
        observer->m_description = description;
        //m_impl->m_setObservers.append(observer);
        m_impl->_conn->SetLocalDescription(observer,
                qSharedPointerCast<QWebRTCSessionDescription_impl>(description)->getNativeDescription());
    } else {
        qWarning() << "session description invalid";
    }
}

void QWebRTCPeerConnection::setRemoteDescription(const QSharedPointer<QWebRTCSessionDescription>& description,
        std::function<void(bool)> completionHandler)
{
    qDebug() << "setting remote description";
    assert(m_impl);
    assert(m_impl->_conn);
    if (description->isValid()) {
        m_impl->m_remoteDescription = description;
        rtc::scoped_refptr<QWebRTCSetSessionDescriptionObserver_p> observer(
            new rtc::RefCountedObject<QWebRTCSetSessionDescriptionObserver_p>());

        observer->m_completionHandler = completionHandler;
        //observer->impl = m_impl;
        observer->m_remoteDescription = true;
        observer->m_description = description;
        //m_impl->m_setObservers.append(observer);
        m_impl->_conn->SetRemoteDescription(observer,
                qSharedPointerCast<QWebRTCSessionDescription_impl>(description)->getNativeDescription());
    } else {
        qWarning() << "session description invalid";
    }
}

void QWebRTCPeerConnection::addIceCandidate(const QSharedPointer<QWebRTCIceCandidate>& iceCandidate)
{
    assert(m_impl);
    assert(m_impl->_conn);
    if (iceCandidate->isValid()) {
        m_impl->_conn->AddIceCandidate(
                    qSharedPointerCast<QWebRTCIceCandidate_impl>(iceCandidate)->iceCandidate());
    } else {
        qWarning() << "invalid ICE candidate";
    }
}

void QWebRTCPeerConnection::removeIceCandidate(const QSharedPointer<QWebRTCIceCandidate>& iceCandidate)
{
    assert(m_impl);
    assert(m_impl->_conn);
    if (iceCandidate->isValid()) {
        m_impl->_conn->AddIceCandidate(
                    qSharedPointerCast<QWebRTCIceCandidate_impl>(iceCandidate)->iceCandidate());
    } else {
        qWarning() << "invalid ICE candidate";
    }
}

QSharedPointer<QWebRTCDataChannel> QWebRTCPeerConnection::dataChannelForLabel(const QString& label, const QWebRTCDataChannelConfig& config)
{
    const webrtc::DataChannelInit nativeInit;
    return QSharedPointer<QWebRTCDataChannel_impl>(new QWebRTCDataChannel_impl(m_impl->_conn->CreateDataChannel(label.toStdString(), 0)));
}

QSharedPointer<QWebRTCSessionDescription> QWebRTCPeerConnection::createSessionDescription(QWebRTCSessionDescription::SDPType type,
        const QByteArray& sdp)
{
    return QSharedPointer<QWebRTCSessionDescription_impl>(new QWebRTCSessionDescription_impl(type, sdp));
}

QSharedPointer<QWebRTCIceCandidate> QWebRTCPeerConnection::createIceCandidate(QByteArray mId, int sdpMLineIndex,
        const QByteArray& sdpData)
{
    return QSharedPointer<QWebRTCIceCandidate_impl>(new QWebRTCIceCandidate_impl(mId, sdpMLineIndex, sdpData));
}

QSharedPointer<QWebRTCSessionDescription> QWebRTCPeerConnection::currentLocalDescription()
{
    if (!m_impl->_conn->current_local_description())
        return QSharedPointer<QWebRTCSessionDescription>();

    return QSharedPointer<QWebRTCSessionDescription_impl>(new QWebRTCSessionDescription_impl(m_impl->_conn->current_local_description()));
}

QSharedPointer<QWebRTCSessionDescription> QWebRTCPeerConnection::currentRemoteDescription()
{
    if (!m_impl->_conn->current_remote_description())
        return QSharedPointer<QWebRTCSessionDescription>();

    return QSharedPointer<QWebRTCSessionDescription_impl>(new QWebRTCSessionDescription_impl(m_impl->_conn->current_local_description()));
}

QWebRTCPeerConnection::SignalingState QWebRTCPeerConnection::signalingState()
{
    return static_cast<QWebRTCPeerConnection::SignalingState>(m_impl->_conn->signaling_state());
}

QWebRTCPeerConnection::IceConnectionState QWebRTCPeerConnection::iceConnectionState()
{
    return static_cast<QWebRTCPeerConnection::IceConnectionState>(m_impl->_conn->ice_connection_state());
}

QWebRTCPeerConnection::IceGatheringState QWebRTCPeerConnection::iceGatheringState()
{
    return static_cast<QWebRTCPeerConnection::IceGatheringState>(m_impl->_conn->ice_gathering_state());
}

QWebRTCPeerConnection::QWebRTCPeerConnection()
    : m_impl(new QWebRTCPeerConnection_impl(this))
{
}

QWebRTCPeerConnection::~QWebRTCPeerConnection()
{
    delete m_impl;
}

QWebRTCPeerConnection_impl::QWebRTCPeerConnection_impl(QWebRTCPeerConnection* q_ptr)
    : q_ptr(q_ptr)
{
    qDebug() << "Creating QWebRTCPeerConnection";
}

QWebRTCPeerConnection_impl::~QWebRTCPeerConnection_impl()
{
    qDebug() << "Destroying QWebRTCPeerConnection";
}

void QWebRTCPeerConnection_impl::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    qDebug() << "signaling change " << (int) new_state;
    q_ptr->signalingChange();
}

void QWebRTCPeerConnection_impl::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
    auto qStream = QSharedPointer<QWebRTCMediaStream_impl>(new QWebRTCMediaStream_impl(stream));
    Q_EMIT q_ptr->streamAdded(qStream);
    qDebug() << "Stream added " << QString::fromStdString(stream->label());
}

void QWebRTCPeerConnection_impl::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    auto qStream = QSharedPointer<QWebRTCMediaStream_impl>(new QWebRTCMediaStream_impl(stream));
    Q_EMIT q_ptr->streamRemoved(qStream);
}

void QWebRTCPeerConnection_impl::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
    // Don't do anything here. The scoped pointer function will be called with the same datachannel after
}

void QWebRTCPeerConnection_impl::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    qDebug() << "New data channel";
    Q_EMIT q_ptr->dataChannelReceived(QSharedPointer<QWebRTCDataChannel_impl>(new QWebRTCDataChannel_impl(data_channel)));
}

void QWebRTCPeerConnection_impl::OnDataChannel(webrtc::DataChannelInterface* data_channel)
{
    // Don't do anything here. The scoped pointer function will be called with the same datachannel after
}

void QWebRTCPeerConnection_impl::OnRenegotiationNeeded() {
    qDebug() << "Negotiation needed";
    Q_EMIT q_ptr->renegotiationNeeded();
}

void QWebRTCPeerConnection_impl::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    Q_EMIT q_ptr->iceConnectionStateChanged();
}

void QWebRTCPeerConnection_impl::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    Q_EMIT q_ptr->iceGatheringChanged();
}

void QWebRTCPeerConnection_impl::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    qDebug() << "Ice candidate ready";
    Q_EMIT q_ptr->newIceCandidate(QSharedPointer<QWebRTCIceCandidate_impl>(new QWebRTCIceCandidate_impl(candidate)));
}

void QWebRTCPeerConnection_impl::OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates)
{
    std::vector<QSharedPointer<QWebRTCIceCandidate_impl>> can;
    for (auto candidate : candidates) {
        std::unique_ptr<webrtc::JsepIceCandidate> candidate_wrapper(
            new webrtc::JsepIceCandidate(candidate.transport_name(), -1, candidate));
        can.push_back(QSharedPointer<QWebRTCIceCandidate_impl>(new QWebRTCIceCandidate_impl(candidate_wrapper.get())));
    }
    qDebug() << "Ice candidate removed";
}

void QWebRTCPeerConnection_impl::OnIceConnectionReceivingChange(bool receiving)
{

}

void QWebRTCPeerConnection_impl::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
    const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams) {
    qDebug() << "video track added";
}
