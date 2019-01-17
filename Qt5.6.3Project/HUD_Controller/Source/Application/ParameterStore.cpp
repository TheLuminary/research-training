#include "ParameterStore.h"

ParameterStore::ParameterStore(QWidget *parent) : QWidget(parent)
{
    this->pitch = 0;
    this->roll = 0;
    this->yaw = 0;
    this->height = 0;
    this->speed = 0;
    this->mach = 0;
    this->altitude = 0;
}

ParameterStore *ParameterStore::instance = NULL;

ParameterStore *ParameterStore::getInstance()
{
    if(instance == NULL) instance = new ParameterStore();
    return instance;
}

short int ParameterStore::getPitch()
{
    return (short int)((this->pitch/90)*30000);
}

void ParameterStore::setPitch(double pitch)
{
    pitch = pitch>-90.0?pitch:-90.0;
    pitch = pitch<90.0?pitch:90.0;
    this->pitch = pitch;
}

short int ParameterStore::getRoll()
{
    return (short int)((this->roll/180)*30000);
}

void ParameterStore::setRoll(double roll)
{
    while(roll>180) roll-=360.0;
    while(roll<-180) roll += 360.0;
    this->roll = roll;
}

short int ParameterStore::getYaw()
{
    return (short int)((this->yaw/180)*30000);
}

void ParameterStore::setYaw(double yaw)
{
    while(yaw>180) yaw-=360.0;
    while(yaw<-180) yaw+=360.0;
    this->yaw = yaw;
}

short int ParameterStore::getHeight()
{
    return this->height;
}

void ParameterStore::setHeight(short height)
{
    this->height = height;
}

short int ParameterStore::getSpeed()
{
    return this->speed;
}

void ParameterStore::setSpeed(short speed)
{
    this->speed = speed;
}

short int ParameterStore::getMach()
{
    return this->mach;
}

void ParameterStore::setMach(float mach)
{
    this->mach = (short int)(100*mach);
}

short int ParameterStore::getAltitude()
{
    return this->altitude;
}

void ParameterStore::setAltitude(short altitude)
{
    this->altitude = altitude;
}


