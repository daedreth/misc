#include "ricebowl.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ricebowl w;

    QString arg = argv[1];

    if(arg.contains("-l"))
    {
        w.localMode = true;
        w.show(); // -l or --local for debugging purposes, will be removed &| reworked later
    }
    return a.exec();
}
