#ifndef INS_H
#define INS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include "../Thread/DataTransmitter.h"

class INS : public QWidget
{
    Q_OBJECT
public:
    explicit INS(QWidget *parent = 0);

private:
    DataTransmitter *dataTransmitterThread;

signals:

public slots:
};

#endif // INS_H
