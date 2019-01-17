#ifndef MACHSETTER_H
#define MACHSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class MachSetter : public QWidget
{
    Q_OBJECT
public:
    explicit MachSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(double);

signals:

public slots:
};

#endif // MACHSETTER_H
