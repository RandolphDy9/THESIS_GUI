#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include "traffic.h"
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QtSql>
#include <QTableView>
#include <iostream>
#include <fstream>
#include <QMouseEvent>
#include <QLCDNumber>
#include <QCheckBox>
#include <QScrollArea>
#include <QFile>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "image_proc.h"
#define RESIZE_MULTIPLIER 1


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase mydb;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void hidden();


    void timer_data();
    void showTime();
    //void showTimeStamp();

    void timer_timeoutgo1();
    void timer_timewait();
    void timer_timestop1();

    void timer_timeoutgo2();
    void timer_timewait2();
    void timer_timestop2();

    void timer_timeoutgo3();
    void timer_timewait3();
    void timer_timestop3();

    void timer_timeoutgo4();
    void timer_timewait4();
    void timer_timestop4();

    void timer_ttimego();
    void timer_ttimego2();
    void timer_ttimego3();

    void timer_ttimestop();
    void timer_ttimestop2();
    void timer_ttimestop3();

    void timer_ttimewait();
    void timer_ttimewait2();
    void timer_ttimewait3();

    void on_actionOpen_triggered();
    void on_actionPlay_triggered();
    void on_actionStop_triggered();

    void on_toHistory_clicked();
    void on_manualBtn_clicked();
    void on_toMain_clicked();
    void on_login_clicked();
    void on_actionClose_Program_2_triggered();
    void on_actionTraffic_History_triggered();
    void on_closeBTN_clicked();
    void on_selectInt_clicked();

    void timerValues();
    void timerValuesT();

//    void multivid();
    void timerOperation();
    void timerOperationT();

    void exportTableViewToCSV(QTableView *table);

//    void on_info1_clicked();
//    void on_info2_clicked();
//    void on_info3_clicked();
//    void on_info4_clicked();

    void on_roi1_clicked();

    void stopAllT();
    void stopAllB();

signals:
    void sendROI(JunctionPointsROI);
    void sendToggleStream(bool);
    void sendSetupCFG(DetectionConfig _cfg);
    void sendToggleDebug();
    void sendSrc1(QString);
    void sendSrc2(QString);
    void sendSrc3(QString);
    void sendSrc4(QString);
    void sendRequestROI_1();
    void sendRequestROI_2();
    void sendRequestROI_3();
    void sendRequestROI_4();
    void sendNightToggle();
    void sendDensityUpdateRequest();
    void sendClassifierFWenable(bool);
    void sendClassifierMTenable(bool);
    void sendClassifierTRenable(bool);
    void sendClassifierPath(QString, int);
    void sendReceiveFlagROI();
    void sendChangeInMinMax(int,int,int);
    void sendEnableTjunctionClassifier();
    void sendEnableBoxjunctionClassifier();
private slots:
    void receiveFrame(ProcessedFrame* outputFrames);
    void sendSetup();
    void receivePointsToSave(JunctionPointsROI);
    void receiveDensityUpdateRequest();
    void on_sim1_toggled(bool checked);
    void on_sim1_clicked(bool checked);
    void on_src1_clicked();
    void on_src2_clicked();
    void on_src3_clicked();
    void on_src4_clicked();



    void on_actionclassifierSelect_triggered();

    void on_selectInt_2_clicked();


    void on_closebj_8_clicked();

    void on_roi2_clicked();

    void on_roi3_clicked();

    void on_roi4_clicked();

    void on_actionOpen_2_triggered();


    void on_counts_clicked();

    void on_NightButton_clicked();
    void on_actionPause_triggered();
    void mouseDoubleClickEvent(QMouseEvent *event);

    void on_toMain_2_clicked();


    void on_classifierFWpath_clicked();

    void on_classifierTRpath_clicked();

    void on_classifierMTpath_clicked();
    void addValues(QString id, QString rn, QString ar, int td, QString ts, QString eg); // THIS IS AN UPDATE


    void on_sim1_clicked();

    void on_setMinMax_clicked();

    void on_submitArea_clicked();

