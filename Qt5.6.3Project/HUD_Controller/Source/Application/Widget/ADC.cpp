#include "ADC.h"

#include "HeightSetter.h"
#include "SpeedSetter.h"
#include "MachSetter.h"

#include <QVBoxLayout>

ADC::ADC(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainVerticalBoxLayout = new QVBoxLayout(this);


    mainVerticalBoxLayout->addWidget(new HeightSetter(this));
    mainVerticalBoxLayout->addWidget(new SpeedSetter(this));
    mainVerticalBoxLayout->addWidget(new MachSetter(this));

    setLayout(mainVerticalBoxLayout);

    this->dataTransmitterThread = new DataTransmitter(2,2);

    this->dataTransmitterThread->start();
    //启动线程

    //启动线程已经被移动到各个子界面中了

    //突然觉得能在Qt 5.6在2019年3.16结束支持之前再来一次是挺有意思的事情

}
