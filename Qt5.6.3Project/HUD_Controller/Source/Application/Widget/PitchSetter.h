#ifndef PITCHSETTER_H
#define PITCHSETTER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class PitchSetter : public QWidget
{
    Q_OBJECT
public:
    explicit PitchSetter(QWidget *parent = 0);

private slots:
    //私有的槽
    void onValueChange(double);

signals:

public slots:
};

#endif // PITCHSETTER_H
