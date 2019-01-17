#ifndef ADC_H
#define ADC_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include "../Thread/DataTransmitter.h"

class ADC : public QWidget
{
    Q_OBJECT
public:
    explicit ADC(QWidget *parent = 0);

private:
    DataTransmitter *dataTransmitterThread;

signals:

public slots:
};

#endif // ADC_H
