#pragma once

#include <memory>
#include <QString>

class QWebRTCIceCandidate_impl;

class QWebRTCIceCandidate {
public:
    virtual QByteArray sdp() const = 0;

    /*! If present, the identifier of the "media stream identification" for the media
     * component this candidate is associated with.
     */
    virtual QByteArray sdpMediaId() const = 0;

    /*! The index (starting at zero) of the media description this candidate is
    *  associated with in the SDP.*/
    virtual int sdpMLineIndex() const = 0;

    virtual bool isValid() const = 0;

    virtual QString type() const = 0;
};
