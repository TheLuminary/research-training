#include "PitchSetter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>

#include "../ParameterStore.h"

PitchSetter::PitchSetter(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *horizontalBoxLayout = new QHBoxLayout(this);
    //这里的this指的是父类为自己
    //创建在堆中

    horizontalBoxLayout->addWidget(new QLabel("俯仰",this));
    QDoubleSpinBox *qtDoubleSpinBox = new QDoubleSpinBox(this);
    qtDoubleSpinBox->setMinimum(-90);
    qtDoubleSpinBox->setMaximum(90);
    qtDoubleSpinBox->setValue(0);//初始值0

    horizontalBoxLayout->addWidget(qtDoubleSpinBox);
    setLayout(horizontalBoxLayout);


    connect(qtDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onValueChange(double)));
}


void PitchSetter::onValueChange(double value)
{
    ParameterStore::getInstance()->setPitch(value);
}
