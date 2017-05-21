#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <unistd.h>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file(argv[1]);

    if(argc == 1)
    {
        QTextStream out(stdout);
        out << "\tNo file specified!\n";
        out << "\t\tUsage:\n";
        out << "\t\tqtprint /path/to/file\n";
        out.flush();
        std::exit(0);
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        QTextStream out(stdout);
        out << "\tUnable to open the file!\n";
        out << "\tEnsure you have permissions to read the file or that the file exists!\n";
        out.flush();
        std::exit(0);
    }

    QTextStream out(stdout);
    QString line = file.readAll();

    for(int i=0;line.length()-1;i++){
        out << line[i];
        out.flush();
        usleep(15000);
        if(line[i].isNull()){
            out << endl;
            std::exit(0);
        }
    }
        std::exit(0);
        return a.exec();
}
