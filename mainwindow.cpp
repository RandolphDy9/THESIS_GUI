#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Traffic UI");

    ui->stackedWidget->setCurrentIndex(2);
    hidden();

    // For Database

    mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("C:/Users/Randolph D/Traffic_GUI2/db/trafficdb.sqlite");

    tData = new QTimer(this);
    connect(tData, SIGNAL(timeout()), this, SLOT(timer_data()));
    //tData->start(10000); // 10 secondsz

    // LOGIN
    QPixmap picUser("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/police.png");
    ui->label->setPixmap(picUser.scaled(33,33,Qt::KeepAspectRatio));
    QPixmap picPass("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/password.png");
    ui->label_2->setPixmap(picPass.scaled(33,33,Qt::KeepAspectRatio));
    ui->lineEdit_user->setPlaceholderText("Enter Username");
    ui->lineEdit_pass->setPlaceholderText("Enter Password");

    // History Page
    QPixmap picTime("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/calendar.png");
    ui->datePic->setPixmap(picTime.scaled(41,41,Qt::KeepAspectRatio));
    QPixmap picDate("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/clock.png");
    ui->timePic->setPixmap(picDate.scaled(41,41,Qt::KeepAspectRatio));

    // Traffic Simulated Intersection
    QPixmap picUp("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/arrowup.png");
    ui->arrUp->setPixmap(picUp.scaled(50,50,Qt::KeepAspectRatio));
    QPixmap picDown("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/arrowdown.png");
    ui->arrDown->setPixmap(picDown.scaled(50,50,Qt::KeepAspectRatio));
    QPixmap picLeft("C:/Users/ERDT Research/Desktop/Traffic_GUI2/img/arrowleft.png");
    ui->arrLeft->setPixmap(picLeft.scaled(50,50,Qt::KeepAspectRatio));
    QPixmap picRight("C:/Users/Randolph D/Pictures/thesisPics/arrowright.png");
    ui->arrRight->setPixmap(picRight.scaled(50,50,Qt::KeepAspectRatio));

    // TIME
    time = new QTimer(this);
    connect(time, SIGNAL(timeout()), this, SLOT(showTime()));
    time->start(1000);

    // DATE
    ui->date->setText(QDate::currentDate().toString("dd/MM/yyyy"));

    timerValues();

    frame_count = 0;
    t_seconds   = 0;

    average_countl1[0] = 0;
    average_countl1[1] = 0;
    average_countl1[2] = 0;

    average_countl2[0] = 0;
    average_countl2[1] = 0;
    average_countl2[2] = 0;

    average_countl3[0] = 0;
    average_countl3[1] = 0;
    average_countl3[2] = 0;

    average_countl4[0] = 0;
    average_countl4[1] = 0;
    average_countl4[2] = 0;


    total_area_l1 = 0, total_area_l2 = 0,total_area_l3 = 0;
    night_flag = 0;
    toggleCount = false;
    cfg = {};
    qRegisterMetaType(&cfg);

    ui->listWidget->addItem(" ");
    ui->listWidget->addItem(" ");
    ui->listWidget->addItem(" ");
    ui->listWidget->addItem(" ");


    //ui->submitArea->hide();

    v1_area = DEFAULT_TOTAL_AREA_OA_CEBU;
    v2_area = DEFAULT_TOTAL_AREA_EO_BUS;
    v3_area = DEFAULT_TOTAL_AREA_EO_CDU;
    v4_area = DEFAULT_TOTAL_AREA_OA_PARKMALL;

    v1_areat = DEFAULT_TOTAL_AREA_PACIFIC_UNA;
    v2_areat = DEFAULT_TOTAL_AREA_PACIFIC_JPR;
    v3_areat = DEFAULT_TOTAL_AREA_PACIFIC_MCB;

    ui->v1_total_area->setValidator(new QIntValidator(0,99999,this));
    ui->v2_total_area->setValidator(new QIntValidator(0,99999,this));
    ui->v3_total_area->setValidator(new QIntValidator(0,99999,this));
    ui->v4_total_area->setValidator(new QIntValidator(0,99999,this));
    setup();

}
void MainWindow::addValues(QString id, QString rn, QString ar, int td, QString ts, QString eg) {
        QSqlQuery queryIn;
        queryIn.prepare("INSERT INTO traffic (ID, road_name, actual_roi_area, traffic_density, traffic_status,  estimated_go, date_time) "
                       "VALUES (:ID, :rName, :aRoi, :tDensity, :tStatus, :eGo, :dTime)");
        queryIn.bindValue(":ID", id);
        queryIn.bindValue(":rName", rn);
        queryIn.bindValue(":aRoi", ar);
        queryIn.bindValue(":tDensity", td);
        queryIn.bindValue(":tStatus", ts);
        queryIn.bindValue(":eGo", eg);
        queryIn.bindValue(":dTime", QDateTime::currentDateTime());
        if (!queryIn.exec()) {
            qDebug() << "Error adding values to database!";
        }
}
void MainWindow::timer_data() {

    if(mydb.open()) {
        qDebug() << "Database is open at this point!";
    }
    QString query = "CREATE TABLE traffic ("
                    "ID VARCHAR(5),"
                    "road_name VARCHAR(40),"
                    "actual_roi_area VARCHAR(40),"
                    "traffic_density integer,"
                    "traffic_status VARCHAR(40),"
                    "estimated_go VARCHAR(10),"
                    "date_time DATETIME);";
    QSqlQuery qry;
    qry.exec(query);

    if(!qry.exec(query)) {
        qDebug() << "Created database!";
    }

    if (click > 0) {
        if (ifBox == 1 && playing == 1) {
            if (cebu_low == 1) {
                addValues("V1", "Quano Ave. (Cebu Bound)", QString::number(v1_area), total_area_l1, "LOW", "19"); //cebu_low = 0;
            } else if (cebu_mod == 1) {
                addValues("V1", "Quano Ave. (Cebu Bound)", QString::number(v1_area), total_area_l1, "MODERATE", "27"); //cebu_mod = 0;
            } else if (cebu_high == 1) {
                addValues("V1", "Quano Ave. (Cebu Bound)", QString::number(v1_area), total_area_l1, "HEAVY", "51"); //cebu_high = 0;
            }
            if (bus_low == 1) {
                addValues("V2", "E.O. Perez (Terminal Bound)", QString::number(v2_area), total_area_l2, "LOW", "19"); //bus_low = 0;
            } else if (bus_mod == 1) {
                addValues("V2", "E.O. Perez (Terminal Bound)", QString::number(v2_area), total_area_l2, "MODERATE", "27"); //bus_mod = 0;
            } else if (bus_high == 1) {
                addValues("V2", "E.O. Perez (Terminal Bound)", QString::number(v2_area), total_area_l2, "HEAVY", "51"); //bus_high = 0;
            }
            if (cdu_low == 1) {
                addValues("V3", "E.O. Perez (CDU Bound)", QString::number(v3_area), total_area_l3, "LOW", "19"); //cdu_low = 0;
            } else if (cdu_mod == 1) {
                addValues("V3", "E.O. Perez (CDU Bound)", QString::number(v3_area), total_area_l3, "MODERATE", "27"); //cdu_mod = 0;
            } else if (cdu_high == 1) {
                addValues("V3", "E.O. Perez (CDU Bound)", QString::number(v3_area), total_area_l3, "HEAVY", "51"); //cdu_high = 0;
            }
            if (pmall_low == 1) {
                addValues("V4", "Quano Ave. (Parkmall Bound)", QString::number(v4_area), total_area_l4, "LOW", "19"); //pmall_low = 0;
            } else if (pmall_mod == 1) {
                addValues("V4", "Quano Ave. (Parkmall Bound)", QString::number(v4_area), total_area_l4, "MODERATE", "27"); //pmall_mod = 0;
            } else if (pmall_high == 1) {
                addValues("V4", "Quano Ave. (Parkmall Bound)", QString::number(v4_area), total_area_l4, "HIGH", "51"); //pmall_high = 0;
            }


        } else if (ifT == 1 && playing == 1) {
            if (mcb_low == 1) {
                addValues("V1", "M.C. Briones", QString::number(v1_areat), total_area_l1, "LOW", "19"); mcb_low = 0;
            } else if (mcb_mod == 1) {
                addValues("V1", "M.C. Briones", QString::number(v1_areat), total_area_l1, "MODERATE", "27"); mcb_mod = 0;
            } else if (mcb_high == 1) {
                addValues("V1", "M.C. Briones", QString::number(v1_areat), total_area_l1, "HEAVY", "51"); mcb_high = 0;
            }
            if (jpr_low == 1) {
                addValues("V2", "J.P. Rizal", QString::number(v2_areat), total_area_l2, "LOW", "19"); jpr_low = 0;
            } else if (jpr_mod == 1) {
                addValues("V2", "J.P. Rizal", QString::number(v2_areat), total_area_l2, "MODERATE", "27"); jpr_mod = 0;
            } else if (jpr_high == 1) {
                addValues("V2", "J.P. Rizal", QString::number(v2_areat), total_area_l2, "HEAVY", "51"); jpr_high = 0;
            }
            if (una_low == 1) {
                addValues("V3", "U.N. Avenue", QString::number(v3_areat), total_area_l3, "LOW", "19"); una_low = 0;
            } else if (una_mod == 1) {
                addValues("V3", "U.N. Avenue", QString::number(v3_areat), total_area_l3, "MODERATE", "27"); una_mod = 0;
            } else if (una_high == 1) {
                addValues("V3", "U.N. Avenue", QString::number(v3_areat), total_area_l3, "HEAVY", "51"); una_high = 0;
            }

        }
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QSqlTableModel *model = new QSqlTableModel(this,mydb);
        model->setTable("traffic");
        model->select();
        ui->tableView->setModel(model);

        ui->tableView->scrollToBottom();

        ui->tableView->show();

    }

}



void MainWindow::showTime()
{
    QTime times = (QTime::currentTime());
    QString currentTime = times.toString("hh:mm:ss");
    ui->time->setText(currentTime);
}

void MainWindow::exportTableViewToCSV(QTableView *table) {
    QString filters("CSV files (*.csv);All files (*.*)");
    QString defaultFilter("CSV files (*.csv)");
    QString fileName = QFileDialog::getSaveFileName(0, "Save Database File", QCoreApplication::applicationDirPath(),
                       filters, &defaultFilter);
    QFile file(fileName);

    QAbstractItemModel *model =  table->model();
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream data(&file);
        QStringList strList;
        for (int i = 0; i < model->columnCount(); i++) {
            if (model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().length() > 0)
                strList.append("\"" + model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() + "\"");
            else
                strList.append("");
        }
        data << strList.join(";") << "\n";
        for (int i = 0; i < model->rowCount(); i++) {
            strList.clear();
            for (int j = 0; j < model->columnCount(); j++) {

                if (model->data(model->index(i, j)).toString().length() > 0)
                    strList.append("\"" + model->data(model->index(i, j)).toString() + "\"");
                else
                    strList.append("");
            }
            data << strList.join(";") + "\n";
        }
        data << strList.join(";") << "\n";
        file.close();
    }

}

