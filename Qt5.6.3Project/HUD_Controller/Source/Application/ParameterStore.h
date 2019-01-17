#ifndef PARAMETERSTORE_H
#define PARAMETERSTORE_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class ParameterStore : public QWidget
{
    Q_OBJECT
public:
    //explicit ParameterStore(QWidget *parent = 0);

    static ParameterStore *getInstance();

    //为了避免溢出，只使用带符号的数据类型

    void setPitch(double pitch);
    void setRoll(double roll);
    void setYaw(double yaw);
    short int getPitch();
    short int getRoll();
    short int getYaw();

    void setHeight(short int height);
    void setSpeed(short int speed);
    void setMach(float mach);
    short int getHeight();
    short int getSpeed();
    short int getMach();

    void setAltitude(short int altitude);
    short int getAltitude();

private:
    double pitch,roll,yaw;
    short int mach,height,speed;
    //在这里只保留需要发送的马赫数
    short int altitude;
private:
    ParameterStore(QWidget *parent = 0);
    static ParameterStore *instance;

signals:

public slots:
};

#endif // PARAMETERSTORE_H
