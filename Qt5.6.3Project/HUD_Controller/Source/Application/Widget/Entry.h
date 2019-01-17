#ifndef ENTRY_H
#define ENTRY_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class Entry : public QWidget
{
    Q_OBJECT
public:
    explicit Entry(QWidget *parent = 0);

private slots:
    void onInertialNavigationSystemSelected(void);
    void onAirDataComputerSelected(void);
    void onAltitudeButtonSelected(void);

signals:

public slots:
};

#endif // ENTRY_H
