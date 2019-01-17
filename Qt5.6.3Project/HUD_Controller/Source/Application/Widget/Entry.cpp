#include "Entry.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>

#include "INS.h"
#include "ADC.h"
#include "ALT.h"

Entry::Entry(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *remoteTerminalSelector =
            new QVBoxLayout(this);

    QPushButton *inertialNavigationSystemSelectButton =
            new QPushButton("惯性导航系统(INS)",this);
    QPushButton *airDataComputerSelectButton =
            new QPushButton("大气数据计算机(ADC)",this);
    //QPushButton altSelectButton("ALT",this);
    QPushButton *altSelectButton =
            new QPushButton("无线电高度仪(ALT)",this);

    remoteTerminalSelector->addWidget(
                inertialNavigationSystemSelectButton);
    remoteTerminalSelector->addWidget(
                airDataComputerSelectButton);
    remoteTerminalSelector->addWidget(
                altSelectButton);


    this->setLayout(remoteTerminalSelector);

    connect(inertialNavigationSystemSelectButton,SIGNAL(clicked()),
            this,SLOT(onInertialNavigationSystemSelected()));
    connect(airDataComputerSelectButton,SIGNAL(clicked()),
            this,SLOT(onAirDataComputerSelected()));
    connect(altSelectButton,SIGNAL(clicked()),
            this,SLOT(onAltitudeButtonSelected()));
}

extern QApplication *applicationInstance;

void Entry::onInertialNavigationSystemSelected()
{
    this->close();
    //或者用hide也可以
    INS *ins = new INS();
    ins->show();
    //applicationInstance->processEvents();
}

void Entry::onAirDataComputerSelected()
{
    this->close();//用hide大概也可以把
    ADC *adc = new ADC();
    adc->show();
}

void Entry::onAltitudeButtonSelected()
{
    this->close();
    ALT *alt = new ALT();
    alt->show();
}
