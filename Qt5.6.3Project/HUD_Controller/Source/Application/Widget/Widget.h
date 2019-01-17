#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "../Thread/DataTransmitter.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    DataTransmitter *dataTransmitterThread;
};

#endif // WIDGET_H