// ==================== first seq
void MainWindow::timer_timeoutgo1() {   // CHANGED HERE (TANAN FUNCTION SA TIMERS -- TAMAN UBOS)
    ui->go1->display(cGo);
    if (cGo >= 1) {
        ui->labelv1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrDown->show(); ui->arrLeft->hide(); ui->arrRight->hide();  ui->arrUp->hide();
        ui->down1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->down2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->down3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        brun1 = 1;
    } else {
        brun1 = 0;
        ui->down1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");

        if (classifierSelect == 0) {
            cGo = 51;
        }

        timer->stop();
        timerw->start(1000);
        ui->labelv1->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    cGo--;
}

void MainWindow::timer_timewait() {
    if (wait >= 0) {
        ui->wait1->display(wait);
        ui->labelv1->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrUp->hide();
            if (wait == 4) {
                cStop2 = 4;
            }
            ui->stop2->display(wait);
            brunw1 = 1;
    } else {
        brunw1 = 0;
            wait = 5;
            timerw->stop();
            ui->stop2->display(0);

            if (classifierSelect == 0) {

                timers->start(1000);
            } else {
                if (bus_low == 1) {
                    ui->est_go2->display(19); cGo2 = 19; timer2->start(1000);
                } else if (bus_mod == 1) {
                    ui->est_go2->display(27); cGo2 = 27; timer2->start(1000);
                } else if (bus_high == 1) {
                    ui->est_go2->display(51); cGo2 = 51; timer2->start(1000);
                }

                ui->est_go2->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
                ui->est_go1->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
                ui->est_go1->display(0);
                timers->stop(); timers2->stop(); timers3->stop(); timers4->stop();
                ui->stop1->display(199); ui->stop3->display(199); ui->stop4->display(199);
            }

            ui->labelv1->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    wait--;
}

void MainWindow::timer_timestop1() {
    ui->stop1->display(cStop);
    if (cStop == 4 && classifierSelect == 1) {
        if (cebu_low == 1) {
            ui->est_go1->display(19); cGo = 19;
        } else if (cebu_mod == 1) {
            ui->est_go1->display(27); cGo = 27;
        } else if (cebu_high == 1) {
            ui->est_go1->display(51); cGo = 51;
        }

    }
    if (cStop >= 1) {
        ui->labelv1->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    } else {
        if (classifierSelect == 0) {
            cStop = 105;
        }
            timers->stop(); timer->start(1000);
            ui->labelv1->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    cStop--;
}

// ========================== second seq
void MainWindow::timer_timeoutgo2() {
    ui->go2->display(cGo2);
    if (cGo2 >= 1) {
        ui->labelv2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrLeft->show(); ui->arrDown->hide(); ui->arrRight->hide();  ui->arrUp->hide();
        ui->left1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left4->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        brun2 = 1;
    } else {
        brun2 = 0;
        ui->left1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left4->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;color: white;}");

        if (classifierSelect == 0) {
            cGo2 = 19;
        }

        timer2->stop();
        timerw2->start(1000);
        ui->labelv2->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    cGo2--;
}

void MainWindow::timer_timewait2() {
    if (wait2 >= 0) {
        ui->wait2->display(wait2);
        ui->labelv2->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrLeft->hide();
        if (wait2 == 4) {
            cStop3 = 4;
        }
        ui->stop3->display(wait2);
        brunw2 = 1;
    } else {
        brunw2 = 0;
            wait2 = 5;
            timerw2->stop();
            ui->stop3->display(0);

            if (classifierSelect == 0) {

                timers2->start(1000);
            } else {
                if (cdu_low == 1) {
                    ui->est_go3->display(19); cGo3 = 19; timer3->start(1000);
                } else if (cdu_mod == 1) {
                    ui->est_go3->display(27); cGo3 = 27; timer3->start(1000);
                } else if (cdu_high == 1) {
                    ui->est_go3->display(51); cGo3 = 51; timer3->start(1000);
                }
                ui->est_go3->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
                ui->est_go2->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
                ui->est_go2->display(0);
                timers2->stop(); timers->stop(); timers3->stop(); timers4->stop();
                ui->stop2->display(199); ui->stop1->display(199); ui->stop4->display(199);

            }
            ui->labelv2->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    wait2--;
}

void MainWindow::timer_timestop2() {
    ui->stop2->display(cStop2);
    if (cStop2 == 4 && classifierSelect == 1) {
        if (bus_low == 1) {
            ui->est_go2->display(19); cGo2 = 19;
        } else if (bus_mod == 1) {
            ui->est_go2->display(27); cGo2 = 27;
        } else if (bus_high == 1) {
            ui->est_go2->display(51); cGo2 = 51;
        }

    }
    if (cStop2 >= 1) {
        ui->labelv2->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
        brun2 = 0;
    } else {
        if (classifierSelect == 0) {
            cStop2 = 137;
        }
            timers2->stop(); timer2->start(1000);
            ui->labelv2->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    cStop2--;
}

// ========================= third seq
void MainWindow::timer_timeoutgo3() {
    ui->go3->display(cGo3);
    if (cGo3 >= 1) {
        ui->labelv3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrRight->show(); ui->arrLeft->hide(); ui->arrDown->hide();  ui->arrUp->hide();
        ui->right1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->right2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->right3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->right4->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        brun3 = 1;
    } else {
        brun3 = 0;
        ui->right1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right4->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;color: white;}");

        if (classifierSelect == 0) {
            cGo3 = 27;
        }

        timer3->stop();
        timerw3->start(1000);
        ui->labelv3->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    cGo3--;
}

void MainWindow::timer_timewait3() {
    if (wait3 >= 0) {
        ui->wait3->display(wait3);
        ui->labelv3->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrRight->hide();
            if (wait3 == 4) {
                cStop4 = 4;
            }
            ui->stop4->display(wait3);
            brunw3 = 1;
    } else {
        brunw3 = 0;
            wait3 = 5;
            timerw3->stop();
            ui->stop4->display(0);

            if (classifierSelect == 0) {

                timers3->start(1000);
            } else {
                if (pmall_low == 1) {
                    ui->est_go4->display(19); cGo4 = 19; timer4->start(1000);
                } else if (pmall_mod == 1) {
                    ui->est_go4->display(27); cGo4 = 27; timer4->start(1000);
                } else if (pmall_high == 1) {
                    ui->est_go4->display(51); cGo4 = 51; timer4->start(1000);
                }
                ui->est_go4->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
                ui->est_go3->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
                ui->est_go3->display(0);
                timers3->stop(); timers2->stop(); timers->stop(); timers4->stop();
                ui->stop3->display(199); ui->stop2->display(199); ui->stop1->display(199);


            }
            ui->labelv3->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    wait3--;
}

void MainWindow::timer_timestop3() {
    ui->stop3->display(cStop3);
    if (cStop3 == 4 && classifierSelect == 1) {
        if (cdu_low == 1) {
            ui->est_go3->display(19); cGo3 = 19;
        } else if (cdu_mod == 1) {
            ui->est_go3->display(27); cGo3 = 27;
        } else if (cdu_high == 1) {
            ui->est_go3->display(51); cGo3 = 51;
        }

    }
    if (cStop3 >= 1) {
        ui->labelv3->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    } else {
            if (classifierSelect == 0) {
                cStop3 = 129;
            }

            timers3->stop(); timer3->start(1000);
            ui->labelv3->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    cStop3--;
}

// ========================== fourth seq
void MainWindow::timer_timeoutgo4() {
    ui->go4->display(cGo4);
    if (cGo4 >= 1) {
        ui->labelv4->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrUp->show(); ui->arrLeft->hide(); ui->arrRight->hide();  ui->arrDown->hide();
        ui->up1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->up2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->up3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        brun4 = 1;
    } else {
        brun4 = 0;
        ui->up1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->up2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->up3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;color: white;}");

        if (classifierSelect == 0) {
            cGo4 = 43;
        }


        timer4->stop();
        timerw4->start(1000);
        ui->labelv4->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    cGo4--;
}

void MainWindow::timer_timewait4() {
    if (wait4 >= 0) {
        ui->wait4->display(wait4);
        ui->labelv4->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrDown->hide();
            if (wait4 == 4) {
                cStop = 4;
            }
            ui->stop1->display(wait4);
            brunw4 = 1;
    } else {
        brunw4 = 0;
            wait4 = 5;
            timerw4->stop();
            ui->stop1->display(0);

            if (classifierSelect == 0) {

                timers4->start(1000);
            } else {
                if (cebu_low == 1) {
                    ui->est_go1->display(19); cGo = 19; timer->start(1000);
                } else if (cebu_mod == 1) {
                    ui->est_go1->display(27); cGo = 27; timer->start(1000);
                } else if (cebu_high == 1) {
                    ui->est_go1->display(51); cGo = 51; timer->start(1000);
                }
                ui->est_go1->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
                ui->est_go4->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
                ui->est_go4->display(0);
                timers4->stop(); timers3->stop(); timers2->stop(); timers->stop();
                ui->stop4->display(199); ui->stop3->display(199); ui->stop2->display(199);
            }
            ui->labelv4->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    wait4--;
}

void MainWindow::timer_timestop4() {
    ui->stop4->display(cStop4);
    if (cStop4 == 4 && classifierSelect == 1) {
        if (pmall_low == 1) {
            ui->est_go4->display(19); cGo4 = 19;
        } else if (pmall_mod == 1) {
            ui->est_go4->display(27); cGo4 = 27;
        } else if (pmall_high == 1) {
            ui->est_go4->display(51); cGo4 = 51;
        }

    }
    if (cStop4 >= 1) {
        ui->labelv4->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    } else {
           if (classifierSelect == 0) {
               cStop4 = 113;
           }

            timers4->stop(); timer4->start(1000);
            ui->labelv4->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    cStop4--;
}


// --------------------------------------------------------------------------- T - JUNC 1
void MainWindow::timer_ttimego() {
    ui->go1_t->display(tGo1);
    if (tGo1 >= 1) {
        ui->labelv1_t->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrDown->show(); ui->arrLeft->hide(); ui->arrRight->hide(); ui->arrUp->hide();
        trun1 = 1;
        ui->down1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->down2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->down3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
    } else {
        trun1 = 0;
        ui->down1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");

        if (classifierSelect == 0) {
            tGo1 = 30;
        }

        ttimer->stop();
        ttimerw->start(1000);
        ui->labelv1_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    tGo1--;
}

void MainWindow::timer_ttimewait() {
    if (twait1 >= 0) {
        ui->wait1_t->display(twait1);
        ui->labelv1_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrDown->hide();
        trunw1 = 1;
        if (twait1 == 4) {
            tStop2 = 4;
        }
        ui->stop2_t->display(twait1);
    } else {
        trunw1 = 0;
        twait1 = 5;
        ttimerw->stop();
        ui->stop2_t->display(0);

        if (classifierSelect == 0) {
            tStop1 = 70;
            tGo2 = 30; ttimers->start(1000);
        } else {
            if (jpr_low == 1) {
                ui->est_go2->display(19); tGo2 = 19; ttimer2->start(1000);
            } else if (jpr_mod == 1) {
                ui->est_go2->display(27); tGo2 = 27; ttimer2->start(1000);
            } else if (jpr_high == 1) {
                ui->est_go2->display(51); tGo2 = 51; ttimer2->start(1000);
            }
            ui->est_go2->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
            ui->est_go1->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
            ui->est_go1->display(0);
            ui->stop1_t->display(199); ui->stop3_t->display(199);
            ttimers3->stop(); ttimers2->stop(); ttimers->stop();
        }

        ui->labelv1_t->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    twait1--;
}

void MainWindow::timer_ttimestop() {
    ui->stop1_t->display(tStop1);
    if (tStop1 == 4) {
        if (una_low == 1) {
            ui->est_go1->display(19); tGo1 = 19;
        } else if (una_mod == 1) {
            ui->est_go1->display(27); tGo1 = 27;
        } else if (una_high == 1) {
            ui->est_go1->display(51); tGo1 = 51;
        }
    }
    if (tStop1 >= 1) {
        ui->labelv1_t->setStyleSheet("QLabel {background-color: #dd3c3c; border-radius: 5px;}");
    } else {
        if (classifierSelect == 0) {
            tStop1 = 70;
        }
        //tStop1 = 70;
        ttimers->stop(); ttimer->start(1000);
        ui->labelv1_t->setStyleSheet("QLabel {background-color: #333; border-radius: 5px;}");
    }
    tStop1--;
}

// ---------------------------------------------------------------------- T - JUNC 2
void MainWindow::timer_ttimego2() {
    ui->go2_t->display(tGo2);
    if (tGo2 >= 1) {
        ui->labelv2_t->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrLeft->show(); ui->arrUp->hide(); ui->arrRight->hide(); ui->arrRight->hide();
        ui->left1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        ui->left4->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        trun2 = 1;
    } else {
        trun2 = 0;
        ui->left1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left4->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");

        if (classifierSelect == 0) {
            tGo2 = 30;
        }

        //tGo2 = 30;
        ttimer2->stop();
        ttimerw2->start(1000);
        ui->labelv2_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    tGo2--;
}

void MainWindow::timer_ttimewait2() {
    if (twait2 >= 0) {
        ui->wait2_t->display(twait2);
        ui->labelv2_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrLeft->hide();
        if (twait2 == 4) {
            tStop3 = 4;
        }
        ui->stop3_t->display(twait2);
        trunw2 = 1;
    } else {
        trunw2 = 0;
        twait2 = 5;
        ttimerw2->stop();
        ui->stop3_t->display(0);

        if (classifierSelect == 0) {
            tStop2 = 70;
            tGo3 = 30; ttimers2->start(1000); //ttimer3->start(1000);
        } else {
            if (mcb_low == 1) {
                ui->est_go3->display(19); tGo3 = 19; ttimer3->start(1000);
            } else if (mcb_mod == 1) {
                ui->est_go3->display(27); tGo3 = 27; ttimer3->start(1000);
            } else if (mcb_high == 1) {
                ui->est_go3->display(51); tGo3 = 51; ttimer3->start(1000);
            }
            ui->est_go3->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
            ui->est_go2->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
            ui->est_go2->display(0);
            ui->stop1_t->display(199); ui->stop2_t->display(199);
            ttimers3->stop(); ttimers2->stop(); ttimers->stop();
        }

        ui->labelv2_t->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    twait2--;
}

void MainWindow::timer_ttimestop2() {
    ui->stop2_t->display(tStop2);
    if (tStop2 == 4) {
        if (jpr_low == 1) {
            ui->est_go2->display(19); tGo2 = 19;
        } else if (jpr_mod == 1) {
            ui->est_go2->display(27); tGo2 = 27;
        } else if (jpr_high == 1) {
            ui->est_go2->display(51); tGo2 = 51;
        }
    }
    if (tStop2 >= 1) {
        ui->labelv2_t->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    } else {
        if (classifierSelect == 0) {
            tStop2 = 36;
        }
        //tStop2 = 36;
        ttimers2->stop(); ttimer2->start(1000);
        ui->labelv2_t->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    tStop2--;
}

// -------------------------------------------------------------------------- T - JUNC 3
void MainWindow::timer_ttimego3() {
    ui->go3_t->display(tGo3);
    if (tGo3 >= 1) {
        ui->labelv3_t->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;color: white;}");
        ui->arrUp->show(); ui->arrRight->hide(); ui->arrLeft->hide(); ui->arrDown->hide();
//        ui->down1->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
//        ui->down2->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        //ui->up3->setStyleSheet("QLabel {background-color: #34cb5a;border-radius: 5px;}");
        trun3 = 1;
    } else {
        trun3 = 0;
//        ui->down1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
//        ui->down2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        //ui->up3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");

        if (classifierSelect == 0) {
            tGo3 = 30;
        }

        //tGo3 = 30;
        ttimer3->stop(); ttimerw3->start(1000);
        ui->labelv3_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
    }
    tGo3--;
}

void MainWindow::timer_ttimewait3() {
    if (twait3 >= 0) {
        ui->wait3_t->display(twait3);
        ui->labelv3_t->setStyleSheet("QLabel {background-color: #eb8947;border-radius: 5px;}");
        ui->arrUp->hide();
        if (twait3 == 4) {
            tStop1 = 4;
        }
        ui->stop1_t->display(twait3);
        trunw3 = 1;
    } else {
        trunw3 = 0;
        twait3 = 5;
        ttimerw3->stop();
        ui->stop1_t->display(0);

        if (classifierSelect == 0) {
            tStop3 = 70;
            tGo1 = 30; ttimers3->start(1000);
        } else {
            if (una_low == 1) {
                ui->est_go1->display(19); tGo1 = 19; ttimer->start(1000);
            } else if (una_mod == 1) {
                ui->est_go1->display(27); tGo1 = 27; ttimer->start(1000);
            } else if (una_high == 1) {
                ui->est_go1->display(51); tGo1 = 51; ttimer->start(1000);
            }
            ui->est_go1->setStyleSheet("QLCDNumber {background-color: #49d06a;border-radius: 5px;border: 1px solid #222;color: white;}");
            ui->est_go3->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
            ui->est_go3->display(0);
            ui->stop2_t->display(199); ui->stop3_t->display(199);
            ttimers3->stop(); ttimers2->stop(); ttimers->stop();
        }

        ui->labelv3_t->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    }
    twait3--;
}

void MainWindow::timer_ttimestop3() {
    ui->stop3_t->display(tStop3);
    if (tStop3 == 4) {
        if (mcb_low == 1) {
            ui->est_go3->display(19); tGo3 = 19;
        } else if (mcb_mod == 1) {
            ui->est_go3->display(27); tGo3 = 27;
        } else if (mcb_high == 1) {
            ui->est_go3->display(51); tGo3 = 51;
        }

    }
    if (tStop3 >= 1) {
        ui->labelv3_t->setStyleSheet("QLabel {background-color: #dd3c3c;border-radius: 5px;}");
    } else {
        if (classifierSelect == 0) {
            tStop3 = 70;
        }
        //tStop3 = 70;
        ttimers3->stop(); ttimer3->start(1000);
        ui->labelv3_t->setStyleSheet("QLabel {background-color: #333;border-radius: 5px;}");
    }
    tStop3--;
}

// ----------------------------------- COPY PASTE ENDS HERE!  ----------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    QWidget::mouseDoubleClickEvent(event);
    if (isFullScreen()) {
        this->setWindowState(Qt::WindowMaximized);
    } else {
        this->setWindowState(Qt::WindowFullScreen);
    }
}

void MainWindow::on_manualBtn_clicked()
{

//    ui->stackedWidget->setCurrentIndex(3);
//    theTraffic = new traffic(this);
//    theTraffic->show();
//   ui->popup->show();

//    if (pauseTrig == 1) {

//        if (ifBox == 1) {
//            ui->listWidget->item(0)->setText(QString("SNR (CEBU):     "));
//            ui->listWidget->item(1)->setText(QString("SNR (PMALL):    "));
//            ui->listWidget->item(2)->setText(QString("SNR (CDU):      "));
//            ui->listWidget->item(3)->setText(QString("SNR (TERMINAL): "));
//        } else if (ifT == 1) {
//            ui->listWidget->item(0)->setText(QString("PACIFIC (MCB):    "));
//            ui->listWidget->item(1)->setText(QString("PACIFIC (JPR):    "));
//            ui->listWidget->item(2)->setText(QString("PACIFIC (UNA):    "));
//        }

//        checkStat = 1;
//    } else {
//        QMessageBox::warning(this, "OOPS!", "Press pause button first!");
//    }

}

void MainWindow::on_toHistory_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWindow::on_toMain_clicked()
{
    ui->stackedWidget->setCurrentIndex(1); //ui->automaticModeLabel->show();
}

void MainWindow::on_toMain_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1); //ui->automaticModeLabel->show();
}


void MainWindow::on_login_clicked()
{
    QString username = ui->lineEdit_user->text();
    QString password = ui->lineEdit_pass->text();

    if (username == "admin" && password == "12345") {
        ui->stackedWidget->setCurrentIndex(1);
        ui->lineEdit_pass->clear(); ui->lineEdit_user->clear(); ui->mainToolBar->show();
    } else {
        QMessageBox::warning(this, "Login", "Username or passwaord is not correct!");
    }
}


void MainWindow::on_actionClose_Program_2_triggered()
{
    on_actionStop_triggered();

    QMessageBox::information(this, "Reminder!", "Save Datas to Database.");
    exportTableViewToCSV(ui->tableView);

    QString query = "DROP TABLE traffic";

    QSqlQuery qry;
    if(!qry.exec(query)) {
        qDebug() << "Tablr is dropped!";
    }

    mydb.close();

    QFile file("C:/Users/ERDT Research/Desktop/Traffic_GUI2/db/trafficdb.sqlite");
    file.remove();

    close();
}

void MainWindow::on_actionTraffic_History_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_closeBTN_clicked()
{
    close();
}

// All hidden features
void MainWindow::hidden(){
    ui->mainToolBar->hide(); //ui->goTimeBox->hide();
    //ui->automaticModeLabel->hide();
    ui->interOption->hide(); //ui->groupBox_2->hide();
    //ui->manualModeLabel->hide(); ui->automaticBtn->hide();ui->manualModeLabel_2->hide();
    //ui->gov1->hide(); ui->gov2->hide(); ui->gov3->hide(); ui->gov4->hide();
    ui->arrDown->hide(); ui->arrUp->hide(); ui->arrLeft->hide(); ui->arrRight->hide();
    ui->go1_t->hide(); ui->go2_t->hide(); ui->go3_t->hide();
    ui->labelv1_t->hide(); ui->labelv2_t->hide(); ui->labelv3_t->hide();
    ui->labelv1_emp->show(); ui->labelv2_emp->show(); ui->labelv3_emp->show(); ui->labelv4_emp->show();
    ui->src1->hide();ui->src2->hide();ui->src3->hide();ui->src4->hide();
    ui->roi1->hide();ui->roi2->hide();ui->roi3->hide();ui->roi4->hide();
    ui->classifier->hide();
    ui->countFW->hide();    ui->countMT->hide();    ui->countTR->hide();
    ui->countFW_2->hide();  ui->countMT_2->hide();  ui->countTR_2->hide();
    ui->countFW_3->hide();  ui->countMT_3->hide();  ui->countTR_3->hide();
    ui->countFW_4->hide();  ui->countMT_4->hide();  ui->countTR_4->hide();
    ui->tjunc_cover->hide();
    //ui->tjunc_cover->hide(); ui->tjunc_pacmall->hide();
            // THIS IS AN UPDATE
    ui->leg_name1->hide(); ui->leg_name2->hide(); ui->leg_name3->hide();    // THIS IS AN UPDATE
}

void MainWindow::timerValuesT() {
    tGo1 = 30; tGo2 = 30; tGo3 = 30;
    twait1 = 4; twait2 = 4; twait3 = 4;
    tStop1 = 70; tStop2 = 36; tStop3 = 70;
//    tStop1 = 70; tStop2 = 55; tStop3 = 110;
}

void MainWindow::timerValues() {

    wait = 4; wait2 = 4; wait3 = 4; wait4 = 4;
    cGo = 51; cGo2 = 19; cGo3 = 27; cGo4 = 43;
    cStop = 105; cStop2 = 58; cStop3 = 81; cStop4 = 112;
//    cStop = 105; cStop2 = 110; cStop3 = 160; cStop4 = 199;
}

void MainWindow::timerOperation() {

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeoutgo1()));

    timerw = new QTimer(this);
    connect(timerw, SIGNAL(timeout()), this, SLOT(timer_timewait()));

    timers = new QTimer(this);
    connect(timers, SIGNAL(timeout()), this, SLOT(timer_timestop1()));

    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(timer_timeoutgo2()));

    timerw2 = new QTimer(this);
    connect(timerw2, SIGNAL(timeout()), this, SLOT(timer_timewait2()));

    timers2 = new QTimer(this);
    connect(timers2, SIGNAL(timeout()), this, SLOT(timer_timestop2()));

    timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(timer_timeoutgo3()));

    timerw3 = new QTimer(this);
    connect(timerw3, SIGNAL(timeout()), this, SLOT(timer_timewait3()));

    timers3 = new QTimer(this);
    connect(timers3, SIGNAL(timeout()), this, SLOT(timer_timestop3()));

    timer4 = new QTimer(this);
    connect(timer4, SIGNAL(timeout()), this, SLOT(timer_timeoutgo4()));

    timerw4 = new QTimer(this);
    connect(timerw4, SIGNAL(timeout()), this, SLOT(timer_timewait4()));

    timers4 = new QTimer(this);
    connect(timers4, SIGNAL(timeout()), this, SLOT(timer_timestop4()));
}

void MainWindow::timerOperationT() {
    ttimer = new QTimer(this);
    connect(ttimer, SIGNAL(timeout()), this, SLOT(timer_ttimego()));

    ttimer2 = new QTimer(this);
    connect(ttimer2, SIGNAL(timeout()), this, SLOT(timer_ttimego2()));

    ttimer3 = new QTimer(this);
    connect(ttimer3, SIGNAL(timeout()), this, SLOT(timer_ttimego3()));

    ttimers = new QTimer(this);
    connect(ttimers, SIGNAL(timeout()), this, SLOT(timer_ttimestop()));

    ttimers2 = new QTimer(this);
    connect(ttimers2, SIGNAL(timeout()), this, SLOT(timer_ttimestop2()));

    ttimers3 = new QTimer(this);
    connect(ttimers3, SIGNAL(timeout()), this, SLOT(timer_ttimestop3()));

    ttimerw = new QTimer(this);
    connect(ttimerw, SIGNAL(timeout()), this, SLOT(timer_ttimewait()));

    ttimerw2 = new QTimer(this);
    connect(ttimerw2, SIGNAL(timeout()), this, SLOT(timer_ttimewait2()));

    ttimerw3 = new QTimer(this);
    connect(ttimerw3, SIGNAL(timeout()), this, SLOT(timer_ttimewait3()));
}

void MainWindow::stopAllT() {
    ttimers->stop(); ui->stop1_t->display(0);
    ttimers2->stop(); ui->stop2_t->display(0);
    ttimers3->stop(); ui->stop3_t->display(0);
    ttimer->stop(); ui->go1_t->display(0);
    ttimer2->stop(); ui->go2_t->display(0);
    ttimer3->stop(); ui->go3_t->display(0);
    ttimerw->stop(); ui->wait1_t->display(0); twait1 = 4;
    ttimerw2->stop(); ui->wait2_t->display(0); twait2 = 4;
    ttimerw3->stop(); ui->wait3_t->display(0);  twait3 = 4;
}

void MainWindow::stopAllB() {
    timer->stop(); ui->go1->display(0); timer2->stop(); ui->go2->display(0);
    timer3->stop(); ui->go3->display(0); timer4->stop(); ui->go4->display(0);
    timerw->stop(); ui->wait1->display(0); timerw2->stop(); ui->wait2->display(0); wait = 4; wait2 = 4;
    timerw3->stop(); ui->wait3->display(0); timerw4->stop(); ui->wait4->display(0); wait3 = 4; wait4 = 4;
    timers->stop(); ui->stop1->display(0); timers2->stop(); ui->stop2->display(0);
    timers3->stop(); ui->stop3->display(0); timers4->stop(); ui->stop4->display(0);
}

void MainWindow::on_actionPlay_triggered()  // CHANGED HERE
{

    if (click == 0) {
        QMessageBox::information(this, "OOPS! Nothing to play yet.", "Please select a desired intersection and open video files.");
    } else if (playing == 1) {
        QMessageBox::information(this, "OOPS! It is already playing.", "Please press the Stop button.");
    } else if (srcTrig == 0) {
        QMessageBox::information(this, "OOPS! Videos must be selected.", "Start with V1.");
    } else {

        tData->start(5000);

        ui->arrDown->hide(); ui->arrLeft->hide(); ui->arrRight->hide(); ui->arrUp->hide();

        sendSetup();
        ui->statusBar->showMessage("Playing...");
        emit sendToggleStream(true);

        if (ifBox == 1 && click > 0) { // if select button is pressed

//            if (vidCount == 0) { // should be 4

//                if (stopTrig == 1) {    // FOR PAUSE
//                    timer->start(1000); timers2->start(1000); timers3->start(1000); timers4->start(1000);
//                    stopTrig = 0;
//                } else
                if (pauseTrig == 1){

                    if (classifierSelect == 1) {
                        if (brun1 == 1) {
                            timer->start(1000); timers2->stop(); timers3->stop(); timers4->stop();
                            ui->stop2->display(199); ui->stop3->display(199); ui->stop4->display(199); timers->stop();
                        } else if (brunw1 == 1) {
                            timerw->start(1000); timers2->start(1000); timers3->stop(); timers4->stop();
                            ui->stop3->display(199); ui->stop4->display(199);
                        } else if (brun2 == 1) {
                            timer2->start(1000); timers->stop(); timers3->stop(); timers4->stop();
                            ui->stop1->display(199); ui->stop3->display(199); ui->stop4->display(199); timers2->stop();
                        } else if (brunw2 == 1) {
                            timerw2->start(1000); timers3->start(1000); timers4->stop(); timers->stop();
                            ui->stop4->display(199); ui->stop1->display(199);
                        } else if (brun3 == 1) {
                            timer3->start(1000); timers2->stop(); timers->stop(); timers4->stop();
                            ui->stop2->display(199); ui->stop1->display(199); ui->stop4->display(199); timers3->stop();
                        } else if (brunw3 == 1) {
                            timerw3->start(1000); timers4->start(1000); timers->stop(); timers2->stop();
                            ui->stop1->display(199); ui->stop2->display(199);
                        } else if (brun4 == 1) {
                            timer4->start(1000); timers2->stop(); timers3->stop(); timers->stop();
                            ui->stop2->display(199); ui->stop3->display(199); ui->stop1->display(199); timers4->stop();
                        } else if (brunw4 == 1) {
                            timerw4->start(1000); timers->start(1000); timers2->stop(); timers3->stop();
                            ui->stop2->display(199); ui->stop3->display(199);
                        }

                    } else {
                        if (brun1 == 1) {
                            timer->start(1000); timers2->start(1000); timers3->start(1000); timers4->start(1000);
                            timers->stop();
                        } else if (brunw1 == 1) {
                            timerw->start(1000); timers2->start(1000); timers3->start(1000); timers4->start(1000);
                        } else if (brun2 == 1) {
                            timer2->start(1000); timers->start(1000); timers3->start(1000); timers4->start(1000);
                            timers2->stop();
                        } else if (brunw2 == 1) {
                            timerw2->start(1000); timers->start(1000); timers3->start(1000); timers4->start(1000);
                        } else if (brun3 == 1) {
                            timer3->start(1000); timers->start(1000); timers2->start(1000); timers4->start(1000);
                            timers3->stop();
                        } else if (brunw3 == 1) {
                            timerw3->start(1000); timers->start(1000); timers2->start(1000); timers4->start(1000);
                        } else if (brun4 == 1) {
                            timer4->start(1000); timers->start(1000); timers2->start(1000); timers3->start(1000);
                            timers4->stop();
                        } else if (brunw4 == 1) {
                            timerw4->start(1000); timers->start(1000); timers2->start(1000); timers3->start(1000);
                        }
                    }

                } else {
                    timerValues();
                    timer->start(1000); timers2->start(1000); timers3->start(1000); timers4->start(1000); // Boxed
                }
                ui->go1_t->hide(); ui->go2_t->hide(); ui->go3_t->hide();
                ui->wait1_t->hide(); ui->wait2_t->hide(); ui->wait3_t->hide();
                ui->stop1_t->hide(); ui->stop2_t->hide(); ui->stop3_t->hide();
                ui->labelv1_emp->hide(); ui->labelv2_emp->hide(); ui->labelv3_emp->hide(); ui->labelv4_emp->hide();

//            } else {
//                QMessageBox::information(this, "Warning!", "4 videos must be selected for Boxed Junction.");
//            }

        } else if (ifT == 1 && click > 0) {

//            if (vidCount == 0) { // Should be 3

//                if (stopTrig == 1) {    // FOR PAUSE
//                    ttimer->start(1000); ttimers2->start(1000); ttimers3->start(1000);
//                    stopTrig = 0;
//                } else
                if (pauseTrig == 1) {

                    if (classifierSelect == 1) {
                        if (trun1 == 1) {
                            ttimer->start(1000); ttimers2->stop(); ttimers3->stop();
                            ui->stop2_t->display(199); ui->stop3_t->display(199); ttimers->stop();
                        } else if (trunw1 == 1) {
                            ttimerw->start(1000); ttimers2->start(1000); ttimers3->stop();
                            ui->stop3_t->display(199);
                        } else if (trun2 == 1) {
                            ttimer2->start(1000); ttimers->start(1000); ttimers3->stop();
                            ui->stop1_t->display(199); ui->stop3_t->display(199); ttimers2->stop();
                        } else if (trunw2 == 1) {
                            ttimerw2->start(1000); ttimers3->start(1000); ttimers->stop();
                            ui->stop1_t->display(199);
                        } else if (trun3 == 1) {
                            ttimer3->start(1000); ttimers->start(1000); ttimers2->stop();
                            ui->stop2_t->display(199); ui->stop1_t->display(199); ttimers3->stop();
                        } else if (trunw3 == 1) {
                            ttimerw3->start(1000); ttimers->start(1000); ttimers2->stop();
                            ui->stop2_t->display(199);
                        }
                    } else {
                        if (trun1 == 1) {
                            ttimer->start(1000); ttimers2->start(1000); ttimers3->start(1000);
                            ttimers->stop();
                        } else if (trunw1 == 1) {
                            ttimerw->start(1000); ttimers2->start(1000); ttimers3->start(1000);
                        } else if (trun2 == 1) {
                            ttimer2->start(1000); ttimers->start(1000); ttimers3->start(1000);
                            ttimers2->stop();
                        } else if (trunw2 == 1) {
                            ttimerw2->start(1000); ttimers->start(1000); ttimers3->start(1000);
                        } else if (trun3 == 1) {
                            ttimer3->start(1000); ttimers->start(1000); ttimers2->start(1000);
                            ttimers3->stop();
                        } else if (trunw3 == 1) {
                            ttimerw3->start(1000); ttimers->start(1000); ttimers2->start(1000);
                        }
                    }

                } else {
                    timerValuesT();
                    ttimer->start(1000); ttimers2->start(1000); ttimers3->start(1000); // T
                }

                ui->go1_t->show(); ui->go2_t->show(); ui->go3_t->show();
                ui->wait1_t->show(); ui->wait2_t->show(); ui->wait3_t->show();
                ui->stop1_t->show(); ui->stop2_t->show(); ui->stop3_t->show();
                ui->labelv1_emp->hide(); ui->labelv2_emp->hide(); ui->labelv3_emp->hide();

//            } else {
//                QMessageBox::information(this, "Warning!", "3 videos must be selected for T Junction.");
//            }

        }
        ui->statusBar->showMessage("Playing");
        pauseTrig = 0;
        playing = 1;
    }


}

void MainWindow::on_actionPause_triggered()
{
    if (click == 0) {
        QMessageBox::information(this, "Warning!", "No Video has been selected");
    } else {

        ui->est_go1->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go2->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go3->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go4->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");

        emit sendToggleStream(false);
        ui->statusBar->showMessage("Paused");

        if (ifBox == 1) {
            timer->stop(); timer2->stop(); timer3->stop(); timer4->stop();
            timerw->stop(); timerw2->stop(); timerw3->stop(); timerw4->stop();
            timers->stop(); timers2->stop(); timers3->stop(); timers4->stop();
        } else if (ifT == 1) {
            ttimer->stop(); ttimer2->stop(); ttimer3->stop();
            ttimerw->stop(); ttimerw2->stop(); ttimerw3->stop();
            ttimers->stop(); ttimers2->stop(); ttimers3->stop();
        }
        ui->arrDown->hide(); ui->arrLeft->hide(); ui->arrRight->hide(); ui->arrUp->hide();
        goflag1 = 0; goflag2 = 0; goflag3 = 0; goflag4 = 0;
        playing = 0;
    }
    pauseTrig = 1;
}


void MainWindow::on_actionStop_triggered()
{
    if (click == 0) {
        QMessageBox::information(this, "Warning!", "No Video has been selected");
    } else {

        ui->est_go1->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go2->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go3->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go4->setStyleSheet("QLCDNumber {background-color: #cbcbb3;border-radius: 5px;border: 1px solid #222;color: black;}");
        ui->est_go1->display(0); ui->est_go2->display(0); ui->est_go3->display(0); ui->est_go4->display(0);
        ui->down1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->down3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->up1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->up2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->up3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->right4->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left1->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left2->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left3->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");
        ui->left4->setStyleSheet("QLabel {background-color: #aaa;border-radius: 5px;}");

            emit sendToggleStream(false);
            ui->statusBar->showMessage("Stopped");

            if (ifBox == 1) {
                timerValues();
                stopAllB();
            } else if (ifT == 1) {
                timerValuesT();
                stopAllT();
            }
            ui->arrDown->hide(); ui->arrLeft->hide(); ui->arrRight->hide(); ui->arrUp->hide();
            goflag1 = 0; goflag2 = 0; goflag3 = 0; goflag4 = 0;
            playing = 0;
    }
    stopTrig = 1;
}

void MainWindow::on_actionOpen_triggered()  // CHANGED HERE
{
    if (playing == 1) {
        QMessageBox::information(this, "Press Stop Button!", "Please press the Stop Button before changing intersections.");
    } else if (pauseTrig == 1) {
        QMessageBox::information(this, "Press Stop Button!", "Videos should be Stopped in order to switch intersections.");
    } else if (playing == 0 && click > 0) {
        ui->interOption->show();
    }
    if (click == 0) {
        ui->interOption->show();
    }
}

void MainWindow::on_selectInt_clicked() // CHANGED HERE
{
    activate_areas_legend = 0;
    vidCount = 0; checkInt = 0; ifT = 0; ifBox = 0;
    ui->arrDown->hide(); ui->arrLeft->hide(); ui->arrRight->hide(); ui->arrUp->hide();

    if (ui->radioTjunc->isChecked()) {
        ui->src1->show(); ui->src2->show(); ui->src3->show(); ui->src4->hide();
        ui->labelv1->hide();ui->labelv2->hide();ui->labelv3->hide(); ui->labelv4->hide();
        ui->labelv1_t->show(); ui->labelv2_t->show(); ui->labelv3_t->show();
        ui->viewJunc4->hide();

        ui->v4_total_area->clear();
        ui->v4_total_area->setDisabled(true);
        ui->leg_v4name_3->show();
        ui->leg_v4name_4->show();
        ui->leg_v4go->show();
        ui->tjunc_cover->show();
        ui->tjunc_cov->show(); ui->tjunc_name->show();
        ui->labelv1_emp->hide(); ui->labelv2_emp->hide(); ui->labelv3_emp->hide();
        /*ui->leg_dr->hide();*/ ui->leg_v4->hide(); ui->leg_v4go->hide(); ui->leg_v4name->hide();   // THIS IS AN UPDATE
        /*ui->leg_dr1->show(); ui->leg_dr2->show(); ui->leg_dr3->show();*/          // THIS IS AN UPDATE
        ui->leg_v4go->show();
        ui->leg_name1->show(); ui->leg_name2->show(); ui->leg_name3->show();    // THIS IS AN UPDATE
        ifT = 1;
        ifBox = 0;
        timerValuesT();
        timerOperationT();
        ui->listWidget->item(0)->setText(QString("V1 - PACIFIC (UNA):    "));
        ui->listWidget->item(1)->setText(QString("V2 - PACIFIC (JPR):    "));
        ui->listWidget->item(2)->setText(QString("V3 - PACIFIC (MCB):    "));
        ui->listWidget->item(3)->setText(QString(" "));


        QMessageBox::information(this, "Success!", "T - junction has been selected!");

    } else if (ui->radioBoxed->isChecked()) {
        ui->src1->show(); ui->src2->show(); ui->src3->show(); ui->src4->show();
        ui->labelv1->show();ui->labelv2->show();ui->labelv3->show(); ui->labelv4->show();
        ui->labelv1_t->hide(); ui->labelv2_t->hide(); ui->labelv3_t->hide();
        ui->viewJunc4->show();
        //ui->info4->show();

        ui->leg_v4name_3->hide();
        ui->leg_v4name_4->hide();
        ui->tjunc_cover->hide();
        ui->tjunc_cov->hide(); ui->tjunc_name->hide();
        ui->labelv1_emp->hide(); ui->labelv2_emp->hide(); ui->labelv3_emp->hide(); ui->labelv4_emp->hide();
        /*ui->leg_dr->show();*/ ui->leg_v4->show(); ui->leg_v4go->show(); ui->leg_v4name->show();   // THIS IS AN UPDATE
        /*ui->leg_dr1->hide(); ui->leg_dr2->hide(); ui->leg_dr3->hide();*/          // THIS IS AN UPDATE
        ui->leg_name1->hide(); ui->leg_name2->hide(); ui->leg_name3->hide();    // THIS IS AN UPDATE
        ifBox = 1;
        ifT = 0;
        timerValues();
        timerOperation();
        ui->listWidget->item(0)->setText(QString("V1 - SNR (Cebu):     "));
        ui->listWidget->item(1)->setText(QString("V2 - SNR (Terminal):    "));
        ui->listWidget->item(2)->setText(QString("V3 - SNR (CDU):      "));
        ui->listWidget->item(3)->setText(QString("V4 - SNR (Parkmall): "));

        QMessageBox::information(this, "Success!", "Boxed junction has been selected!");
    }
    ui->radioBoxed->setAutoExclusive(false);
    ui->radioBoxed->setChecked(false);
    ui->radioTjunc->setAutoExclusive(false);
    ui->radioTjunc->setChecked(false);
    ui->interOption->close();
    firstMan = 1;
    click++;

}


// --------------------------------------------- COPY TANAN TAMANS UBOS!


void MainWindow::on_counts_clicked()
{
    if(flagInfoToggle == true){
        flagInfoToggle = false;
    } else flagInfoToggle = true;
    if (ifBox == 1) {
        if(flagInfoToggle == true){
            ui->countFW->show();    ui->countMT->show();    ui->countTR->show();
            ui->countFW_2->show();  ui->countMT_2->show();  ui->countTR_2->show();
            ui->countFW_3->show();  ui->countMT_3->show();  ui->countTR_3->show();
            ui->countFW_4->show();  ui->countMT_4->show();  ui->countTR_4->show();
            ui->counts->setStyleSheet("QPushButton {background-color: #cbcbb3;font-size: 16px; font: bold; color: #4d79ff;border-radius: 5px;border: 2px solid #4d79ff;}");
        }else{
            ui->countFW->hide();    ui->countMT->hide();    ui->countTR->hide();
            ui->countFW_2->hide();  ui->countMT_2->hide();  ui->countTR_2->hide();
            ui->countFW_3->hide();  ui->countMT_3->hide();  ui->countTR_3->hide();
            ui->countFW_4->hide();  ui->countMT_4->hide();  ui->countTR_4->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #4d79ff; font-size: 16px; color: white; border-radius: 5px;}");
        }

    } else if (ifT == 1) {
        if(flagInfoToggle == true){
            ui->countFW->show();    ui->countMT->show();    ui->countTR->show();
            ui->countFW_2->show();  ui->countMT_2->show();  ui->countTR_2->show();
            ui->countFW_3->show();  ui->countMT_3->show();  ui->countTR_3->show();
            ui->countFW_4->hide();  ui->countMT_4->hide();  ui->countTR_4->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #cbcbb3;font-size: 16px; font: bold; color: #4d79ff;border-radius: 5px;border: 2px solid #4d79ff;}");
        }else{
            ui->countFW->hide();    ui->countMT->hide();    ui->countTR->hide();
            ui->countFW_2->hide();  ui->countMT_2->hide();  ui->countTR_2->hide();
            ui->countFW_3->hide();  ui->countMT_3->hide();  ui->countTR_3->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #4d79ff; font-size: 16px; color: white; border-radius: 5px;}");
        }
    }
}

void MainWindow::on_roi1_clicked()
{
    emit sendToggleStream(false);
    emit sendRequestROI_1();
}

void MainWindow::on_roi2_clicked()
{
    emit sendToggleStream(false);
    emit sendRequestROI_2();
}

void MainWindow::on_roi3_clicked()
{
    emit sendToggleStream(false);
    emit sendRequestROI_3();
}

void MainWindow::on_roi4_clicked()
{
    emit sendToggleStream(false);
    emit sendRequestROI_4();
}

// from SETH's
void MainWindow::setup()
{
    boolProcessedView = false;
    flagInfoToggle = false;
    thread = new QThread();
    OpenCvWorker *worker = new OpenCvWorker();
    QTimer *workerTrigger = new QTimer();
    workerTrigger->setInterval(10);

    connect(workerTrigger,SIGNAL(timeout()),worker,SLOT(receiveGrabFrame()));
    connect(thread,SIGNAL(started()),workerTrigger,SLOT(start()));
    connect(this,SIGNAL(sendToggleStream(bool)),worker,SLOT(receiveToggleStream(bool)));
    connect(this,SIGNAL(sendSetupCFG(DetectionConfig)),worker,SLOT(receiveSetupCFG(DetectionConfig)));
    connect(this,SIGNAL(sendToggleDebug()),worker,SLOT(receiveToggleDebug()));
    connect(this,SIGNAL(sendSrc1(QString)),worker,SLOT(receiveSrc1(QString)));
    connect(this,SIGNAL(sendSrc2(QString)),worker,SLOT(receiveSrc2(QString)));
    connect(this,SIGNAL(sendSrc3(QString)),worker,SLOT(receiveSrc3(QString)));
    connect(this,SIGNAL(sendSrc4(QString)),worker,SLOT(receiveSrc4(QString)));
    connect(this,SIGNAL(sendRequestROI_1()),worker,SLOT(receiveRequestROI_1()));
    connect(this,SIGNAL(sendRequestROI_2()),worker,SLOT(receiveRequestROI_2()));
    connect(this,SIGNAL(sendRequestROI_3()),worker,SLOT(receiveRequestROI_3()));
    connect(this,SIGNAL(sendRequestROI_4()),worker,SLOT(receiveRequestROI_4()));
    connect(this,SIGNAL(sendROI(JunctionPointsROI)),worker,SLOT(receiveROI(JunctionPointsROI)));
    connect(worker,SIGNAL(sendPointsToSave(JunctionPointsROI)),this,SLOT(receivePointsToSave(JunctionPointsROI)));
    connect(worker,SIGNAL(sendFrame(ProcessedFrame *)),this,SLOT(receiveFrame(ProcessedFrame *)));
    connect(this,SIGNAL(sendDensityUpdateRequest()),this,SLOT(receiveDensityUpdateRequest()));
    connect(this,SIGNAL(sendNightToggle()),worker,SLOT(receiveNightToggle()));
    connect(this,SIGNAL(sendClassifierFWenable(bool)),worker,SLOT(receiveClassifierFWenable(bool)));
    connect(this,SIGNAL(sendClassifierMTenable(bool)),worker,SLOT(receiveClassifierMTenable(bool)));
    connect(this,SIGNAL(sendClassifierTRenable(bool)),worker,SLOT(receiveClassifierTRenable(bool)));
    connect(this,SIGNAL(sendClassifierPath(QString,int)),worker,SLOT(receiveClassifierPath(QString,int)));
    connect(this,SIGNAL(sendReceiveFlagROI()),worker,SLOT(receiveFlagROI()));
    connect(this,SIGNAL(sendChangeInMinMax(int,int,int)),worker,SLOT(receiveChangeInMinMax(int,int,int)));
    connect(this,SIGNAL(sendEnableTjunctionClassifier()),worker,SLOT(receiveEnableT()));
    connect(this,SIGNAL(sendEnableBoxjunctionClassifier()),worker,SLOT(receiveEnableB()));
    createMask();
    workerTrigger->start();
    worker->moveToThread(thread);
    workerTrigger->moveToThread(thread);

    thread->start();


}

void MainWindow::receiveFrame(ProcessedFrame *outputFrames)
{
    frame_count++;
    if(! (frame_count % 10)){
        t_seconds++;
    }

    static int area_l1[3], area_l2[3], area_l3[3], area_l4[3];
    static int frames;

    //BOX CEBU BOUND
    if(outputFrames->original[0].isNull() == false){
        if(boolProcessedView == true)
            ui->viewJunc1->setPixmap(QPixmap::fromImage(outputFrames->processed[0]));
        else
            ui->viewJunc1->setPixmap(QPixmap::fromImage(outputFrames->original[0]));

         if(flagInfoToggle == true)
         {

             if (classifierSelect == 0) {    // CHANGED HERE

                 outputFrames->numberOfFW[0] = 0;
                 outputFrames->numberOfMT[0] = 0;
                 outputFrames->numberOfTR[0] = 0;

             } else {

                 if (ui->classifierFW->isChecked() == false) {
                     outputFrames->numberOfFW[0] = 0;
                 }
                 if (ui->classifierMT->isChecked() == false) {
                     outputFrames->numberOfMT[0] = 0;
                 }
                 if (ui->classifierTR->isChecked() == false) {
                     outputFrames->numberOfTR[0] = 0;
                 }

                 ui->countFW->display(outputFrames->numberOfFW[0]);
                 ui->countMT->display(outputFrames->numberOfMT[0]);
                 ui->countTR->display(outputFrames->numberOfTR[0]);
             }

         }
         if(ifBox == 1){
             if(t_seconds == 5){

                 area_l1[0] = outputFrames->numberOfFW[0] * FW_AREA;
                 area_l1[1] = outputFrames->numberOfMT[0] * MT_AREA;
                 area_l1[2] = outputFrames->numberOfTR[0] * TR_AREA;
                 //total_area_l1 = area_l1[0] + area_l1[1] + area_l1[2];
                 qDebug() << "FW: "<<average_countl1[0]/50;
                 qDebug() << "MT: "<<average_countl1[1]/50;
                 qDebug() << "TR: "<<average_countl1[2]/50;
                 total_area_l1 = ((((average_countl1[0]* FW_AREA)/50))) + ((((average_countl1[1]* MT_AREA)/50))) +((((average_countl1[2]* TR_AREA)/50)));
                 //qDebug() << "5 secc count" << average_countl1[0];
                 average_countl1[0] = 0;
                 average_countl1[1] = 0;
                 average_countl1[2] = 0;


             }else{
                 average_countl1[0] += outputFrames->numberOfFW[0] ;
                 average_countl1[1] += outputFrames->numberOfMT[0];
                 average_countl1[2] += outputFrames->numberOfTR[0];

             }
         }else{
             if(t_seconds == 5){
                 area_l1[0] = outputFrames->numberOfFW[0] * FW_AREA;
                 area_l1[1] = outputFrames->numberOfMT[0] * MT_AREA;
                 area_l1[2] = outputFrames->numberOfTR[0] * TR_AREA;
                 //total_area_l1 = area_l1[0] + area_l1[1] + area_l1[2];
                 total_area_l1 = ((((average_countl1[0]* FW_AREA)/50))) + ((((average_countl1[1]* MT_AREA)/50))) +((((average_countl1[2]* TR_AREA)/50)));
                 //qDebug() << "5 secc count" << average_countl1[0];
                 average_countl1[0] = 0;
                 average_countl1[1] = 0;
                 average_countl1[2] = 0;
             }else{
                 average_countl1[0] += outputFrames->numberOfFW[0] ;
                 average_countl1[1] += outputFrames->numberOfMT[0];
                 average_countl1[2] += outputFrames->numberOfTR[0];
             }

    }
    if(outputFrames->original[1].isNull() == false){
        if(boolProcessedView == true)
            ui->viewJunc2->setPixmap(QPixmap::fromImage(outputFrames->processed[1]));
        else
            ui->viewJunc2->setPixmap(QPixmap::fromImage(outputFrames->original[1]));
        if(flagInfoToggle == true)
        {

            if (classifierSelect == 0) {    // CHANGED HERE

                outputFrames->numberOfFW[1] = 0;
                outputFrames->numberOfMT[1] = 0;
                outputFrames->numberOfTR[1] = 0;

            } else {

                if (ui->classifierFW->isChecked() == false) {
                    outputFrames->numberOfFW[1] = 0;
                }
                if (ui->classifierMT->isChecked() == false) {
                    outputFrames->numberOfMT[1] = 0;
                }
                if (ui->classifierTR->isChecked() == false) {
                    outputFrames->numberOfTR[1] = 0;
                }

                ui->countFW_2->display(outputFrames->numberOfFW[1]);
                ui->countMT_2->display(outputFrames->numberOfMT[1]);
                ui->countTR_2->display(outputFrames->numberOfTR[1]);
            }

        }
        if(ifBox == 1){
            if(t_seconds == 5){

                area_l2[0] = outputFrames->numberOfFW[1] * FW_AREA;
                area_l2[1] = outputFrames->numberOfMT[1] * MT_AREA;
                area_l2[2] = outputFrames->numberOfTR[1] * TR_AREA;
                //total_area_l2 = area_l2[0] + area_l2[1] + area_l2[2];
                total_area_l2 = ((((average_countl2[0]* FW_AREA)/50))) + ((((average_countl2[1]* MT_AREA)/50))) +((((average_countl2[2]* TR_AREA)/50)));
                //qDebug() << "5 secc count" << average_countl1[0];
                average_countl2[0] = 0;
                average_countl2[1] = 0;
                average_countl2[2] = 0;

            }else{
                average_countl2[0] += outputFrames->numberOfFW[1] ;
                average_countl2[1] += outputFrames->numberOfMT[1];
                average_countl2[2] += outputFrames->numberOfTR[1];
            }
        }else{
            if(t_seconds == 5){
                area_l2[0] = outputFrames->numberOfFW[1] * FW_AREA;
                area_l2[1] = outputFrames->numberOfMT[1] * MT_AREA;
                area_l2[2] = outputFrames->numberOfTR[1] * TR_AREA;
                //total_area_l2 = area_l2[0] + area_l2[1] + area_l2[2];
                total_area_l2 = ((((average_countl2[0]* FW_AREA)/50))) + ((((average_countl2[1]* MT_AREA)/50))) +((((average_countl2[2]* TR_AREA)/50)));
                average_countl2[0] = 0;
                average_countl2[1] = 0;
                average_countl2[2] = 0;
            }else{
                average_countl2[0] += outputFrames->numberOfFW[1] ;
                average_countl2[1] += outputFrames->numberOfMT[1];
                average_countl2[2] += outputFrames->numberOfTR[1];
            }
        }
    }

    if(outputFrames->original[2].isNull() == false){
        if(boolProcessedView == true)
            ui->viewJunc3->setPixmap(QPixmap::fromImage(outputFrames->processed[2]));
        else
            ui->viewJunc3->setPixmap(QPixmap::fromImage(outputFrames->original[2]));
        if(flagInfoToggle == true)
        {

            if (classifierSelect == 0) {    // CHANGED HERE

                outputFrames->numberOfFW[2] = 0;
                outputFrames->numberOfMT[2] = 0;
                outputFrames->numberOfTR[2] = 0;

            } else {

                if (ui->classifierFW->isChecked() == false) {
                    outputFrames->numberOfFW[2] = 0;
                }
                if (ui->classifierMT->isChecked() == false) {
                    outputFrames->numberOfMT[2] = 0;
                }
                if (ui->classifierTR->isChecked() == false) {
                    outputFrames->numberOfTR[2] = 0;
                }

                ui->countFW_3->display(outputFrames->numberOfFW[2]);
                ui->countMT_3->display(outputFrames->numberOfMT[2]);
                ui->countTR_3->display(outputFrames->numberOfTR[2]);
            }

        }
        if(ifBox == 1){
            if(t_seconds == 5){

                area_l3[0] = outputFrames->numberOfFW[2] * FW_AREA;
                area_l3[1] = outputFrames->numberOfMT[2] * MT_AREA;
                area_l3[2] = outputFrames->numberOfTR[2] * TR_AREA;
                //total_area_l3 = area_l3[0] + area_l3[1] + area_l3[2];
                total_area_l3 = ((((average_countl3[0]* FW_AREA)/50))) + ((((average_countl3[1]* MT_AREA)/50))) +((((average_countl3[2]* TR_AREA)/50)));
                average_countl3[0] = 0;
                average_countl3[1] = 0;
                average_countl3[2] = 0;

            }else{
                average_countl3[0] += outputFrames->numberOfFW[2] ;
                average_countl3[1] += outputFrames->numberOfMT[2];
                average_countl3[2] += outputFrames->numberOfTR[2];
            }
        }else{
            if(t_seconds == 5){
                area_l3[0] = outputFrames->numberOfFW[2] * FW_AREA;
                area_l3[1] = outputFrames->numberOfMT[2] * MT_AREA;
                area_l3[2] = outputFrames->numberOfTR[2] * TR_AREA;
                //total_area_l3 = area_l3[0] + area_l3[1] + area_l3[2];
                total_area_l3 = ((((average_countl3[0]* FW_AREA)/50))) + ((((average_countl3[1]* MT_AREA)/50))) +((((average_countl3[2]* TR_AREA)/50)));
                average_countl3[0] = 0;
                average_countl3[1] = 0;
                average_countl3[2] = 0;
            }else{
                average_countl3[0] += outputFrames->numberOfFW[2] ;
                average_countl3[1] += outputFrames->numberOfMT[2];
                average_countl3[2] += outputFrames->numberOfTR[2];
            }
        }
    }

    if(outputFrames->original[3].isNull() == false){
        if(boolProcessedView == true)
            ui->viewJunc4->setPixmap(QPixmap::fromImage(outputFrames->processed[3]));
        else
            ui->viewJunc4->setPixmap(QPixmap::fromImage(outputFrames->original[3]));
        if(flagInfoToggle == true)
        {

            if (classifierSelect == 0) {    // CHANGED HERE

                outputFrames->numberOfFW[3] = 0;
                outputFrames->numberOfMT[3] = 0;
                outputFrames->numberOfTR[3] = 0;

            } else {

                if (ui->classifierFW->isChecked() == false) {
                    outputFrames->numberOfFW[3] = 0;
                }
                if (ui->classifierMT->isChecked() == false) {
                    outputFrames->numberOfMT[3] = 0;
                }
                if (ui->classifierTR->isChecked() == false) {
                    outputFrames->numberOfTR[3] = 0;
                }

                ui->countFW_4->display(outputFrames->numberOfFW[3]);
                ui->countMT_4->display(outputFrames->numberOfMT[3]);
                ui->countTR_4->display(outputFrames->numberOfTR[3]);
            }

        }
        if(ifBox == 1){
            if(t_seconds == 5){

                area_l4[0] = outputFrames->numberOfFW[3] * FW_AREA;
                area_l4[1] = outputFrames->numberOfMT[3] * MT_AREA;
                area_l4[2] = outputFrames->numberOfTR[3] * TR_AREA;
                //total_area_l4 = area_l4[0] + area_l4[1] + area_l4[2];
                total_area_l4 = ((((average_countl4[0]* FW_AREA)/50))) + ((((average_countl4[1]* MT_AREA)/50))) +((((average_countl4[2]* TR_AREA)/50)));
                average_countl4[0] = 0;
                average_countl4[1] = 0;
                average_countl4[2] = 0;

            }else{
                average_countl4[0] += outputFrames->numberOfFW[3] ;
                average_countl4[1] += outputFrames->numberOfMT[3];
                average_countl4[2] += outputFrames->numberOfTR[3];
            }
        }
        }

    }
    if(t_seconds == 5) t_seconds = 0;
    emit sendDensityUpdateRequest();

}

void MainWindow::sendSetup()
{


    cfg.resizeMultiplier = 1;
    cfg.maxSize_fw = cv::Size(500,500);
    cfg.minSize_fw = cv::Size(48,48);

    cfg.maxSize_tr = cv::Size(1200,1200);
    cfg.minSize_tr = cv::Size(150,150);

    cfg.maxSize_mt = cv::Size(200,200);
    cfg.minSize_mt = cv::Size(24,24);

    cfg.toggleDetect = true;



}



void MainWindow::on_sim1_toggled(bool checked)
{
   ui->statusBar->showMessage("Sim...");
}

void MainWindow::on_sim1_clicked(bool checked) // CHANGED HERE
{
    boolProcessedView = !boolProcessedView;
    emit sendToggleDebug();

    if(flagInfoToggle == true){
        //ui->submitArea->hide();
//        ui->minSpinBox->hide();
//        ui->maxSpinBox->hide();
//        ui->verticalSlider->hide();
//        ui->verticalSlider_2->hide();
//        ui->radioFW->hide();
//        ui->radioMT->hide();
//        ui->radioTR->hide();
//        ui->label_3->hide();
//        ui->label_4->hide();
//        ui->setMinMax->hide();

        activate_debug_btn = 0;

        flagInfoToggle = false;
    } else {

        //ui->submitArea->show();
//        ui->minSpinBox->show();
//        ui->maxSpinBox->show();
//        ui->verticalSlider->show();
//        ui->verticalSlider_2->show();
//        ui->radioFW->show();
//        ui->radioMT->show();
//        ui->radioTR->show();
//        ui->label_3->show();
//        ui->label_4->show();
//        ui->setMinMax->show();

        activate_debug_btn = 1;

        flagInfoToggle = true;
    }
    if (ifBox == 1) {

        if (activate_areas_legend == 1) {
            ui->v1_total_area->setText(QString::number(v1_area));   // CHANGED HERE
            ui->v2_total_area->setText(QString::number(v2_area));
            ui->v3_total_area->setText(QString::number(v3_area));
            ui->v4_total_area->setText(QString::number(v4_area));
        }


        ui->v1_total_area->setEnabled(true);
        ui->v2_total_area->setEnabled(true);
        ui->v3_total_area->setEnabled(true);
        ui->v4_total_area->setEnabled(true);
        if(flagInfoToggle == true){
            ui->v1_total_area->setReadOnly(false);
            ui->v2_total_area->setReadOnly(false);
            ui->v3_total_area->setReadOnly(false);
            ui->v4_total_area->setReadOnly(false);
            ui->countFW->show();    ui->countMT->show();    ui->countTR->show();
            ui->countFW_2->show();  ui->countMT_2->show();  ui->countTR_2->show();
            ui->countFW_3->show();  ui->countMT_3->show();  ui->countTR_3->show();
            ui->countFW_4->show();  ui->countMT_4->show();  ui->countTR_4->show();
            ui->counts->setStyleSheet("QPushButton {background-color: #cbcbb3;font-size: 16px; font: bold; color: #4d79ff;border-radius: 5px;border: 2px solid #4d79ff;}");
        }else{

            ui->v1_total_area->setText(" ");   // CHANGED HERE
            ui->v2_total_area->setText(" ");
            ui->v3_total_area->setText(" ");
            ui->v4_total_area->setText(" ");

            ui->v1_total_area->setReadOnly(true);
            ui->v2_total_area->setReadOnly(true);
            ui->v3_total_area->setReadOnly(true);
            ui->v4_total_area->setReadOnly(true);
            ui->countFW->hide();    ui->countMT->hide();    ui->countTR->hide();
            ui->countFW_2->hide();  ui->countMT_2->hide();  ui->countTR_2->hide();
            ui->countFW_3->hide();  ui->countMT_3->hide();  ui->countTR_3->hide();
            ui->countFW_4->hide();  ui->countMT_4->hide();  ui->countTR_4->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #4d79ff; font-size: 16px; color: white; border-radius: 5px;}");
        }

    } else if (ifT == 1) {
        if(flagInfoToggle == true){

            if (activate_areas_legend == 1) {
                ui->v1_total_area->setText(QString::number(v1_areat));  // CHANGED HERE
                ui->v2_total_area->setText(QString::number(v2_areat));
                ui->v3_total_area->setText(QString::number(v3_areat));
            }


            ui->v1_total_area->setReadOnly(false);
            ui->v2_total_area->setReadOnly(false);
            ui->v3_total_area->setReadOnly(false);
            ui->v4_total_area->setReadOnly(true);
            ui->countFW->show();    ui->countMT->show();    ui->countTR->show();
            ui->countFW_2->show();  ui->countMT_2->show();  ui->countTR_2->show();
            ui->countFW_3->show();  ui->countMT_3->show();  ui->countTR_3->show();
            ui->countFW_4->hide();  ui->countMT_4->hide();  ui->countTR_4->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #cbcbb3;font-size: 16px; font: bold; color: #4d79ff;border-radius: 5px;border: 2px solid #4d79ff;}");
        }else{

            ui->v1_total_area->setText(" ");  // CHANGED HERE
            ui->v2_total_area->setText(" ");
            ui->v3_total_area->setText(" ");

            ui->v1_total_area->setReadOnly(true);
            ui->v2_total_area->setReadOnly(true);
            ui->v3_total_area->setReadOnly(true);
            ui->v4_total_area->setReadOnly(true);
            ui->countFW->hide();    ui->countMT->hide();    ui->countTR->hide();
            ui->countFW_2->hide();  ui->countMT_2->hide();  ui->countTR_2->hide();
            ui->countFW_3->hide();  ui->countMT_3->hide();  ui->countTR_3->hide();
            ui->counts->setStyleSheet("QPushButton {background-color: #4d79ff; font-size: 16px; color: white; border-radius: 5px;}");
        }
    }
}

void MainWindow::on_src1_clicked()
{
    src1 = QFileDialog::getOpenFileName(this,"      PLAY video for V1","","Video File (*.3gp *.avi *.mp4 *.mkv)");
    if(src1.isNull()) return;
    emit sendSrc1(src1);
    ui->roi1->show();
    srcTrig = 1;
   // vidCount++;
}



void MainWindow::on_src2_clicked()
{
    src2 = QFileDialog::getOpenFileName(this,"      PLAY video for V1","","Video File (*.3gp *.avi *.mp4 *.mkv)");
    if(src2.isNull()) return;
    emit sendSrc2(src2);
    ui->roi2->show();
    srcTrig = 1;
    //vidCount++;
}

void MainWindow::on_src3_clicked()
{
    src3 = QFileDialog::getOpenFileName(this,"      PLAY video for V1","","Video File (*.3gp *.avi *.mp4 *.mkv)");
    if(src3.isNull()) return;
    emit sendSrc3(src3);
    ui->roi3->show();
    srcTrig = 1;
    //vidCount++;
}

void MainWindow::on_src4_clicked()
{
    src4 = QFileDialog::getOpenFileName(this,"      PLAY video for V1","","Video File (*.3gp *.avi *.mp4 *.mkv)");
    if(src4.isNull()) return;
    emit sendSrc4(src4);
    ui->roi4->show();
    srcTrig = 1;
    //vidCount++;
}

void MainWindow::createMask()
{

    JunctionPointsROI roi;
    qRegisterMetaType(&roi);
    std::vector<cv::Point> src1,src2,src3,src4;
    src1.push_back(cv::Point(0,0));
    src1.push_back(cv::Point(2560,0));
    src1.push_back(cv::Point(2560,1920));
    src1.push_back(cv::Point(0,1920));

    src2.push_back(cv::Point(0,0));
    src2.push_back(cv::Point(2560,0));
    src2.push_back(cv::Point(2560,1920));
    src2.push_back(cv::Point(0,1920));

    src3.push_back(cv::Point(0,0));
    src3.push_back(cv::Point(2560,0));
    src3.push_back(cv::Point(2560,1920));
    src3.push_back(cv::Point(0,1920));

    src4.push_back(cv::Point(0,0));
    src4.push_back(cv::Point(2560,0));
    src4.push_back(cv::Point(2560,1920));
    src4.push_back(cv::Point(0,1920));

    roi.pSrc1 = src1;
    roi.pSrc2 = src2;
    roi.pSrc3 = src3;
    roi.pSrc4 = src4;

    emit sendROI(roi);
}


void MainWindow::on_actionclassifierSelect_triggered()
{
    if (playing == 0) {
        QMessageBox::information(this, "OOPS!", "Videos must be played first.");
    } else {

        if (ifBox == 1) {
//            if (brun1 == 0 && classifierSelect == 1) {
//                QMessageBox::information(this, "OOPS!", "Must wait for Go Time to reach V1 and remove classifiers.");
//            } else {
                ui->classifier->show();
//            }
        } else if (ifT == 1) {
            if (trun1 == 0 && classifierSelect == 1) {
                QMessageBox::information(this, "OOPS!", "Must wait for Go Time to reach V1 and remove classifiers.");
            } else {
                ui->classifier->show();
            }
        }

    }
}

void MainWindow::on_selectInt_2_clicked()
{
      if(ui->classifierFW->isChecked())
          emit sendClassifierFWenable(true);
      else
          emit sendClassifierFWenable(false);

      if(ui->classifierMT->isChecked())
          emit sendClassifierMTenable(true);
      else
          emit sendClassifierMTenable(false);

      if(ui->classifierTR->isChecked())
          emit sendClassifierTRenable(true);
      else
          emit sendClassifierTRenable(false);

      ui->classifier->close();
//    if(ui->classifierFW->isChecked() == true){

//        cfg.classifierFW = cv::CascadeClassifier(CLASSIFIER_FW_PATH);
//        cfg.classifierFW_n = cv::CascadeClassifier(CLASSIFIER_NIGHT_FW_PATH);
//    }else{
//        cfg.classifierFW = cv::CascadeClassifier();
//        cfg.classifierFW_n = cv::CascadeClassifier();
//    }
//    if(ui->classifierMT->isChecked() == true){
//        cfg.classifierMT = cv::CascadeClassifier(CLASSIFIER_M_PATH);
//        cfg.classifierFW_n = cv::CascadeClassifier(CLASSIFIER_NIGHT_MT_PATH);
//    }else{
//        cfg.classifierMT = cv::CascadeClassifier();
//        cfg.classifierMT_n = cv::CascadeClassifier();
//    }
//    if(ui->classifierTR->isChecked() == true){
//        cfg.classifierTR = cv::CascadeClassifier(CLASSIFIER_T_PATH);
//        cfg.classifierFW_n = cv::CascadeClassifier(CLASSIFIER_NIGHT_T_PATH);
//    }else{
//        cfg.classifierTR = cv::CascadeClassifier();
//        cfg.classifierTR_n = cv::CascadeClassifier();
//    }


//    if(cfg.classifierFW.empty() == true){
//        ui->statusBar->showMessage("FW empty...");
//    }else{
//        ui->statusBar->showMessage("not empty...");
//    }
//    sendSetup();
//    emit sendSetupCFG(cfg);
//    ui->classifier->close();

      // CHANGED HERE!
    if (ui->classifierFW->isChecked() || ui->classifierMT->isChecked() || ui->classifierTR->isChecked()) {
        classifierSelect = 1;
    } else {
        classifierSelect = 0;
        ui->countFW->display(0); ui->countFW_2->display(0); ui->countFW_3->display(0); ui->countFW_4->display(0);
        ui->countMT->display(0); ui->countMT_2->display(0); ui->countMT_3->display(0); ui->countMT_4->display(0);
        ui->countTR->display(0); ui->countTR_2->display(0); ui->countTR_3->display(0); ui->countTR_4->display(0);
        on_actionStop_triggered();
        QMessageBox::information(this, "No Classifier has been Selected!", "Press Play button.");
    }

}


void MainWindow::on_closebj_8_clicked()
{
    ui->classifier->close();
}

void MainWindow::receivePointsToSave(JunctionPointsROI jproi)
{

    char buff[256];
    sprintf(buff,
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n"
                 "%d,%d\n",
             jproi.pSrc1[0].x,jproi.pSrc1[0].y,
             jproi.pSrc1[1].x,jproi.pSrc1[1].y,
             jproi.pSrc1[2].x,jproi.pSrc1[2].y,
             jproi.pSrc1[3].x,jproi.pSrc1[3].y,

             jproi.pSrc2[0].x,jproi.pSrc2[0].y,
             jproi.pSrc2[1].x,jproi.pSrc2[1].y,
             jproi.pSrc2[2].x,jproi.pSrc2[2].y,
             jproi.pSrc2[3].x,jproi.pSrc2[3].y,

             jproi.pSrc3[0].x,jproi.pSrc3[0].y,
             jproi.pSrc3[1].x,jproi.pSrc3[1].y,
             jproi.pSrc3[2].x,jproi.pSrc3[2].y,
             jproi.pSrc3[3].x,jproi.pSrc3[3].y,

             jproi.pSrc4[0].x,jproi.pSrc4[0].y,
             jproi.pSrc4[1].x,jproi.pSrc4[1].y,
             jproi.pSrc4[2].x,jproi.pSrc4[2].y,
             jproi.pSrc4[3].x,jproi.pSrc4[3].y);

    std::ofstream myFile;
    myFile.open("C:\\img\\roi_points.txt");\
    myFile << buff;
    myFile.close();
    ui->statusBar->showMessage("ROI saved.");
}

void MainWindow::on_actionOpen_2_triggered()
{
    if (playing == 0) {
        QMessageBox::information(this, "OOPS!", "Videos must be played first.");
    } else {
        JunctionPointsROI jp;
        std::string s1_p1,s1_p2,s1_p3,s1_p4,
                s2_p1,s2_p2,s2_p3,s2_p4,
                s3_p1,s3_p2,s3_p3,s3_p4,
                s4_p1,s4_p2,s4_p3,s4_p4;
        QString points;
        points = QFileDialog::getOpenFileName(this,"      ROI file","","ROI (*.txt)");
        std::ifstream myFile;
        myFile.open(points.toStdString());

        if(myFile.is_open()){
            std::getline(myFile,s1_p1);
            std::getline(myFile,s1_p2);
            std::getline(myFile,s1_p3);
            std::getline(myFile,s1_p4);

            std::getline(myFile,s2_p1);
            std::getline(myFile,s2_p2);
            std::getline(myFile,s2_p3);
            std::getline(myFile,s2_p4);

            std::getline(myFile,s3_p1);
            std::getline(myFile,s3_p2);
            std::getline(myFile,s3_p3);
            std::getline(myFile,s3_p4);

            std::getline(myFile,s4_p1);
            std::getline(myFile,s4_p2);
            std::getline(myFile,s4_p3);
            std::getline(myFile,s4_p4);
        }

        int prev_pos = 4;
        JunctionPointsROI *jpROI = new JunctionPointsROI;
        qRegisterMetaType(jpROI);
        jpROI->pSrc1.push_back(cv::Point(
                    atoi(s1_p1.substr(0,s1_p1.find_first_of(',')).c_str()),
                    atoi(s1_p1.substr(s1_p1.find_first_of(',')+1,s1_p1.size()).c_str()))
                    );
        jpROI->pSrc1.push_back(cv::Point(
                    atoi(s1_p2.substr(0,s1_p2.find_first_of(',')).c_str()),
                    atoi(s1_p2.substr(s1_p2.find_first_of(',')+1,s1_p2.size()).c_str()))
                    );
        jpROI->pSrc1.push_back(cv::Point(
                    atoi(s1_p3.substr(0,s1_p3.find_first_of(',')).c_str()),
                    atoi(s1_p3.substr(s1_p3.find_first_of(',')+1,s1_p3.size()).c_str()))
                    );
        jpROI->pSrc1.push_back(cv::Point(
                    atoi(s1_p4.substr(0,s1_p4.find_first_of(',')).c_str()),
                    atoi(s1_p4.substr(s1_p4.find_first_of(',')+1,s1_p4.size()).c_str()))
                    );


        jpROI->pSrc2.push_back(cv::Point(
                    atoi(s2_p1.substr(0,s2_p1.find_first_of(',')).c_str()),
                    atoi(s2_p1.substr(s2_p1.find_first_of(',')+1,s2_p1.size()).c_str()))
                    );
        jpROI->pSrc2.push_back(cv::Point(
                    atoi(s2_p2.substr(0,s2_p2.find_first_of(',')).c_str()),
                    atoi(s2_p2.substr(s2_p2.find_first_of(',')+1,s2_p2.size()).c_str()))
                    );
        jpROI->pSrc2.push_back(cv::Point(
                    atoi(s2_p3.substr(0,s2_p3.find_first_of(',')).c_str()),
                    atoi(s2_p3.substr(s2_p3.find_first_of(',')+1,s2_p3.size()).c_str()))
                    );
        jpROI->pSrc2.push_back(cv::Point(
                    atoi(s2_p4.substr(0,s2_p4.find_first_of(',')).c_str()),
                    atoi(s2_p4.substr(s2_p4.find_first_of(',')+1,s2_p4.size()).c_str()))
                    );


        jpROI->pSrc3.push_back(cv::Point(
                    atoi(s3_p1.substr(0,s3_p1.find_first_of(',')).c_str()),
                    atoi(s3_p1.substr(s3_p1.find_first_of(',')+1,s3_p1.size()).c_str()))
                    );
        jpROI->pSrc3.push_back(cv::Point(
                    atoi(s3_p2.substr(0,s3_p2.find_first_of(',')).c_str()),
                    atoi(s3_p2.substr(s3_p2.find_first_of(',')+1,s3_p2.size()).c_str()))
                    );
        jpROI->pSrc3.push_back(cv::Point(
                    atoi(s3_p3.substr(0,s3_p3.find_first_of(',')).c_str()),
                    atoi(s3_p3.substr(s3_p3.find_first_of(',')+1,s3_p3.size()).c_str()))
                    );
        jpROI->pSrc3.push_back(cv::Point(
                    atoi(s3_p4.substr(0,s3_p4.find_first_of(',')).c_str()),
                    atoi(s3_p4.substr(s3_p4.find_first_of(',')+1,s3_p4.size()).c_str()))
                    );



        jpROI->pSrc4.push_back(cv::Point(
                    atoi(s4_p1.substr(0,s4_p1.find_first_of(',')).c_str()),
                    atoi(s4_p1.substr(s4_p1.find_first_of(',')+1,s4_p1.size()).c_str()))
                    );
        jpROI->pSrc4.push_back(cv::Point(
                    atoi(s4_p2.substr(0,s4_p2.find_first_of(',')).c_str()),
                    atoi(s4_p2.substr(s4_p2.find_first_of(',')+1,s4_p2.size()).c_str()))
                    );
        jpROI->pSrc4.push_back(cv::Point(
                    atoi(s4_p3.substr(0,s4_p3.find_first_of(',')).c_str()),
                    atoi(s4_p3.substr(s4_p3.find_first_of(',')+1,s4_p3.size()).c_str()))
                    );
        jpROI->pSrc4.push_back(cv::Point(
                    atoi(s4_p4.substr(0,s4_p4.find_first_of(',')).c_str()),
                    atoi(s4_p4.substr(s4_p4.find_first_of(',')+1,s4_p4.size()).c_str()))
                    );

        myFile.close();
        emit sendReceiveFlagROI();
        emit sendROI(*jpROI);
        activate_areas_legend = 1;  //CHANGED HERE
    }

}


void MainWindow::receiveDensityUpdateRequest()
{
    char buff[256];
    //    if (checkStat == 1) {
            if(ifBox == 1){


                if (classifierSelect == 0) {    // CHANGED HERE
                    ui->listWidget->item(0)->setText("V1 - SNR (Cebu): ");
                    ui->listWidget->item(1)->setText("V2 - SNR (Terminal): ");
                    ui->listWidget->item(2)->setText("V3 - SNR (CDU): ");
                    ui->listWidget->item(3)->setText("V4 - SNR (Parkmall): ");
                } else {
                    if(total_area_l1 <= v1_area * LOW_PERCENT){
                        sprintf(buff,"V1 - SNR (Cebu): LOW (%.0f%)",(float)((float)total_area_l1/v1_area)*100);
                        ui->listWidget->item(0)->setText(QString(buff));

                        //ui->est_go1->display(19);
                        cebu_low = 1;
                        cebu_mod = 0;
                        cebu_high = 0;
                    }else if(total_area_l1 > v1_area * LOW_PERCENT && total_area_l1 <= v1_area * MOD_PERCENT){
                        sprintf(buff,"V1 - SNR (Cebu): MOD (%.0f%)",(float)((float)total_area_l1/v1_area)*100);
                        ui->listWidget->item(0)->setText(QString(buff));

                        //ui->est_go1->display(27);
                        cebu_low = 0;
                        cebu_high = 0;
                        cebu_mod = 1;
                    }else if(total_area_l1 > v1_area * MOD_PERCENT){
                        sprintf(buff,"V1 - SNR (Cebu): HIGH (%.0f%)",(float)((float)total_area_l1/v1_area)*100);
                        ui->listWidget->item(0)->setText(QString(buff));

                        //ui->est_go1->display(51);
                        cebu_high = 1;
                        cebu_low = 0;
                        cebu_mod = 0;
                    }

                    if(total_area_l2 < v2_area * LOW_PERCENT){
                        sprintf(buff,"V2 - SNR (Terminal): LOW (%.2f%)",(float)((float)total_area_l2/v2_area)*100);
                        ui->listWidget->item(1)->setText(QString(buff));
                        //ui->est_go2->display(19);

                        bus_low = 1;
                        bus_mod = 0;
                        bus_high = 0;
                    }else if(total_area_l2 > v2_area * LOW_PERCENT && total_area_l2 < v2_area * MOD_PERCENT){
                         sprintf(buff,"V2 - SNR (Terminal): MOD (%.2f%)",(float)((float)total_area_l2/v2_area)*100);
                        ui->listWidget->item(1)->setText(QString(buff));
                        //ui->est_go2->display(27);

                        bus_low = 0;
                        bus_mod = 1;
                        bus_high = 0;
                    }else if(total_area_l2  > v2_area * MOD_PERCENT){
                         sprintf(buff,"V2 - SNR (Terminal): HIGH (%.2f%)",(float)((float)total_area_l2/v2_area)*100);
                        ui->listWidget->item(1)->setText(QString(buff));
                        //ui->est_go2->display(51);
                        bus_low = 0;
                        bus_mod = 0;
                        bus_high = 1;
                    }

                    if((total_area_l3 ) < v3_area * LOW_PERCENT){
                        sprintf(buff,"V3 - SNR (CDU): LOW (%.2f%)",(float)((float)total_area_l3/v3_area)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        //ui->est_go3->display(19);
                        cdu_mod = 0;
                        cdu_high = 0;
                        cdu_low = 1;
                    }else if((total_area_l3 ) > v3_area * LOW_PERCENT && total_area_l3 < v3_area * MOD_PERCENT){
                        sprintf(buff,"V3 - SNR (CDU): MOD (%.2f%)",(float)((float)total_area_l3/v3_area)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        //ui->est_go3->display(27);
                        cdu_low = 0;
                        cdu_mod = 1;
                        cdu_high = 0;
                    }else if((total_area_l3 ) > v3_area * MOD_PERCENT){
                        sprintf(buff,"V3 - SNR (CDU): HIGH (%.2f%)",(float)((float)total_area_l3/v3_area)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        //ui->est_go3->display(51);
                        cdu_high = 1;
                        cdu_mod = 0;
                        cdu_low = 0;
                    }

                    if((total_area_l4 ) < v4_area * LOW_PERCENT){
                        sprintf(buff,"V4 - SNR (Parkmall): LOW (%.2f%)",(float)((float)total_area_l4/v4_area)*100 );
                        ui->listWidget->item(3)->setText(QString(buff));

                        //ui->est_go4->display(19);
                        pmall_low = 1;
                        pmall_mod = 0;
                        pmall_high = 0;
                    }else if((total_area_l4 ) > v4_area * LOW_PERCENT && total_area_l4 < v4_area * MOD_PERCENT){
                        sprintf(buff,"V4 - SNR (Parkmall): MOD (%.2f%)",(float)((float)total_area_l4/v4_area)*100 );
                        ui->listWidget->item(3)->setText(QString(buff));

                        //ui->est_go4->display(27);
                        pmall_mod = 1;
                        pmall_high = 0;
                        pmall_low = 0;
                    }else if((total_area_l4 ) > v4_area * MOD_PERCENT){
                        sprintf(buff,"V4 - SNR (Parkmall): HIGH (%.2f%)",(float)((float)total_area_l4/v4_area)*100 );
                        ui->listWidget->item(3)->setText(QString(buff));

                        //ui->est_go4->display(51);
                        pmall_high = 1;
                        pmall_low = 0;
                        pmall_mod = 0;
                    }
                }


            } else {

                if (classifierSelect == 0) {    // CHANGED HERE
                    ui->listWidget->item(2)->setText("V3 PACIFIC (MCB): ");
                    ui->listWidget->item(1)->setText("V2 PACIFIC (JPR): ");
                    ui->listWidget->item(0)->setText("V1 PACIFIC (UNA): ");
                } else {
                    if((total_area_l3 )  < v3_areat * LOW_PERCENT){
                        sprintf(buff,"V2 PACIFIC (MCB): LOW (%.2f%)",(float)((float)total_area_l3/v3_areat)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        //ui->est_go3->display(19);
                        mcb_low = 1;
                        mcb_high = 0;
                        mcb_mod = 0;
                    }else if((total_area_l3 ) > v3_areat * LOW_PERCENT && total_area_l3 < v3_areat * MOD_PERCENT){
                        sprintf(buff,"V3 PACIFIC (MCB): MOD (%.2f%)",(float)((float)total_area_l3/v3_areat)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        //ui->est_go3->display(19);
                        mcb_mod = 1;
                        mcb_low = 0;
                        mcb_high = 0;
                    }else if((total_area_l3 ) > v3_areat * MOD_PERCENT){
                        sprintf(buff,"V3 PACIFIC (MCB): HIGH (%.2f%)",(float)((float)total_area_l3/v3_areat)*100 );
                        ui->listWidget->item(2)->setText(QString(buff));

                        mcb_high = 1;
                        mcb_low = 0;
                        mcb_mod = 0;
                    }

                    if((total_area_l2 ) < v2_areat * LOW_PERCENT){
                        sprintf(buff,"V3 PACIFIC (JPR): LOW (%.2f%)",(float)((float)total_area_l2/v2_areat)*100 );
                        ui->listWidget->item(1)->setText(QString(buff));

                        jpr_low = 1;
                        jpr_high = 0;
                        jpr_mod = 0;

                    }else if((total_area_l2 ) > v2_areat * LOW_PERCENT && total_area_l2 < v2_areat *MOD_PERCENT){
                        sprintf(buff,"V2 PACIFIC (JPR): MOD (%.2f%)",(float)((float)total_area_l2/v2_areat)*100 );
                        ui->listWidget->item(1)->setText(QString(buff));

                        jpr_mod = 1;
                        jpr_high = 0;
                        jpr_low = 0;
                    }else if((total_area_l2 ) > v2_areat * MOD_PERCENT){
                        sprintf(buff,"V2 PACIFIC (JPR): HIGH (%.2f%)",(float)((float)total_area_l2/v2_areat)*100 );
                        ui->listWidget->item(1)->setText(QString(buff));

                        jpr_high = 1;
                        jpr_low = 0;
                        jpr_mod = 0;
                    }

                    if((total_area_l1 ) < v1_areat * LOW_PERCENT){
                        sprintf(buff,"V1 PACIFIC (UNA): LOW (%.2f%)",(float)((float)total_area_l1/v1_areat)*100 );
                        ui->listWidget->item(0)->setText(QString(buff));

                        una_low = 1;
                        una_high = 0;
                        una_mod = 0;
                    }else if((total_area_l1 ) > v1_areat * LOW_PERCENT && total_area_l1 < v1_areat * MOD_PERCENT){
                        sprintf(buff,"V1 PACIFIC (UNA): MOD (%.2f%)",(float)((float)total_area_l1/v1_areat)*100 );
                        ui->listWidget->item(0)->setText(QString(buff));

                        una_mod = 1;
                        una_high = 0;
                        una_low = 0;
                    }else if((total_area_l1 ) > v1_areat * MOD_PERCENT){
                        sprintf(buff,"V1 PACIFIC (UNA): HIGH (%.2f%)",(float)((float)total_area_l1/v1_areat)*100 );
                        ui->listWidget->item(0)->setText(QString(buff));
                        una_low = 0;
                        una_mod = 0;
                        una_high = 1;
                    }
                }
            }

}

void MainWindow::on_NightButton_clicked()
{

    emit sendNightToggle();

}



void MainWindow::on_classifierFWpath_clicked()
{
    QString classifier;
    classifier = QFileDialog::getOpenFileName(this,"Classifier ","C:\\","*.xml");
    if(classifier.isNull()) return;
    emit sendClassifierPath(classifier,0);
}

void MainWindow::on_classifierTRpath_clicked()
{
    QString classifier;
    classifier = QFileDialog::getOpenFileName(this,"Classifier ","C:\\","*.xml");
    if(classifier.isNull()) return;
    emit sendClassifierPath(classifier,1);
}

void MainWindow::on_classifierMTpath_clicked()
{
    QString classifier;
    classifier = QFileDialog::getOpenFileName(this,"Classifier ","C:\\","*.xml");
    if(classifier.isNull()) return;
    emit sendClassifierPath(classifier,2);
}



void MainWindow::on_sim1_clicked()
{

}

void MainWindow::on_setMinMax_clicked()
{

}

void MainWindow::on_submitArea_clicked()
{
    if(ifBox){
        v1_area = ui->v1_total_area->text().toInt();
        v2_area = ui->v2_total_area->text().toInt();
        v3_area = ui->v3_total_area->text().toInt();
        v4_area = ui->v4_total_area->text().toInt();
    }else{
        v1_areat = ui->v1_total_area->text().toInt();
        v2_areat = ui->v2_total_area->text().toInt();
        v3_areat = ui->v3_total_area->text().toInt();
    }

    return;
}
