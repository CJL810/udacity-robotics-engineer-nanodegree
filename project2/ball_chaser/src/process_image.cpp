#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!srvClient.call(srv)) {
        ROS_ERROR("Failed to call service to drive_bot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera


    // Loops thru to find white pixels (data == 255)
    int white_pixel_ht = -1, white_pixel_step = -1;

    for (int i = 0; i < img.height * img.step; i++)
    {
        if (img.data[i] == 255)
        {
            white_pixel_ht = i / img.step;
            white_pixel_step = i % img.step;
            break;
        }
        
    }

    // Moves depending on the white ball position
    float lin_x = 0.0, ang_z = 0.0;
    if (white_pixel_step <= img.step * 0.3 && white_pixel_step >= 0)
    {
        ang_z = 0.5;
    } else if (white_pixel_step > img.step * 0.7 && white_pixel_step <= img.step)
    {
        ang_z = -0.5;
    } else if (white_pixel_step != -1)
    {
        lin_x = 0.5;
    }

    // Calls drive_bot function and passes velocities to it
    drive_robot(lin_x, ang_z);


    // Publish info
    if (white_pixel_step != -1)
    {
        ROS_INFO_STREAM("Target spotted: moving to target..");
    } else
    {
        ROS_INFO_STREAM("Unable to spot target..");
    }
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}