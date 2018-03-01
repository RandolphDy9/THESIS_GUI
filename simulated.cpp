#include "simulated.h"
#include "ui_simulated.h"


simulated::simulated(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::simulated)
{
    ui->setupUi(this);
    this->setWindowTitle("Simulated Video 1");

    simPlay = new QMediaPlayer;
    simvw = new QVideoWidget;
    simPlay->setVideoOutput(simvw);

    simPlay->setMedia(QUrl::fromLocalFile("C:/Users/Randolph D/Videos/ako.mp4"));
    ui->gv_sim->setViewport(simvw);
    simPlay->play();

    // if window is closed, simPlay->stop();

}

simulated::~simulated()
{
    delete ui;
}

void simulated::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Simulated V1",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        event->accept();
        simPlay->stop();
    }
}
