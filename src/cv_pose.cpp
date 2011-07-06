/*
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ecto/ecto.hpp>

#include <geometry_msgs/PoseStamped.h>
#include <opencv2/core/core.hpp>
#include <Eigen/Dense>

#include <iostream>
#include <string>

namespace ecto_ros
{

  using ecto::tendrils;
  using std::string;
  using namespace geometry_msgs;

  struct RT2PoseStamped
  {
    static void declare_io(const tendrils& /*p*/, tendrils& i, tendrils& o)
    {
      i.declare<cv::Mat> ("R",
                                "3X3 Rotation matrix.");
      i.declare<cv::Mat> ("T",
                                "3X1 Translation vector.");
      o.declare<PoseStampedConstPtr> ("pose", "A geometry_msgs::PoseStamped.");
    }
    void configure(const tendrils& p, tendrils& i, tendrils& o)
    {
      R_ = i.at("R");
      T_ = i.at("T");
      pose_ = o.at("pose");
      frame_id_ = "ecto_frame";
    }
    int process(const tendrils&, tendrils&)
    {
      cv::Mat R,T;
      R_().convertTo(R,CV_32F);T_().convertTo(T,CV_32F);
      Eigen::Matrix3f rotation_matrix;
      for (unsigned int j = 0; j < 3; ++j)
        for (unsigned int i = 0; i < 3; ++i)
          rotation_matrix(j, i) = R.at<float> (j, i);

      Eigen::Quaternion<float> quaternion(rotation_matrix);
      wpose_.reset(new PoseStamped);
      PoseStamped& pose = *wpose_;
      pose.pose.position.x = T.at<float> (0);
      pose.pose.position.y = T.at<float> (1);
      pose.pose.position.z = T.at<float> (2);
      pose.pose.orientation.x = quaternion.x();
      pose.pose.orientation.y = quaternion.y();
      pose.pose.orientation.z = quaternion.z();
      pose.pose.orientation.w = quaternion.w();
      pose.header.seq++;
      pose.header.stamp = ros::Time::now();
      pose.header.frame_id = frame_id_;
      *pose_ = wpose_;
      return ecto::OK;
    }
    PoseStampedPtr wpose_;
    std::string frame_id_;
    ecto::spore<PoseStampedConstPtr> pose_;
    ecto::spore<cv::Mat> R_,T_;
  };

}

ECTO_CELL(ecto_ros, ecto_ros::RT2PoseStamped, "RT2PoseStamped", "Takes an R and T cv::Mat style and emmits a stamped pose.");
