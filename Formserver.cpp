#include "Formserver.h"
#include "ui_Formserver.h"

Formserver::Formserver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Formserver)
{
    ui->setupUi(this);
}

Formserver::~Formserver()
{
    delete ui;
}
