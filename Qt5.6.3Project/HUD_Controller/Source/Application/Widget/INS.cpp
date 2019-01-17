#include "INS.h"

#include "PitchSetter.h"
#include "RollSetter.h"
#include "YawSetter.h"

#include <QVBoxLayout>

INS::INS(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainVerticalBoxLayout = new QVBoxLayout(this);


    mainVerticalBoxLayout->addWidget(new PitchSetter(this));
    mainVerticalBoxLayout->addWidget(new RollSetter(this));
    mainVerticalBoxLayout->addWidget(new YawSetter(this));

    setLayout(mainVerticalBoxLayout);

    this->dataTransmitterThread = new DataTransmitter(1,1);

    this->dataTransmitterThread->start();
    //启动线程

    //启动线程已经被移动到各个子界面中了

    //突然觉得能在Qt 5.6在2019年3.16结束支持之前再来一次是挺有意思的事情
}
