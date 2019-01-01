////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "RTIMULib.h"
#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "sensor_msgs/MagneticField.h"

int main(int argc, char * argv[])
{
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;

    ros::Publisher imu_pub;
    ros::Publisher compass_pub; 
  
    //  now just process data
    ros::init(argc, argv, "imu_node");
    ros::NodeHandle n;
 
    imu_pub = n.advertise<sensor_msgs::Imu>("/imu_data",1000);
    compass_pub = n.advertise<sensor_msgs::MagneticField>("/compass_data",1000);



    //  Using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.
    //  Or, you can create the .ini in some other directory by using:
    //      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
    //  where <directory path> is the path to where the .ini file is to be loaded/saved

    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

    RTIMU *imu = RTIMU::createIMU(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    //  This is an opportunity to manually override any settings before the call IMUInit

    //  set up IMU

    imu->IMUInit();

    //  this is a convenient place to change fusion parameters

    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    //  set up for rate timer

    
  
    
    while (ros::ok()) {
        //  poll at the rate recommended by the IMU

        usleep(imu->IMUGetPollInterval() * 1000);

        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();
           
           
            //Create an IMU message
            sensor_msgs::Imu msg_out;

            if(imuData.fusionQPoseValid)
            {
                msg_out.orientation.x = imuData.fusionQPose.x();
                msg_out.orientation.y = imuData.fusionQPose.y();
                msg_out.orientation.z = imuData.fusionQPose.z();
                msg_out.orientation.w = imuData.fusionQPose.scalar();
            }  

            if(imuData.accelValid)
            {
                msg_out.linear_acceleration.x = imuData.accel.x();
                msg_out.linear_acceleration.y = imuData.accel.y();
                msg_out.linear_acceleration.z = imuData.accel.z();
            }
            
            if(imuData.gyroValid)
            {
                msg_out.angular_velocity.x  = imuData.gyro.x();
                msg_out.angular_velocity.y  = imuData.gyro.y();
                msg_out.angular_velocity.z  = imuData.gyro.z();
            }
            imu_pub.publish(msg_out);

            sensor_msgs::MagneticField compass_out;
            if(imuData.compassValid)
            {
                compass_out.magnetic_field.x  = imuData.compass.x();
                compass_out.magnetic_field.y  = imuData.compass.y();
                compass_out.magnetic_field.z  = imuData.compass.z();    
            }
            compass_pub.publish(compass_out);
            
        }
    }
}

