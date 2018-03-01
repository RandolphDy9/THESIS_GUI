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
#
#define SCALE_FACTOR 1.1

#define CLASSIFIER_FW_PATH  "C:\\img\\haar\\myhaar-2424-11.xml"
#define CLASSIFIER_M_PATH   "C:\\img\\haar\\myhaar-m2424-7.xml"//FINAL-MT.xml
#define CLASSIFIER_T_PATH   "C:\\img\\haar\\myhaar-tr-5.xml"

#define CLASSIFIER_NIGHT_FW_PATH  "C:\\img\\haar\\myhaar-n2424-3.xml"
#define CLASSIFIER_NIGHT_MT_PATH   "C:\\img\\haar\\classifier-n-MT.xml"
#define CLASSIFIER_NIGHT_T_PATH   "C:\\img\\haar\\classifier-n-TR.xml"

#define CLASSIFIER_FW_LOWLIGHT "C:\\img\\haar\\myhaar-low-light.xml"
#define FW_AREA 303
#define MT_AREA 151
#define TR_AREA 377

#define M2_TO_F2 10.761
#define DEFAULT_TOTAL_AREA_OA_CEBU 5454
#define DEFAULT_TOTAL_AREA_EO_BUS 6051
#define DEFAULT_TOTAL_AREA_EO_CDU 9446
#define DEFAULT_TOTAL_AREA_OA_PARKMALL 8152

#define DEFAULT_TOTAL_AREA_PACIFIC_MCB 4401
#define DEFAULT_TOTAL_AREA_PACIFIC_JPR 3166
#define DEFAULT_TOTAL_AREA_PACIFIC_UNA 2740

#define LOW_PERCENT 0.333333
#define MOD_PERCENT 0.666666

#define DEFAULT_SNR_AREA_OA_CEBU_LOW 2967
#define DEFAULT_SNR_AREA_OA_CEBU_MOD 5935
#define DEFAULT_SNR_AREA_OA_CEBU_HIGH 600

#define DEFAULT_SNR_AREA_EO_BUS_LOW 2016
#define DEFAULT_SNR_AREA_EO_BUS_MOD 4033
#define DEFAULT_SNR_AREA_EO_BUS_HIGH 600

#define DEFAULT_SNR_AREA_OA_PARKMALL_LOW  3090
#define DEFAULT_SNR_AREA_OA_PARKMALL_MOD 6181
#define DEFAULT_SNR_AREA_OA_PARKMALL_HIGH 1686

#define DEFAULT_SNR_AREA_EO_CDU_LOW 3148
#define DEFAULT_SNR_AREA_EO_CDU_MOD 6297
#define DEFAULT_SNR_AREA_EO_CDU_HIGH 321

#define DEFAULT_AREA_PACIFIC_MCB_LOW 1055
#define DEFAULT_AREA_PACIFIC_MCB_MOD 2110
#define DEFAULT_AREA_PACIFIC_MCB_HIGH 440

#define DEFAULT_AREA_PACIFIC_JPR_LOW 913
#define DEFAULT_AREA_PACIFIC_JPR_MOD 1826
#define DEFAULT_AREA_PACIFIC_JPR_HIGH 316

#define DEFAULT_AREA_PACIFIC_UNA_LOW 1467
#define DEFAULT_AREA_PACIFIC_UNA_MOD 2934
#define DEFAULT_AREA_PACIFIC_UNA_HIGH 274

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
    cv::CascadeClassifier classifierFW;
    cv::CascadeClassifier classifierMT;
    cv::CascadeClassifier classifierTR;

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

    std::vector<int> skip_these;
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
};

#endif // IMAGE_PROC_H
