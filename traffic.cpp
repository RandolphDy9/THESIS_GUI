#include "traffic.h"
#include "ui_traffic.h"
#include "mainwindow.h"

traffic::traffic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::traffic)
{
    ui->setupUi(this);
    this->setWindowTitle("Traffic Updates");
}

traffic::~traffic()
{
    delete ui;
}
