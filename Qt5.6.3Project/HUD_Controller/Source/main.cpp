#include "./Application/Widget/Entry.h"
#include <QApplication>

QApplication *applicationInstance = NULL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    applicationInstance = &a;
    //Widget w;
    //w.show();
    Entry entry;
    entry.show();

    return a.exec();
}
