#include "ALT.h"

#include "AltitudeSetter.h"

#include <QVBoxLayout>

ALT::ALT(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainVerticalBoxLayout = new QVBoxLayout(this);


    mainVerticalBoxLayout->addWidget(new AltitudeSetter(this));

    setLayout(mainVerticalBoxLayout);

    this->dataTransmitterThread = new DataTransmitter(3,3);

    this->dataTransmitterThread->start();
    //启动线程

    //启动线程已经被移动到各个子界面中了

    //突然觉得能在Qt 5.6在2019年3.16结束支持之前再来一次是挺有意思的事情
}
