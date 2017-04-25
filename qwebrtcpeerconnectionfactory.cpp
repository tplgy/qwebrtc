#include <memory>
#include <qwebrtcpeerconnectionfactory.hpp>
#include <webrtc/api/peerconnectioninterface.h>
#include "qwebrtcpeerconnection_p.hpp"
#include "qwebrtcpeerconnection.hpp"
#include "qwebrtcmediatrack_p.hpp"
#include "qwebrtcmediastream_p.hpp"
#include "qwebrtcicecandidate.hpp"
#include "qwebrtcconfiguration.hpp"
#include "qwebrtcdatachannel.hpp"
#include "qwebrtcdesktopvideosource_p.hpp"
//#include <webrtc/sdk/objc/Framework/Classes/videotoolboxvideocodecfactory.h>
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"
#include <QThread>
#include <QCoreApplication>

class QWebRTCPeerConnectionFactory_impl {
public:
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> native_interface;

    std::unique_ptr<rtc::Thread> m_workerThread;
    std::unique_ptr<rtc::Thread> m_signalingThread;
    std::unique_ptr<rtc::Thread> m_networkingThread;
};

//QWebRTCPeerConnectionFactory_impl::~QWebRTCPeerConnectionFactory_impl()
//{
//    m_workerThread->Quit();
//    m_signalingThread->Quit();
//    m_networkingThread->Quit();
//    while (m_workerThread->IsQuitting() || m_signalingThread->IsQuitting() || m_networkingThread->IsQuitting()) {
//        QThread::currentThread()->msleep(50);
//    }
//}

Q_DECLARE_METATYPE(QSharedPointer<QWebRTCIceCandidate>)
Q_DECLARE_METATYPE(QSharedPointer<QWebRTCMediaStream>)
Q_DECLARE_METATYPE(QSharedPointer<QWebRTCDataChannel>)

QWebRTCPeerConnectionFactory::QWebRTCPeerConnectionFactory()
{
    qRegisterMetaType<QSharedPointer<QWebRTCIceCandidate> >();
    qRegisterMetaType<QSharedPointer<QWebRTCMediaStream> >();
    qRegisterMetaType<QSharedPointer<QWebRTCDataChannel> >();
    m_impl = QSharedPointer<QWebRTCPeerConnectionFactory_impl>(new QWebRTCPeerConnectionFactory_impl());
    m_impl->m_networkingThread = rtc::Thread::CreateWithSocketServer();
    if (!m_impl->m_networkingThread->Start()) {
        qWarning() << "Faild to start networking thread";
    }


    m_impl->m_workerThread = rtc::Thread::Create();
    if (!m_impl->m_workerThread->Start()) {
        qWarning() <<"Failed to start worker thread.";
    }

    m_impl->m_signalingThread = rtc::Thread::Create();
    if (!m_impl->m_signalingThread->Start()) {
        qWarning() << "Failed to start signaling thread.";
    }

    //const auto encoder_factory = new webrtc::VideoToolboxVideoEncoderFactory();
    //const auto decoder_factory = new webrtc::VideoToolboxVideoDecoderFactory();

    m_impl->native_interface = webrtc::CreatePeerConnectionFactory(
                m_impl->m_networkingThread.get(), m_impl->m_workerThread.get(), m_impl->m_signalingThread.get(),
                nullptr, nullptr, nullptr/*encoder_factory, decoder_factory*/);
}

QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createAudioTrack(const QVariantMap& constraints, const QString& label)
{
    auto audioSource = m_impl->native_interface->CreateAudioSource(0);
    auto audioTrack = m_impl->native_interface->CreateAudioTrack(label.toStdString(), audioSource);
    return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(audioTrack));
}

QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createVideoTrack(const QVariantMap& constraints, const QString& label)
{
    std::vector<std::string> device_names;
    {
        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
                    webrtc::VideoCaptureFactory::CreateDeviceInfo());
        if (!info) {
            return QSharedPointer<QWebRTCMediaTrack>();
        }
        int num_devices = info->NumberOfDevices();
        for (int i = 0; i < num_devices; ++i) {
            const uint32_t kSize = 256;
            char name[kSize] = {0};
            char id[kSize] = {0};
            if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
                device_names.push_back(name);
                qDebug() << "Video device " << i << " " << QString::fromStdString(name);
            }
        }
    }

    cricket::WebRtcVideoDeviceCapturerFactory factory;
    std::unique_ptr<cricket::VideoCapturer> capturer;
    for (const auto& name : device_names) {
        capturer = factory.Create(cricket::Device(name, 0));
        if (capturer) {
            break;
        }
    }
    if (!capturer) {
        qWarning() << "Could not find a camera device";
        return QSharedPointer<QWebRTCMediaTrack>();
    }

    auto videoSource = m_impl->native_interface->CreateVideoSource(std::move(capturer));
    auto videoTrack = m_impl->native_interface->CreateVideoTrack(label.toStdString(), videoSource);
    return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(videoTrack, videoSource));
}

QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createScreenCaptureTrack(const QString& label)
{
    auto videoSource = new rtc::RefCountedObject<QWebRTCDesktopVideoSource>();
    videoSource->moveToThread(QCoreApplication::instance()->thread());
    videoSource->Start();
    auto videoTrack = m_impl->native_interface->CreateVideoTrack(label.toStdString(), videoSource);
    return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(videoTrack, videoSource));
}

QSharedPointer<QWebRTCMediaStream> QWebRTCPeerConnectionFactory::createMediaStream(const QString& label)
{
   return QSharedPointer<QWebRTCMediaStream>(new QWebRTCMediaStream_impl(m_impl->native_interface->CreateLocalMediaStream(label.toStdString())));
}

QSharedPointer<QWebRTCPeerConnection> QWebRTCPeerConnectionFactory::createPeerConnection(const QWebRTCConfiguration& config)
{
    rtc::LogMessage::LogToDebug(rtc::LS_WARNING);
    auto webRTCCOnfig = webrtc::PeerConnectionInterface::RTCConfiguration();
    std::vector<webrtc::PeerConnectionInterface::IceServer> servers;
    for (auto server : config.iceServers) {
        webrtc::PeerConnectionInterface::IceServer iceS;
        iceS.tls_cert_policy = server.tlsCertNoCheck ? webrtc::PeerConnectionInterface::kTlsCertPolicySecure : webrtc::PeerConnectionInterface::kTlsCertPolicyInsecureNoCheck;
        for (auto url : server.urls) {
            iceS.urls.push_back(url.toStdString());
        }
        iceS.username = server.username.toStdString();
        iceS.password = server.password.toStdString();
        servers.push_back(iceS);
    }
    webRTCCOnfig.servers = servers;
    auto conn = QSharedPointer<QWebRTCPeerConnection>(new QWebRTCPeerConnection());
    conn->m_impl->m_factory = m_impl;
    conn->m_impl->_conn = m_impl->native_interface->CreatePeerConnection(webRTCCOnfig,
            nullptr, nullptr, conn->m_impl);
    return conn;
}
