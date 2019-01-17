#include "YawSetter.h"

#include "../ParameterStore.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>

YawSetter::YawSetter(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *horizontalBoxLayout = new QHBoxLayout(this);
    //这里的this指的是父类为自己
    //创建在堆中

    horizontalBoxLayout->addWidget(new QLabel("航向",this));
    QDoubleSpinBox *qtDoubleSpinBox = new QDoubleSpinBox(this);
    qtDoubleSpinBox->setMinimum(-180);
    qtDoubleSpinBox->setMaximum(180);
    qtDoubleSpinBox->setValue(0);//初始值0

    horizontalBoxLayout->addWidget(qtDoubleSpinBox);
    this->setLayout(horizontalBoxLayout);


    connect(qtDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onValueChange(double)));
}

void YawSetter::onValueChange(double value)
{
    ParameterStore::getInstance()->setYaw(value);
}
