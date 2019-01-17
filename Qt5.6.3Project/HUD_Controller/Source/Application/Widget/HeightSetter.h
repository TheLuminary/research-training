#ifndef HEIGHTSETTER_H
#define HEIGHTSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class HeightSetter : public QWidget
{
    Q_OBJECT
public:
    explicit HeightSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(int);

signals:

public slots:
};

#endif // HEIGHTSETTER_H
