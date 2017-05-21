#include "forker.h"
#include "ui_forker.h"
#include <QHostAddress>
#include <iostream>
#include <unistd.h>
#include <QTextStream>

forker::forker(QStringList arguments, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::forker)
{
    ui->setupUi(this);
    argumentList = arguments;
    connect(&client, SIGNAL(connected()),this, SLOT(startTransfer()));
}

forker::~forker()
{
    delete ui;
}

void forker::start(QString address, quint16 port)
{
    QHostAddress addr(address);
    client.connectToHost(addr, port);
    client.waitForConnected(1000);
}

void forker::startTransfer()
{
    std::string str = argumentList.at(1).toStdString();
    const char* p = str.c_str();
    client.write(p, 13);
    client.waitForBytesWritten(20);
    std::exit(1);
}
