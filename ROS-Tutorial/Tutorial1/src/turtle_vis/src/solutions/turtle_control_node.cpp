/*********************************************************************
* Compiler:         gcc 4.6.3
*
* Company:          Institute for Cognitive Systems
*                   Technical University of Munich
*
* Author:           Emmanuel Dean (dean@tum.de)
*                   Karinne Ramirez (karinne.ramirez@tum.de)
*
* Compatibility:    Ubuntu 12.04 64bit (ros hydro)
*
* Software Version: V0.1
*
* Created:          01.06.2015
*
* Comment:          turtle connection and visualization (Sensor and Signals)
*
********************************************************************/


/*********************************************************************
* STD INCLUDES
********************************************************************/
#include <iostream>
#include <fstream>
#include <pthread.h>


/*********************************************************************
* ROS INCLUDES
********************************************************************/
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <tf_conversions/tf_eigen.h>

/*********************************************************************
 * CUSTOM CLASS
 * ******************************************************************/
#include <turtle_vis/myClass/TurtleClass.h>


int main( int argc, char** argv )
{

    ros::init(argc, argv, "turtle_control",ros::init_options::AnonymousName);

    ROS_INFO_STREAM("**Publishing turtle control..");

    ros::NodeHandle n;
    ros::Rate r(60);



    //ADVERTISE THE SERVICE
    turtleSpace::TurtleClass turtleF;
    ros::ServiceServer service=n.advertiseService("TurtlePose",
                                                  &turtleSpace::TurtleClass::getDPose,
						  &turtleF);
    //CALL SERVICE FROM TERMINAL//
    //    rosservice call /TurtlePose '{p: [0.5, 0.0, 3.0]}'
    //    rosservice call /TurtlePose "{p: {x: 1.5, y: 1.0, theta: 0.0}}"
    //DON'T FORGET TO SOURCE THE WORKSPACE BEFORE CALLING THE SERVICE

    //ADVERTIZE THE TOPIC
    ros::Publisher pub=n.advertise<turtle_vis::DesiredPose>("turtle_control",100);

    ros::Time ti, tf;
    ti=ros::Time::now();

    //Proportional Gain
    Matrix3d Kp;


   	

    //#>>>>TODO: SET GAINS

    double p_g=0.0;


    //#>>>>TODO: LOAD p_gain FROM THE ROS PARAMETER SERVER 
    
    ros::param::get("/turtle_gains/p_gain",p_g);
    ROS_INFO_STREAM("p_g= "<<p_g); 


    //Proportional Gain

    Kp<<p_g,0  ,0,
            0  ,p_g,0,
            0  ,0  ,p_g;

    ROS_INFO_STREAM("Kp= \n"<<Kp);

    Vector3d turtlePose,turtlePose_old,turtleVel;
    Vector3d error;
    double dt;

    //INITIALIZE THE TURTLE POSE
    turtlePose<<1,0,0;
    turtlePose_old=turtlePose;
    turtleVel<<0,0,0;

    //DESIRED POSE
    Vector3d turtlePose_desired_local;

    turtleF.turtlePose_desired_g=turtlePose;
    turtlePose_desired_local = turtlePose;


    //CREATE A DESIREDPOSE MSG VARIABLE
    turtle_vis::DesiredPose msg; //#>>>>TODO:DEFINE THE MSG TYPE

    while(ros::ok())
    {
        tf=ros::Time::now();

        dt=tf.toSec()-ti.toSec();

        ////#>>>>TODO: Get Desired Pose from the class variable
        turtlePose_desired_local = turtleF.getLocalDesiredPose();

        //CONTROL
        ////#>>>>TODO:COMPUTE THE ERROR BETWEEN CURRENT POSE AND DESIRED
        error=turtlePose-turtlePose_desired_local ;
        // COMPUTE THE INCREMENTS
        turtleVel=error*dt;
	turtlePose += -Kp * turtleVel;
	msg.x = turtlePose[0];
	msg.y = turtlePose[1];
	msg.theta = turtlePose[2];
	pub.publish(msg);

	turtlePose_old = turtlePose;
        ti = tf;
        ros::spinOnce();

        r.sleep();
        

    }



}




















        




