#include "MachSetter.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>

#include "../ParameterStore.h"

MachSetter::MachSetter(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *horizontalBoxLayout = new QHBoxLayout(this);
    //这里的this指的是父类为自己
    //创建在堆中

    horizontalBoxLayout->addWidget(new QLabel("马赫数",this));
    QDoubleSpinBox *qtDoubleSpinBox = new QDoubleSpinBox(this);
    qtDoubleSpinBox->setMinimum(0.0);
    qtDoubleSpinBox->setMaximum(3.25);
    qtDoubleSpinBox->setValue(0);//初始值0

    horizontalBoxLayout->addWidget(qtDoubleSpinBox);
    setLayout(horizontalBoxLayout);


    connect(qtDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onValueChange(double)));
}


void MachSetter::onValueChange(double value)
{
    ParameterStore::getInstance()->setMach((float)value);
}
