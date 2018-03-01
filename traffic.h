#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <QDialog>

namespace Ui {
class traffic;
}

class traffic : public QDialog
{
    Q_OBJECT

public:
    explicit traffic(QWidget *parent = 0);
    ~traffic();

private:
    Ui::traffic *ui;
};

#endif // TRAFFIC_H
