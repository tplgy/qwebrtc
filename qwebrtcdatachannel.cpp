#include "qwebrtcdatachannel_p.hpp"

QWebRTCDataChannel_impl::QWebRTCDataChannel_impl(rtc::scoped_refptr<webrtc::DataChannelInterface> nativeDC)
    : m_nativeDataChannel(nativeDC)
{
}
