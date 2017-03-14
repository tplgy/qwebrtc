#!/usr/bin/python

import argparse
import os
import shutil
import subprocess
import sys
from multiprocessing import cpu_count

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--clean", help="Force clean build", action="store_true")
parser.add_argument("-r", "--release", help="Build in release mode", action="store_true")
parser.add_argument("-j", "--jobs", help="Specify how many commands can run in parallel", type=int, default=cpu_count())
parser.add_argument("-w", "--buildwebrtc", help="Checkout and build WebRTC", action="store_true")
parser.add_argument("--asan", help="Build with ASAN", action="store_true")
parser.add_argument("--tsan", help="Build with TSAN", action="store_true")
parser.add_argument("--ubsan", help="Build with UBSAN", action="store_true")
args = parser.parse_args()


main_dir = os.path.dirname(os.path.abspath(__file__))
configuration = "Release" if args.release else "Debug"
build_dir = os.path.join(os.path.dirname(main_dir), "out/" + configuration)

if args.clean and os.path.exists(build_dir):
    shutil.rmtree(build_dir)

if args.buildwebrtc:
    build_args = [main_dir + "/build_webrtc.py"]
    if args.release:
        build_args.append("-r")
    r = subprocess.call(build_args)
    if r != 0:
       exit(r)

build_args = ["cmake", "-GNinja", "-DCMAKE_BUILD_TYPE=" + configuration]
if args.asan:
    build_args.append("-DENABLE_ASAN=ON")
if args.tsan:
    build_args.append("-DENABLE_TSAN=ON")
if args.ubsan:
    build_args.append("-DENABLE_UBSAN=ON")
build_args.append(main_dir)
r = subprocess.call(build_args)
if r != 0:
    exit(r)

exit(subprocess.call("ninja -j" + str(args.jobs), shell=True))
