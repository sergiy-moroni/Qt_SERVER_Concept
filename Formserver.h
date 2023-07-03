#ifndef FORMSERVER_H
#define FORMSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
namespace Ui {
class Formserver;
}

class Formserver : public QWidget, QTcpServer
{
    Q_OBJECT

public:
    explicit Formserver(QWidget *parent = nullptr);
    ~Formserver();

private:
    Ui::Formserver *ui;
};

#endif // FORMSERVER_H
