#ifndef SPEEDSETTER_H
#define SPEEDSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class SpeedSetter : public QWidget
{
    Q_OBJECT
public:
    explicit SpeedSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(int);

signals:

public slots:
};

#endif // SPEEDSETTER_H
