#pragma once

#include <memory>
#include <QString>

class QWebRTCMediaTrack;
class QWebRTCMediaStream {
public:
    virtual void addTrack(const std::shared_ptr<QWebRTCMediaTrack>&) = 0;
    virtual void removeTrack(const std::shared_ptr<QWebRTCMediaTrack>&) = 0;
    virtual QList<std::shared_ptr<QWebRTCMediaTrack>> tracks() = 0;
};
