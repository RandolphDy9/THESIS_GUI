#include "roi_image.h"
#include "ui_roi_image.h"

roi_image::roi_image(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::roi_image)
{
    ui->setupUi(this);
    this->setWindowTitle("Region of Interest");

    ui->roi1->setStyleSheet("border-image: url(C:/Users/Randolph D/Pictures/thesisPics/roiv1.jpg) 0 0 0 0 stretch stretch;"
                             "border-width: 0px;");
    ui->roi2->setStyleSheet("border-image: url(C:/Users/Randolph D/Pictures/thesisPics/roiv2.jpg) 0 0 0 0 stretch stretch;"
                             "border-width: 0px;");
    ui->roi3->setStyleSheet("border-image: url(C:/Users/Randolph D/Pictures/thesisPics/roiv3.jpg) 0 0 0 0 stretch stretch;"
                             "border-width: 0px;");
    ui->roi4->setStyleSheet("border-image: url(C:/Users/Randolph D/Pictures/thesisPics/roiv4.jpg) 0 0 0 0 stretch stretch;"
                             "border-width: 0px;");
}

roi_image::~roi_image()
{
    delete ui;
}
