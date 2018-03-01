#ifndef ROI_IMAGE_H
#define ROI_IMAGE_H

#include <QDialog>

namespace Ui {
class roi_image;
}

class roi_image : public QDialog
{
    Q_OBJECT

public:
    explicit roi_image(QWidget *parent = 0);
    ~roi_image();

private:
    Ui::roi_image *ui;
};

#endif // ROI_IMAGE_H
