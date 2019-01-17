#ifndef ALTITUDESETTER_H
#define ALTITUDESETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class AltitudeSetter : public QWidget
{
    Q_OBJECT
public:
    explicit AltitudeSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(int);

signals:

public slots:
};

#endif // ALTITUDESETTER_H
