#ifndef DATATRANSMITTER_H
#define DATATRANSMITTER_H

#include <QThread>

class DataTransmitter : public QThread
{
    Q_OBJECT
public:
    explicit DataTransmitter(quint8 address,quint8 subAddress);

private:
    quint8 address,subAddress;

    void loadData(quint8 address,unsigned short int *dataBuffer);

protected:
    void run();//用于启动线程

signals:

public slots:
};

#endif // DATATRANSMITTER_H
