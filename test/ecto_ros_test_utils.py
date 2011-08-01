#!/usr/bin/env python
import ecto
import ecto_ros, ecto_sensor_msgs
import sys
import subprocess
import yaml
import time

def bag_counts(bagname):
    proc = subprocess.Popen(['rosbag','info','-k','topics','-y',bagname],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    stdout,stderr = proc.communicate()
    #should exit without error
    if len(stderr) != 0:
        print stderr
        sys.exit(-1)
    result = yaml.load(stdout)
    counts = {info['topic']:info['messages'] for info in result}
    return counts

def start_roscore(delay=0.5):
    roscore = subprocess.Popen(['roscore'],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    time.sleep(delay)
    return roscore

def play_bag(bagname,delay=0.5,rate=1):
    rosbag = subprocess.Popen(['rosbag','play','-d',str(delay),'-r',str(rate),bagname],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    return rosbag

def wait_bag(rosbag):
    stdout,stderr = rosbag.communicate()
    #should exit without error
    if len(stderr) != 0:
        print stderr
        sys.exit(-1)