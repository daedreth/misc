#ifndef FORKER_H
#define FORKER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStringList>

namespace Ui {
class forker;
}

class forker : public QMainWindow
{
    Q_OBJECT

public:
    explicit forker(QStringList arguments,QWidget *parent = 0);
    ~forker();
    void start(QString address, quint16 port);
    QStringList argumentList;

public slots:
     void startTransfer();

private:
    Ui::forker *ui;
    QTcpSocket client;
};

#endif // FORKER_H
