#ifndef PANTILT_HPP
#define PANTILT_HPP

#include "ros/ros.h"
#include "actionlib/client/simple_action_client.h"
#include <pantilt_control/PtuGotoAction.h>



typedef actionlib::SimpleActionClient<pantilt_control::PtuGotoAction> ptuGotoClient;

class Pantilt
{

public:
    Pantilt(ros::NodeHandle *nH);
    ~Pantilt();
    void init(bool enabled);
    void set_pantilt_withOutWait(double pan, double tilt);
    bool set_pantilt(double pan, double tilt);

private:
    ptuGotoClient *ptuGoto_ac;
    pantilt_control::PtuGotoGoal ptuGotoGoal;
    ros::NodeHandle *n;
};

#endif
