//################################################################
//# Copyright (C) 2022, EduArt Robotik GmbH, All rights reserved.#
//# Further information can be found in the LICENSE file.        #
//################################################################

/**
 * @brief Example template to show how the IOTBot (Eduard Performance V2) is controlled
 * In this case a simple State machine that turns the robot around its own axis (could be extended). 
 * ATENTION!!! Place the robot on the storage frame to avoid it from falling
 * 
 */

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <unistd.h>

enum MovementState{INIT, ENABLE, WAIT1, WAIT2, FORWARD, BACKWARD, TURNLEFT, TURNRIGHT};

int main(int argc, char** argv)
{
     ros::init(argc, argv, "eduard_test");
     ros:: NodeHandle nh = ros::NodeHandle("~");
     ros::Publisher joy_pub = nh.advertise<sensor_msgs::Joy>("/joy", 10);

     ros::Rate node_rate(10);

    sensor_msgs::Joy joy_buf;
    joy_buf.header.frame_id = "joy";
    // Create "joy" message
    for(int i=0; i<4; i++)
        joy_buf.axes.push_back(0.0);
        
    for(int i=0; i<11; i++)
        joy_buf.buttons.push_back(0);

    MovementState state = INIT;
    ros::Time tRef = ros::Time::now();


        joy_buf.axes[0] = 0.0;
        joy_buf.axes[1] = 0.0;
        joy_buf.axes[2] = 0.0;
        joy_buf.axes[3] = 1.0;
        joy_buf.buttons[10] = 0;        // Enable button

    while(ros::ok())
    {
        ros::Time t      = ros::Time::now();
        ros::Duration dt = t - tRef;
        double elapsed   = dt.toSec();

        switch(state)
        {
            case INIT:
                // just send once the default message
                if(elapsed > 1.0)
                {
                    state = ENABLE;
                    tRef = t;
                }
            break;
            case ENABLE:
                // send enable signal
                joy_buf.buttons[10] = 1;
                if(elapsed > 1.0)
                {
                    state = WAIT1;
                    tRef = t;
                }
            break;
            case WAIT1:
                // wait, while sending the default message
                if(elapsed > 1.0)
                {
                    state = TURNLEFT;
                    tRef = t;
                }
            break;
            case TURNLEFT:
                // turn robot left for a fixed period
                joy_buf.axes[2] = 1.0;

                if(elapsed > 60.0)
                {
                    state = WAIT2;
                    tRef  = t;
                }
            break;
            case WAIT2:
                // wait, while sending the default message
                if(elapsed > 1.0)
                {
                    state = TURNRIGHT;
                    tRef = t;
                }
            break;
            case TURNRIGHT:
                // turn robot right for a fixed period
                joy_buf.axes[2] = -1.0;

                if(elapsed > 60.0)
                {
                    state = WAIT1;
                    tRef  = t;
                }
            break;
        }

        //Enable robots movement (maybe disabled through HALT-Button or Power Supply)
        std::cout << "Joy Enable: " << joy_buf.buttons[10] << std::endl;
        joy_pub.publish(joy_buf);
        node_rate.sleep();
     }

     return 0;
}