#include "forker.h"
#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    if(a.arguments().at(1) == "help"){
        QTextStream stream(stdout);
        stream << "\tUsage:\n\t\tforker <command>\n\n\tFull list of commands can be found in the README.md file.";
    }

    forker w(a.arguments());
    w.start("127.0.0.1", 6363);

    //w.show();

    return a.exec();
}
