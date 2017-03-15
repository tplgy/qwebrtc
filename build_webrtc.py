#!/usr/bin/python

import argparse
import os
import shutil
import subprocess
import sys
from multiprocessing import cpu_count
from distutils import spawn

parser = argparse.ArgumentParser()
#parser.add_argument("-o", "--out_dir", help="WebRTC build directory", action="store_true")
parser.add_argument("-c", "--clean", help="Clean WebRTC build", action="store_true")
parser.add_argument("-r", "--release", help="Build in release mode", action="store_true")
args = parser.parse_args()

if not spawn.find_executable("gclient"):
    print("Could not find depot tools (https://www.chromium.org/developers/how-tos/install-depot-tools)")
    exit(1)

main_dir = os.getcwd()
webrtc_dir = os.path.join(main_dir, "webrtc")
webrtc_src_dir = os.path.join(webrtc_dir, "src")

#WebRTC
if args.clean and os.path.exists(webrtc_dir):
    shutil.rmtree(webrtc_dir)

print(webrtc_dir)
if not os.path.exists(webrtc_dir):
    os.makedirs(webrtc_dir)
    os.chdir(webrtc_dir)
    r = subprocess.call("fetch --nohooks webrtc", shell=True)
    if r != 0:
        exit(r)

    os.chdir(webrtc_src_dir)
    r = subprocess.call("git checkout branch-heads/57", shell=True)
    if r != 0:
        exit(r)
    r = subprocess.call("gclient sync --shallow", shell=True)
    if r != 0:
       exit(r)

os.chdir(webrtc_src_dir)

is_debug = "true"
if args.release:
    is_debug = "false"

r = subprocess.call("gn gen out/Default --args='rtc_include_tests=false proprietary_codecs=true is_debug="+is_debug+" libyuv_include_tests=false rtc_use_h264=true use_external_popup_menu=false rtc_use_gtk=false'", shell=True)
if r != 0:
    exit(r)
exit(subprocess.call("ninja -C out/Default", shell=True))
