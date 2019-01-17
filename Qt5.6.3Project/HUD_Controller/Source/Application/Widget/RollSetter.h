#ifndef ROLLSETTER_H
#define ROLLSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class RollSetter : public QWidget
{
    Q_OBJECT
public:
    explicit RollSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(double);

signals:

public slots:
};

#endif // ROLLSETTER_H
