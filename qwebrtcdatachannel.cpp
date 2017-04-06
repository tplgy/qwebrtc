#include "qwebrtcdatachannel_p.hpp"

QWebRTCDataChannel_impl::QWebRTCDataChannel_impl(rtc::scoped_refptr<webrtc::DataChannelInterface> nativeDC)
    : m_nativeDataChannel(nativeDC)
{
    m_nativeDataChannel->RegisterObserver(this);
}

QWebRTCDataChannel_impl::~QWebRTCDataChannel_impl()
{
    if (channelState() != ChannelState::Closing || channelState() != ChannelState::Closed) {
        close();
    }
}

bool QWebRTCDataChannel_impl::sendData(const QString& str)
{
    webrtc::DataBuffer buffer(str.toStdString());
    return m_nativeDataChannel->Send(buffer);
}

bool QWebRTCDataChannel_impl::sendData(const QByteArray& data)
{
    webrtc::DataBuffer buffer(rtc::CopyOnWriteBuffer(
        reinterpret_cast<const uint8_t*>(data.constData()), data.size()), true);
    return m_nativeDataChannel->Send(buffer);
}

void QWebRTCDataChannel_impl::close()
{
    m_nativeDataChannel->Close();
}

QString QWebRTCDataChannel_impl::label()
{
    return QString::fromStdString(m_nativeDataChannel->label());
}

bool QWebRTCDataChannel_impl::isReliable()
{
    return m_nativeDataChannel->reliable();
}

QString QWebRTCDataChannel_impl::protocol()
{
    return QString::fromStdString(m_nativeDataChannel->protocol());
}

bool QWebRTCDataChannel_impl::isNegotiated()
{
    return m_nativeDataChannel->negotiated();
}

QWebRTCDataChannel::ChannelState QWebRTCDataChannel_impl::channelState()
{
    switch (m_nativeDataChannel->state()) {
    case webrtc::DataChannelInterface::DataState::kConnecting:
        return QWebRTCDataChannel::Connecting;
    case webrtc::DataChannelInterface::DataState::kOpen:
        return QWebRTCDataChannel::Open;
    case webrtc::DataChannelInterface::DataState::kClosing:
        return QWebRTCDataChannel::Closing;
    case webrtc::DataChannelInterface::DataState::kClosed:
        return QWebRTCDataChannel::Closed;
    }
}

uint64_t QWebRTCDataChannel_impl::bufferedAmount()
{
    return m_nativeDataChannel->buffered_amount();
}

void QWebRTCDataChannel_impl::OnStateChange()
{
    Q_EMIT channelStateChanged();
}

void QWebRTCDataChannel_impl::OnMessage(const webrtc::DataBuffer& buffer)
{
    QByteArray data(reinterpret_cast<const char*>(buffer.data.data()), buffer.data.size());
    Q_EMIT dataReceived(data);
}

void QWebRTCDataChannel_impl::OnBufferedAmountChange(uint64_t previous_amount)
{
    Q_EMIT bufferAmountChanged();
}
