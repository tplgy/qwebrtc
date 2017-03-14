#pragma once

#include <memory>
#include <QString>
#include <QObject>

namespace QMediaTrackConstraints {
    extern const QString minAspectRatio;
    extern const QString maxAspectRatio;
    extern const QString maxWidth;
    extern const QString minWidth;
    extern const QString maxHeight;
    extern const QString minHeight;
    extern const QString maxFrameRate;
    extern const QString minFrameRate;
    extern const QString levelControl;

    extern const QString valueTrue;
    extern const QString valueFalse;
}

class QWebRTCMediaTrack : public QObject {
public:
    enum class Type {
        Video,
        Audio
    };
    virtual Type trackType() = 0;
    virtual bool screenCast() = 0;
    /** sets the volume of the audio track must be between [0,10] */
    virtual void setVolume(uint8_t) = 0;
};
