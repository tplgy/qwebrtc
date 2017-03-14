cmake_minimum_required(VERSION 3.1)
project(QWebRTC C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
add_definitions(-DWEBRTC_POSIX)

option(ENABLE_ASAN "ASAN build" OFF)
option(ENABLE_TSAN "TSAN build" OFF)
option(ENABLE_UBSAN "UBSAN build" OFF)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-deprecated-declarations -Wno-error=unused-variable ${CMAKE_CXX_FLAGS}")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND "${CMAKE_GENERATOR}" STREQUAL "Ninja")
    set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always -Wno-error=maybe-uninitialized -Wno-error=unused-local-typedefs ${CMAKE_CXX_FLAGS}")
endif()

if(ENABLE_ASAN)
    set(CMAKE_C_FLAGS "-fsanitize=address ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=address ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=address ${CMAKE_EXE_LINKER_FLAGS}")
endif()
if(ENABLE_TSAN)
    set(CMAKE_C_FLAGS "-fsanitize=thread ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fsanitize=thread ${CMAKE_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=thread ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=thread ${CMAKE_EXE_LINKER_FLAGS}")
endif()
if(ENABLE_UBSAN)
    if(APPLE)
        set(CMAKE_C_FLAGS "-fsanitize=undefined-trap -fsanitize-undefined-trap-on-error ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-fsanitize=undefined-trap -fsanitize-undefined-trap-on-error ${CMAKE_CXX_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=undefined-trap -fsanitize-undefined-trap-on-error ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=undefined-trap -fsanitize-undefined-trap-on-error ${CMAKE_EXE_LINKER_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "-fsanitize=undefined ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-fsanitize=undefined ${CMAKE_CXX_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-fsanitize=undefined ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=undefined ${CMAKE_EXE_LINKER_FLAGS}")
    endif()
endif()

include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}/webrtc/src/"
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_path(WebRTCOutDir webrtc
    HINTS "${CMAKE_CURRENT_BINARY_DIR}/webrtc/src/out/Default/obj"
    PATH_SUFFIXES webrtc)

find_package(Qt5 COMPONENTS Core Widgets Quick)

set(INSTALL_DISABLED TRUE)

set(LIB_SOURCES
    "qwebrtc.cpp"
    "qwebrtcpeerconnectionfactory.cpp"
    "qwebrtcpeerconnection.cpp"
    "qwebrtcdatachannel.cpp"
    "qwebrtcmediastream.cpp"
    "qwebrtcsessiondescription.cpp"
    "qwebrtcmediatrack.cpp"
    "qwebrtcutilities.cpp"
    "qwebrtcquickvideoitem.cpp"
    "qwebrtcicecandidate.cpp"
    "qwebrtcdesktopvideosource.cpp"
)

set(PUBLIC_HEADER
    "qwebrtc.h"
    "qwebrtcpeerconnectionfactory.hpp"
    "qwebrtcpeerconnection.hpp"
    "qwebrtcdatachannel.hpp"
    "qwebrtcmediastream.hpp"
    "qwebrtcsessiondescription.hpp"
    "qwebrtcmediatrack.hpp"
    "qwebrtcutilities.hpp"
    "qwebrtcquickvideoitem.hpp"
    "qwebrtcicecandidate.hpp"
    "qwebrtcconfiguration.hpp"
)

install(FILES ${PUBLIC_HEADER} DESTINATION "${CMAKE_BINARY_DIR}/include")

add_definitions(-DQT_NO_KEYWORDS)

set(PRIVATE_HEADERS
    "qwebrtcpeerconnection_p.hpp"
    "qwebrtcmediatrack_p.hpp"
    "qwebrtcmediastream_p.hpp"
    "qwebrtcdatachannel_p.hpp"
    "qwebrtctypes_p.hpp"
    "qwebrtcquickvideoitem_p.hpp"
    "qwebrtcdesktopvideosource_p.hpp"
)

add_library(${PROJECT_NAME} SHARED
    ${LIB_SOURCES}
    ${PRIVATE_HEADERS}
)

file(GLOB_RECURSE BORING_SSL_OBJ
    "${WebRTCOutDir}/../third_party/boringssl/boringssl/*.o"
    "${WebRTCOutDir}/../third_party/boringssl/boringssl_asm/*.o"
)

file(GLOB_RECURSE VPX
    #"${WebRTCOutDir}/../third_party/libvpx/libvpx_yasm/*.o"
    "${WebRTCOutDir}/../third_party/libvpx/*.o"
    #"${WebRTCOutDir}/../third_party/libjpeg_turbo/libjpeg/*.o"
    "${WebRTCOutDir}/../third_party/libjpeg_turbo/*.o"
    "${WebRTCOutDir}/../third_party/openmax_dl/dl/dl/*.o"
    "${WebRTCOutDir}/api/video_frame_api/i420_buffer.o"
    "${WebRTCOutDir}/api/video_frame_api/video_frame.o"
    #"${WebRTCOutDir}/api/audio_codecs/builtin_audio_decoder_factory/builtin_audio_decoder_factory.o"
    "${WebRTCOutDir}/api/audio_codecs/audio_codecs_api/audio_decoder.o"
    #"${WebRTCOutDir}/modules/audio_coding/builtin_audio_decoder_factory_internal/builtin_audio_decoder_factory_internal.o"
    "${WebRTCOutDir}/common_audio/common_audio_c/*.o"
    "${WebRTCOutDir}/api/audio_codecs/audio_codecs_api/audio_format.o"
    "${WebRTCOutDir}/modules/media_file/media_file/media_file_impl.o"
    "${WebRTCOutDir}/modules/media_file/media_file/media_file_utility.o"
    "${WebRTCOutDir}/modules/audio_processing/audio_processing/*.o"
    "${WebRTCOutDir}/modules/audio_processing/audio_processing_c/*.o"
    "${WebRTCOutDir}/modules/audio_processing/audio_processing_sse2/*o"
    "${WebRTCOutDir}/modules/audio_processing/audioproc_debug_proto/debug.pb.o"
    "${WebRTCOutDir}/modules/audio_conference_mixer/audio_conference_mixer/*.o"
    "${WebRTCOutDir}/modules/audio_coding/audio_coding/*.o"
    "${WebRTCOutDir}/modules/audio_coding/audio_encoder_interface/*.o"
    "${WebRTCOutDir}/modules/audio_coding/webrtc_opus_c/*.o"
    "${WebRTCOutDir}/modules/audio_coding/g722/*.o"
    "${WebRTCOutDir}/modules/audio_coding/g722_c/*.o"
    "${WebRTCOutDir}/modules/audio_coding/cng/*.o"
    "${WebRTCOutDir}/modules/audio_coding/rent_a_codec/*.o"
    "${WebRTCOutDir}/modules/audio_coding/ilbc/*.o"
    "${WebRTCOutDir}/modules/audio_coding/ilbc_c/*.o"
    "${WebRTCOutDir}/modules/audio_coding/g711/*.o"
    "${WebRTCOutDir}/modules/audio_coding/g711_c/*.o"
    "${WebRTCOutDir}/modules/audio_coding/pcm16b/*.o"
    "${WebRTCOutDir}/modules/audio_coding/pcm16b_c/*.o"
    "${WebRTCOutDir}/modules/audio_device/audio_device/*.o"
    "${WebRTCOutDir}/modules/audio_device/mac_portaudio/*.o"
    "${WebRTCOutDir}/modules/audio_coding/audio_network_adaptor/*.o"
    "${WebRTCOutDir}/modules/audio_coding/red/*.o"
    "${WebRTCOutDir}/modules/audio_coding/ana_debug_dump_proto/*.o"
    "${WebRTCOutDir}/modules/audio_coding/ana_config_proto/config.pb.o"
    "${WebRTCOutDir}/modules/audio_coding/legacy_encoded_audio_frame/legacy_encoded_audio_frame.o"
    "${WebRTCOutDir}/modules/audio_coding/isac_common/locked_bandwidth_info.o"
    "${WebRTCOutDir}/modules/audio_coding/neteq/*.o"
    "${WebRTCOutDir}/modules/remote_bitrate_estimator/remote_bitrate_estimator/*.o"
    "${WebRTCOutDir}/modules/bitrate_controller/bitrate_controller/*.o"
    "${WebRTCOutDir}/call/call_interfaces/audio_send_stream.o"
    "${WebRTCOutDir}/call/call_interfaces/syncable.o"
    "${WebRTCOutDir}/modules/congestion_controller/congestion_controller/*.o"
    "${WebRTCOutDir}/modules/audio_coding/isac_c/*.o"
    "${WebRTCOutDir}/audio/utility/audio_frame_operations/audio_frame_operations.o"
    "${WebRTCOutDir}/voice_engine/level_indicator/level_indicator.o"
    "${WebRTCOutDir}/logging/rtc_event_log_proto/rtc_event_log.pb.o"
    "${WebRTCOutDir}/system_wrappers/metrics_default/metrics_default.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_module/video_capture_factory.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_module/video_capture_impl.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_internal_impl/device_info.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_module/device_info_impl.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_internal_impl/video_capture.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_internal_impl/rtc_video_capture_objc.o"
    "${WebRTCOutDir}/modules/video_capture/video_capture_internal_impl/device_info_objc.o"
    "${WebRTCOutDir}/../third_party/openh264/*.o"
    #"${WebRTCOutDir}/sdk/webrtc_h264_video_toolbox/h264_video_toolbox_decoder.o"
    #"${WebRTCOutDir}/sdk/webrtc_h264_video_toolbox/h264_video_toolbox_encoder.o"
    #"${WebRTCOutDir}/sdk/webrtc_h264_video_toolbox/h264_video_toolbox_nalu.o"
    #"${WebRTCOutDir}/sdk/rtc_sdk_peerconnection_objc/videotoolboxvideocodecfactory.o"
    #"${WebRTCOutDir}/media/rtc_media/payload_type_mapper.o"
)

find_library(FoundationLibs Foundation)
find_library(CoreAudioLibs CoreAudio)
find_library(AudioToolboxLibs AudioToolbox)
find_library(CoreGraphicsLibs CoreGraphics)
find_library(CoreVideoLibs CoreVideo)
find_library(CoreMediaLibs CoreMedia)
find_library(VideoToolboxLibs VideoToolbox)
find_library(AVFoundationLibs AVFoundation)
find_library(OpenGlLibs OpenGl)
find_library(CocoaLibs Cocoa)

target_link_libraries(${PROJECT_NAME}
    "${WebRTCOutDir}/libwebrtc.a"
    "${WebRTCOutDir}/libwebrtc_common.a"
    #"${WebRTCOutDir}/pc/libjingle_peerconnection.a"
    "${WebRTCOutDir}/pc/librtc_pc.a"
    "${WebRTCOutDir}/call/libcall.a"
    "${WebRTCOutDir}/audio/libaudio.a"
    "${WebRTCOutDir}/common_audio/libcommon_audio.a"
    "${WebRTCOutDir}/common_audio/libcommon_audio_sse2.a"
    "${WebRTCOutDir}/base/librtc_base_approved.a"
    "${WebRTCOutDir}/base/librtc_base.a"
    "${WebRTCOutDir}/video/libvideo.a"
    "${WebRTCOutDir}/common_video/libcommon_video.a"
    "${WebRTCOutDir}/base/librtc_numerics.a"
    "${WebRTCOutDir}/base/librtc_task_queue.a"
    #"${WebRTCOutDir}/base/librtc_task_runner.a"
    "${WebRTCOutDir}/media/librtc_media.a"
    "${WebRTCOutDir}/media/librtc_media_base.a"
    "${WebRTCOutDir}/api/libjingle_peerconnection.a"
    "${WebRTCOutDir}/../third_party/usrsctp/libusrsctp.a"
    "${WebRTCOutDir}/../third_party/libsrtp/libsrtp.a"
    "${WebRTCOutDir}/../third_party/libyuv/libyuv.a"
    "${WebRTCOutDir}/../third_party/expat/libexpat.a"
    "${WebRTCOutDir}/../third_party/opus/libopus.a"
    "${WebRTCOutDir}/../third_party/libvpx/libvpx.a"
    "${WebRTCOutDir}/../third_party/protobuf/libprotobuf_full.a"
    "${WebRTCOutDir}/../third_party/ffmpeg/libffmpeg_internal.a"
    "${WebRTCOutDir}/../third_party/ffmpeg/libffmpeg_yasm.a"
    "${BORING_SSL_OBJ}"
    "${VPX}"
    "${WebRTCOutDir}/p2p/librtc_p2p.a"
    "${WebRTCOutDir}/modules/rtp_rtcp/librtp_rtcp.a"
    "${WebRTCOutDir}/modules/audio_coding/libisac.a"
    "${WebRTCOutDir}/modules/audio_coding/libwebrtc_opus.a"
    "${WebRTCOutDir}/modules/audio_coding/libaudio_format_conversion.a"
    "${WebRTCOutDir}/modules/audio_coding/libaudio_decoder_interface.a"
    "${WebRTCOutDir}/modules/audio_coding/libbuiltin_audio_decoder_factory.a"
    "${WebRTCOutDir}/modules/audio_coding/libaudio_format.a"
    "${WebRTCOutDir}/modules/video_coding/libwebrtc_i420.a"
    "${WebRTCOutDir}/modules/video_coding/libwebrtc_vp8.a"
    "${WebRTCOutDir}/modules/video_coding/libwebrtc_vp9.a"
    "${WebRTCOutDir}/modules/video_coding/libvideo_coding.a"
    "${WebRTCOutDir}/modules/video_coding/libwebrtc_h264.a"
    "${WebRTCOutDir}/modules/video_coding/libvideo_coding_utility.a"
    "${WebRTCOutDir}/modules/pacing/libpacing.a"
    "${WebRTCOutDir}/modules/utility/libutility.a"
    "${WebRTCOutDir}/modules/audio_mixer/libaudio_mixer_impl.a"
    "${WebRTCOutDir}/modules/audio_mixer/libaudio_frame_manipulator.a"
    "${WebRTCOutDir}/modules/desktop_capture/libdesktop_capture.a"
    "${WebRTCOutDir}/modules/desktop_capture/libprimitives.a"
    "${WebRTCOutDir}/modules/desktop_capture/libdesktop_capture_differ_sse2.a"
    "${WebRTCOutDir}/system_wrappers/libsystem_wrappers.a"
    "${WebRTCOutDir}/system_wrappers/libfield_trial_default.a"
    "${WebRTCOutDir}/voice_engine/libvoice_engine.a"
    "${WebRTCOutDir}/voice_engine/libfile_player.a"
    "${WebRTCOutDir}/voice_engine/libfile_recorder.a"
    "${WebRTCOutDir}/voice_engine/libaudio_coder.a"
    "${WebRTCOutDir}/logging/librtc_event_log_impl.a"
    "${WebRTCOutDir}/stats/librtc_stats.a"
    #"${WebRTCOutDir}/api/video_frame_api/i420_buffer.o"
    #"${WebRTCOutDir}/sdk/librtc_sdk_peerconnection_objc.a"
    #"${WebRTCOutDir}/sdk/librtc_sdk_common_objc.a"
    Qt5::Core
    Qt5::Quick
    ${FoundationLibs}
    ${CoreAudioLibs}
    ${AudioToolboxLibs}
    ${CoreGraphicsLibs}
    ${CoreVideoLibs}
    ${CoreMediaLibs}
    ${CocoaLibs}
    #${VideoToolboxLibs}
    ${AVFoundationLibs}
    ${OpenGlLibs}
)
