#ifndef ALT_H
#define ALT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#include "../Thread/DataTransmitter.h"

class ALT : public QWidget
{
    Q_OBJECT
public:
    explicit ALT(QWidget *parent = 0);

private:
    DataTransmitter *dataTransmitterThread;

signals:

public slots:
};

#endif // ALT_H
