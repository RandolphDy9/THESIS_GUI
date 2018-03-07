#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QElapsedTimer>
#include <QTime>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect.hpp>


#define CLASSIFIER_FW_PATH_1  "C:\\img\\haar\\low_reso_classifiers\\myhaar-fw-12.xml"
#define CLASSIFIER_FW_PATH_2  "C:\\img\\haar\\low_reso_classifiers\\myhaar-CDU1616.xml"
#define CLASSIFIER_FW_PATH_3  "C:\\img\\haar\\low_reso_classifiers\\myhaar-CDU2424.xml"
#define CLASSIFIER_FW_PATH_4  "C:\\img\\haar\\low_reso_classifiers\\myhaar-pmall2424.xml"
#define CLASSIFIER_M_PATH   "C:\\img\\haar\\low_reso_classifiers\\myhaar-mt-8.xml"
#define CLASSIFIER_T_PATH   "C:\\img\\haar\\low_reso_classifiers\\myhaar-t2424-6.xml" //TRUCK_2.xml

#define CLASSIFIER_NIGHT_FW_PATH  "C:\\img\\haar\\myhaar-n2424-4.xml"
#define CLASSIFIER_NIGHT_MT_PATH   "C:\\img\\haar\\myhaar-nm2424-3.xml"
#define CLASSIFIER_NIGHT_T_PATH   "C:\\img\\haar\\low_reso_classifiers\\TRUCK_N_1.xml"

#define CLASSIFIER_FW_LOWLIGHT "C:\\img\\haar\\low_reso_classifiers\\TRUCK_N_1.xml"
#define FW_AREA 303 //303
#define MT_AREA 151
#define TR_AREA 377

#define M2_TO_F2 10.761
#define DEFAULT_TOTAL_AREA_OA_CEBU 7123 //7850  //5454
#define DEFAULT_TOTAL_AREA_EO_BUS 4617 //6397 ////6051
#define DEFAULT_TOTAL_AREA_EO_CDU 4302 //5967 ////9446
#define DEFAULT_TOTAL_AREA_OA_PARKMALL 6835 //9491////8152

#define DEFAULT_TOTAL_AREA_PACIFIC_MCB 2142
#define DEFAULT_TOTAL_AREA_PACIFIC_JPR 2448
#define DEFAULT_TOTAL_AREA_PACIFIC_UNA 3366

#define LOW_PERCENT 0.33
#define MOD_PERCENT 0.66

#define MAX_NEIGHBOR_DAY_FW 15
#define MAX_NEIGHBOR_NIGHT_FW 15

#define MAX_NEIGHBOR_DAY_MT 20
#define MAX_NEIGHBOR_NIGHT_MT 10

#define MAX_NEIGHBOR_DAY_TR 20
#define MAX_NEIGHBOR_NIGHT_TR 15


#define SCALE_FACTOR_DETECTOR 1.1
#define SCALE_FACTOR_TRUCK 1.1
#define SCALE_FACTOR_MOTOR 1.1

#define FW_MAX_B 500,500
#define MT_MAX_B 150,200
#define TR_MAX_B 2000,2000

#define FW_MIN_B 32,32
#define MT_MIN_B 32,64
#define TR_MIN_B 250,250

#define FW_MAX_T 300,300
#define MT_MAX_T 150,200
#define TR_MAX_T 2000,2000

#define FW_MIN_T 32,32
#define MT_MIN_T 32,64
#define TR_MIN_T 250,250

typedef struct ProcessedFrame
{
    QImage original[4];
    QImage processed[4];
    int numberOfFW[4];
    int numberOfTR[4];
    int numberOfMT[4];
}PF_t; Q_DECLARE_METATYPE(ProcessedFrame)

typedef struct CropImageCoordinates
{
    int x1[2]; int y1[2];
    int x2[2]; int y2[2];
    int x3[2]; int y3[2];
    int x4[2]; int y4[2];

}CROP_COORDINATES_t;
typedef struct DetectionConfig
{
    cv::CascadeClassifier classifierFW[4];
    cv::CascadeClassifier classifierMT[4];
    cv::CascadeClassifier classifierTR[4];

    cv::CascadeClassifier classifierFW_n;
    cv::CascadeClassifier classifierMT_n;
    cv::CascadeClassifier classifierTR_n;

    cv::Size minSize_fw;
    cv::Size maxSize_fw;

    cv::Size minSize_tr;
    cv::Size maxSize_tr;

    cv::Size minSize_mt;
    cv::Size maxSize_mt;

    double resizeMultiplier;

    bool toggleDetect;


}Detection_CFG; Q_DECLARE_METATYPE(DetectionConfig)

struct JunctionPointsROI
{
    std::vector<cv::Point> pSrc1,pSrc2,pSrc3,pSrc4;
}; Q_DECLARE_METATYPE(JunctionPointsROI)

class OpenCvWorker : public QObject
{
    Q_OBJECT
    mutable QMutex m_mutex;
private:
    cv::Mat maskROI[4];
    cv::Mat matFrameOriginal[4];
    cv::Mat matFrameProcessed[4];
    cv::Mat matFrameDraw[4];
    cv::Mat *bgr;
    cv::VideoCapture *stream[4];

    CropImageCoordinates crop;

    Detection_CFG cfg;
    ProcessedFrame *outputFrames;
    JunctionPointsROI *jpROI;
    QString src1,src2,src3,src4;

    void cleanBoundingBox(std::vector<cv::Rect> &fw, std::vector<cv::Rect> &mt, std::vector<cv::Rect> &nmt);
    bool night_flag;
    bool flag_receive_roi;

    bool classifier_flag_fw;
    bool classifier_flag_mt;
    bool classifier_flag_tr;

    bool flagMaskCreated;
    bool toggleStream;
    bool toggleDebug;
    bool startProcess;

    void process();
    void resize();
    void cvtQimage();
    void createMask();

    bool t_flag = false;
    bool b_flag = false;
    bool low_reso = false;
    std::vector<int> skip_these;
    cv::Size min_fw,min_tr,min_mt,max_fw,max_tr,max_mt;


public:
    explicit OpenCvWorker(QObject *parent = 0);
    ~OpenCvWorker();
signals:
    void sendFrame(ProcessedFrame* pf);
    void sendPointsToSave(JunctionPointsROI);
public slots:
    void findMax();
    void receiveGrabFrame();
    void receiveToggleStream(bool);
    void receiveSetupCFG(DetectionConfig _cfg);
    void receiveToggleDebug();
    void receiveSrc1(QString);
    void receiveSrc2(QString);
    void receiveSrc3(QString);
    void receiveSrc4(QString);
    void receiveROI(JunctionPointsROI);
    void receiveRequestROI_1();
    void receiveRequestROI_2();
    void receiveRequestROI_3();
    void receiveRequestROI_4();
    void receiveNightToggle();
    void receiveClassifierFWenable(bool);
    void receiveClassifierMTenable(bool);
    void receiveClassifierTRenable(bool);
    void receiveClassifierPath(QString,int);
    void receiveFlagROI();
    void receiveChangeInMinMax(int _min_size, int _max_size, int id);
    void receiveEnableT();
    void receiveEnableB();
};

#endif // IMAGE_PROC_H
