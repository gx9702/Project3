#include <ros/ros.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <std_msgs/Int32.h>
#include <create_node/TurtlebotSensorState.h>

using namespace std;

const string IFNAME = "wlan0";

int batteryPercentage;

std_msgs::Int32 determineWifiStrength(){
    std_msgs::Int32 value;
    value.data = -1;

    ifstream input("/proc/net/wireless");
    if(!input)
    {
     	cout << "Couldn't open the file\n";
	return value;
    }

    string line;
    string word;
	
    while(getline(input, line)) {
        istringstream bp(line);	
        string fname;
        bp >> fname;
        if(fname == IFNAME + ':')
        {
            replace(line.begin(), line.end(), '.', ' ');
	    bp >> word; bp >> word;
            ROS_INFO_STREAM("Current WiFi Signal Strength: " << word << '\n');
            value.data = atoi(word.c_str());
 	    return value;    
 	}
    }

    return value;
}

void turtlebotSensorStateMessageReceived(create_node::TurtlebotSensorState sensorState){
	batteryPercentage = int(double(sensorState.charge)/sensorState.capacity * 100);	
}

int main(int argc, char** argv)
{
	ros::init(argc,argv, "wifi_signal_strength");
	ros::NodeHandle nh;

	ros::Publisher wifiPublisher = nh.advertise <std_msgs::Int32>("/wifi_ss",1000);

	ros::Subscriber batterySubscriber = nh.subscribe("/turtlebot_node/sensor_state", 1000, &turtlebotSensorStateMessageReceived);
	ros::Publisher batteryPublisher = nh.advertise <std_msgs::Int32>("/battery_percentage", 1000);
	
	ros::Rate rate(0.2);

	while(ros::ok()){

	    wifiPublisher.publish(determineWifiStrength());

	    std_msgs::Int32 battery;
	    battery.data = batteryPercentage;
	    batteryPublisher.publish(battery);

	    rate.sleep();
	    ros::spinOnce();
	}
}


