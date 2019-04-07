#include "../include/robina_faceRecognition/pantilt.hpp"

Pantilt::Pantilt(ros::NodeHandle *nH):
    n(nH)
{
}

Pantilt::~Pantilt()
{
    delete ptuGoto_ac;
}

void Pantilt::init(bool enabled)
{
    ptuGoto_ac = new ptuGotoClient("SetPTUState",true);
    if(enabled)
        ptuGoto_ac->waitForServer();
    ROS_INFO(" initialize Pantilt action ");
}

void Pantilt::set_pantilt_withOutWait(double pan, double tilt)
{
    //mutex lock
    ptuGotoGoal.tilt = tilt;
    ptuGotoGoal.pan  = pan;
    ptuGoto_ac->sendGoal(ptuGotoGoal);

    //thread to wait{
    //ptuGoto_ac->waitForResult();
    //mutex unlock
    //}
}

bool Pantilt::set_pantilt(double pan, double tilt)
{
    ptuGotoGoal.tilt = tilt;
    ptuGotoGoal.pan  = pan;
    ptuGoto_ac->sendGoal(ptuGotoGoal);
    return ptuGoto_ac->waitForResult();
}

