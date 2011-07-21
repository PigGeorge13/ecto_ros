#!/usr/bin/env python
PKG = 'ecto_ros' # this package name
import roslib; roslib.load_manifest(PKG)
import ecto
import ecto_ros, ecto_sensor_msgs
from ecto_opencv import highgui
import sys


ImageSub = ecto_sensor_msgs.Subscriber_Image
CameraInfoSub = ecto_sensor_msgs.Subscriber_CameraInfo

def do_ecto():
    sub_rgb = ImageSub("image_sub",topic_name='/camera/rgb/image_mono')
    sub_depth = ImageSub("depth_sub",topic_name='/camera/depth/image')
    sub_depth_info = CameraInfoSub(topic_name='/camera/depth/camera_info')
    sub_rgb_info = CameraInfoSub(topic_name='/camera/rgb/camera_info')
    
    im2mat_rgb = ecto_ros.Image2Mat()
    im2mat_depth = ecto_ros.Image2Mat()

    graph = [
                sub_rgb["output"]>>im2mat_rgb["image"],
                im2mat_rgb["image"] >> highgui.imshow("rgb show",name="rgb", waitKey=5)[:],
                sub_depth["output"]>> im2mat_depth["image"],
                im2mat_depth["image"] >> highgui.imshow("depth show",name="depth", waitKey=-1)[:]
            ]
    plasm = ecto.Plasm()
    plasm.connect(graph)
    ecto.view_plasm(plasm)
    
    sched = ecto.schedulers.Threadpool(plasm)
    sched.execute()

if __name__ == "__main__":
    ecto_ros.init(sys.argv,"image_node")
    do_ecto()