private:
    Ui::MainWindow *ui;
//    traffic *theTraffic;
    int night_flag;
    QTimer *timer, *timer2, *timer3, *timer4,
        *timerw, *timerw2, *timerw3, *timerw4,
        *timers, *timers2, *timers3, *timers4;
    QTimer *ttimer, *ttimer2, *ttimer3,
            *ttimers, *ttimers2, *ttimers3,
            *ttimerw, *ttimerw2, *ttimerw3;
    QTimer *tData, *time; //*timeStamp

    QMediaPlayer* player;
    QMediaPlayer* player1;
    QMediaPlayer* player2;
    QMediaPlayer* player3;

    QVideoWidget* vw;
    QVideoWidget* vw1;
    QVideoWidget* vw2;
    QVideoWidget* vw3;


    QThread *thread;

    void setup();
    void createMask();

    bool toggleCount;
    bool flagInfoToggle;
    bool boolProcessedView;
    unsigned int t_seconds;
    unsigned int frame_count;
    DetectionConfig cfg;

    QString src1,src2,src3,src4;



    // Boxed Junction
    int wait, wait2, wait3, wait4;
    int cStop, cStop2, cStop3, cStop4;
    int cGo, cGo2, cGo3, cGo4;

    // T-Junction
    int tGo1, tGo2, tGo3;
    int tStop1, tStop2, tStop3;
    int twait1, twait2, twait3;

    int ifBox, ifT = 0;
    int click = 0;

    // check if manual
    int man = 0;

    int keyVid = 0;
    int firstMan = 0;

    int checkInt = 0; // to be removed
    int playing = 0;

    int goflag1, goflag2, goflag3, goflag4 = 0;
    int lowClicked, modClicked, heavyClicked = 0;

    int trigMan = 0;
    int trun1, trun2, trun3 = 0;
    int trunw1, trunw2, trunw3 = 0; // CHANGED HERE
    int tFlag1, tFlag2, tFlag3 = 0;

    int brun1, brun2, brun3, brun4 = 0;
    int brunw1, brunw2, brunw3, brunw4 = 0; // CHANGED HERE
    int bFlag1, bFlag2, bFlag3, bFlag4 = 0;

    int vidCount = 0;
    int stopTrig = 0;
    int pauseTrig = 0;

    int min_size;
    int max_size;

    int srcTrig = 0;
    int checkStat = 0;

    int total_area_l1 = 0, total_area_l2 = 0,total_area_l3 = 0,total_area_l4 = 0;
    int cebu_low, cebu_mod, cebu_high = 0;
    int pmall_low, pmall_mod, pmall_high = 0;
    int cdu_low, cdu_mod, cdu_high = 0;
    int bus_low, bus_mod, bus_high = 0;
    int mcb_low, mcb_mod, mcb_high = 0;
    int jpr_low, jpr_mod, jpr_high = 0;
    int una_low, una_mod, una_high = 0;
    int classifierSelect = 0;

    int
    v1_area = DEFAULT_TOTAL_AREA_OA_CEBU,
    v2_area = DEFAULT_TOTAL_AREA_EO_BUS,
    v3_area = DEFAULT_TOTAL_AREA_EO_CDU,
    v4_area = DEFAULT_TOTAL_AREA_OA_PARKMALL;

    int
    v1_areat = DEFAULT_TOTAL_AREA_PACIFIC_UNA,
    v2_areat = DEFAULT_TOTAL_AREA_PACIFIC_JPR,
    v3_areat = DEFAULT_TOTAL_AREA_PACIFIC_MCB;

    uint average_countl1[3],average_countl2[3],average_countl3[3],average_countl4[3];


    int activate_areas_legend;  // CHANGED HERE
    int activate_debug_btn;

};

#endif // MAINWINDOW_H
