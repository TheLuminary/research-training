#ifndef YAWSETTER_H
#define YAWSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class YawSetter : public QWidget
{
    Q_OBJECT
public:
    explicit YawSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(double);

signals:

public slots:
};

#endif // YAWSETTER_H
