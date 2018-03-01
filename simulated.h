#ifndef SIMULATED_H
#define SIMULATED_H

#include <QDialog>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
class simulated;
}

class simulated : public QDialog
{
    Q_OBJECT

public:
    explicit simulated(QWidget *parent = 0);
    ~simulated();

private slots:
    void closeEvent (QCloseEvent *event);

private:
    Ui::simulated *ui;

    QMediaPlayer* simPlay;

    QVideoWidget* simvw;

};

#endif // SIMULATED_H
