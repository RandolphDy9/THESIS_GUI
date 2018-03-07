#include "image_proc.h"
#include "roi_hci.hpp"

OpenCvWorker::OpenCvWorker(QObject *parent) :
    QObject(parent),
    toggleStream(false),
    toggleDebug(false),
    flagMaskCreated(false),
    startProcess(false),
    classifier_flag_fw(false),
    classifier_flag_mt(false),
    classifier_flag_tr(false),
    flag_receive_roi(false),
    low_reso(false)

{
    src1="";
    src2="";
    src3="";
    src4="";

    jpROI = new JunctionPointsROI;

    stream[0] = new cv::VideoCapture();
    stream[1] = new cv::VideoCapture();
    stream[2] = new cv::VideoCapture();
    stream[3] = new cv::VideoCapture();
    crop = {};
    cfg = {};

//    cfg.classifierFW.load(CLASSIFIER_FW_PATH);
//    cfg.classifierMT.load(CLASSIFIER_M_PATH);
//    cfg.classifierTR.load(CLASSIFIER_T_PATH);

    cfg.classifierFW_n.load(CLASSIFIER_NIGHT_FW_PATH);
    cfg.classifierMT_n.load(CLASSIFIER_NIGHT_MT_PATH);
    cfg.classifierTR_n.load(CLASSIFIER_NIGHT_T_PATH);


    cfg.resizeMultiplier = 1;


    cfg.toggleDetect = true;

    night_flag = false;

    bgr = new cv::Mat[3];

    outputFrames = new ProcessedFrame();
    qRegisterMetaType(outputFrames);



}
OpenCvWorker::~OpenCvWorker()
{
    if(stream[0]->isOpened()) stream[0]->release();
    if(stream[1]->isOpened()) stream[1]->release();
    if(stream[2]->isOpened()) stream[2]->release();
    if(stream[3]->isOpened()) stream[3]->release();
    delete stream;
}
void OpenCvWorker::receiveGrabFrame()
{
//    QMutexLocker locker(&m_mutex);

    if(toggleStream == false) return;

    if(stream[0]->isOpened() == true){
        (*stream[0]) >> matFrameOriginal[0];
    }
    if(stream[1]->isOpened() == true){
        (*stream[1]) >> matFrameOriginal[1];
    }
    if(stream[2]->isOpened() == true){
        (*stream[2]) >> matFrameOriginal[2];
    }
    if(stream[3]->isOpened() == true){
        (*stream[3]) >> matFrameOriginal[3];
    }

    if(flagMaskCreated == false){
        createMask();
        findMax();
        flagMaskCreated = true;
    }

    if(flagMaskCreated == true){

        if(matFrameOriginal[0].empty() == false){
            matFrameDraw[0] = maskROI[0] & matFrameOriginal[0];
            matFrameProcessed[0] = matFrameDraw[0].colRange(crop.x1[0],crop.x1[1]);
            matFrameProcessed[0] = matFrameProcessed[0].rowRange(crop.y1[0],crop.y1[1]);


        }
        if(matFrameOriginal[1].empty() == false ){
            matFrameDraw[1] = maskROI[1] & matFrameOriginal[1];
            matFrameProcessed[1] = matFrameDraw[1].colRange(crop.x2[0],crop.x2[1]);
            matFrameProcessed[1] = matFrameProcessed[1].rowRange(crop.y2[0],crop.y2[1]);
            ;
        }
        if(matFrameOriginal[2].empty() == false){
            matFrameDraw[2] = maskROI[2] & matFrameOriginal[2];
            matFrameProcessed[2] = matFrameDraw[2].colRange(crop.x3[0],crop.x3[1]);
            matFrameProcessed[2] = matFrameProcessed[2].rowRange(crop.y3[0],crop.y3[1]);

        }
        if(matFrameOriginal[3].empty() == false){
            matFrameDraw[3] = maskROI[3] & matFrameOriginal[3];
            matFrameProcessed[3] = matFrameDraw[3].colRange(crop.x4[0],crop.x4[1]);
            matFrameProcessed[3] = matFrameProcessed[3].rowRange(crop.y4[0],crop.y4[1]);

        }
    }

    if(cfg.toggleDetect == true){
        if(startProcess == true){
            QTime timer2;
            timer2.start();
//            QtConcurrent::run(this,&OpenCvWorker::process);
            process();

        }
        startProcess = true;
    }

        cvtQimage();
        emit sendFrame(outputFrames);

}

void OpenCvWorker::receiveToggleStream(bool stream)
{
    toggleStream = stream;

}
void OpenCvWorker::process()
{



    static std::vector<cv::Rect> rectFW, rectTR, rectMT,nMT,nFW,nTR;
    cv::Mat skipFrame;

    for(int i = 0; i < 4; i++)
    {
        if(t_flag){
            switch(i){
            case 0:
                max_fw = cv::Size(500,500);
                max_mt = cv::Size(64,64);
                max_tr = cv::Size(1000,1000);

                min_fw = cv::Size(4,4);
                min_mt = cv::Size(16,16);
                min_tr = cv::Size(200,200);
                break;
            case 1:
                max_fw = cv::Size(500,500);
                max_mt = cv::Size(200,200);
                max_tr = cv::Size(1000,1000);

                min_fw = cv::Size(70,70);
                min_mt = cv::Size(24,24);
                min_tr = cv::Size(120,120);
                break;
            case 2:
                max_fw = cv::Size(1000,1000);
                max_mt = cv::Size(200,200);
                max_tr = cv::Size(1000,1000);

                min_fw = cv::Size(70,70);
                min_mt = cv::Size(64,64);
                min_tr = cv::Size(200,200);
                break;

            }
        }else{
            switch(i){
            case 0:
                max_fw = cv::Size(500,500);
                max_mt = cv::Size(300,300);
                max_tr = cv::Size(553,553);

                min_fw = cv::Size(64,64);
                min_mt = cv::Size(24,24);
                min_tr = cv::Size(304,304);
                break;
            case 1:
                max_fw = cv::Size(500,500);
                max_mt = cv::Size(200,200);
                max_tr = cv::Size(1280,1280);

                min_fw = cv::Size(70,70);
                min_mt = cv::Size(48,48);
                min_tr = cv::Size(248,248);
                break;
            case 2:
                max_fw = cv::Size(500,500);
                max_mt = cv::Size(200,200);
                max_tr = cv::Size(1280,1280);

                min_fw = cv::Size(24,24);
                min_mt = cv::Size(48,48);
                min_tr = cv::Size(242,242);
                break;
            case 3:
                max_fw = cv::Size(1000,1000);
                max_mt = cv::Size(200,200);
                max_tr = cv::Size(430,430);

                min_fw = cv::Size(64,64);
                min_mt = cv::Size(64,64);
                min_tr = cv::Size(180,180);
                break;

            }
        }
        if(matFrameDraw[i].empty() == false && matFrameOriginal[i].empty() == false)
        {

            if(cfg.resizeMultiplier != 1)
            {
                cv::Size res(cvRound(matFrameProcessed[i].cols * cfg.resizeMultiplier),
                             cvRound(matFrameProcessed[i].rows * cfg.resizeMultiplier));
                cv::resize(matFrameProcessed[i],matFrameProcessed[i],
                           res);
                cv::cvtColor(matFrameProcessed[i],matFrameProcessed[i],cv::COLOR_BGR2GRAY);
            }
            cv::cvtColor(matFrameProcessed[i],matFrameProcessed[i],cv::COLOR_BGR2GRAY);
            if(night_flag == false)
                cv::equalizeHist(matFrameProcessed[i],matFrameProcessed[i]);
            if(classifier_flag_fw == true){
                if(cfg.classifierFW[i].empty() == false)
                {
                    if(night_flag == false){

                            cfg.classifierFW[i].detectMultiScale(matFrameProcessed[i],rectFW,SCALE_FACTOR_DETECTOR,15,
                                                              0 | CV_HAAR_SCALE_IMAGE,min_fw,max_fw);
                    }else{

                        if(low_reso == true)
                            cfg.classifierFW_n.detectMultiScale(matFrameProcessed[i],rectFW,SCALE_FACTOR_DETECTOR,20,
                                                                0 | CV_HAAR_SCALE_IMAGE,cv::Size(32,32),cv::Size(150,150));
                        else
                            cfg.classifierFW_n.detectMultiScale(matFrameProcessed[i],rectFW,SCALE_FACTOR_DETECTOR,20,
                                                                0 | CV_HAAR_SCALE_IMAGE,cv::Size(70,70),cv::Size(200,200));
                    }
                    std::vector<int> del_f;
                    del_f.clear();
                    outputFrames->numberOfFW[i] = rectFW.size();
                    if(toggleDebug == true){
                        for(int j = 0; j < rectFW.size() ;j++)
                        {
                            switch(i){
                            case 0:
                                (rectFW)[j].x += crop.x1[0];
                                (rectFW)[j].y += crop.y1[0];
                                break;
                            case 1:
                                (rectFW)[j].x += crop.x2[0];
                                (rectFW)[j].y += crop.y2[0];
                                break;
                            case 2:
                                (rectFW)[j].x += crop.x3[0];
                                (rectFW)[j].y += crop.y3[0];
                                break;
                            case 3:
                                (rectFW)[j].x += crop.x4[0];
                                (rectFW)[j].y += crop.y4[0];
                                break;
                            }

                            cv::Point center = cv::Point(rectMT[j].x + rectMT[j].width/2, rectMT[j].y + rectMT[j].height/2);
                            cv::Point br_thresh = cv::Point(rectMT[j].br().x, rectMT[j].br().y - 15);
                            for(int fw_size = 0; fw_size < rectFW.size(); fw_size++){
                                if(rectFW[fw_size].contains(rectFW[j].br()) == true && rectFW[fw_size].contains(rectFW[j].tl()) == true && j != fw_size){
                                    del_f.push_back(j);
                                }
                            }


                        }

                        nFW = rectFW;
                        for(int k = 0; k < del_f.size();k++){
                            nFW.at(del_f[k]).x = 0;
                            nFW.at(del_f[k]).y = 0;
                        }

                        for(int k = 0; k < nFW.size();k++){
                            if(nFW[k].x == 0 && nFW[k].y == 0){
                                outputFrames->numberOfFW[i] --;
                            }else
                                cv::rectangle(matFrameDraw[i],(rectFW)[k],cv::Scalar(0,0,255),8);
                        }



                    }

                }
            }else{
                outputFrames->numberOfFW[i] = 0;
            }
            if(classifier_flag_mt == true){
                if(cfg.classifierMT[i].empty() == false)
                {
                    if(night_flag == false){
                        cfg.classifierMT[i].detectMultiScale(matFrameProcessed[i],rectMT,SCALE_FACTOR_MOTOR,MAX_NEIGHBOR_DAY_MT,
                                                          0 | CV_HAAR_SCALE_IMAGE,min_mt,max_mt);
                    }else{
                        cfg.classifierMT_n.detectMultiScale(matFrameProcessed[i],rectMT,SCALE_FACTOR_MOTOR,MAX_NEIGHBOR_NIGHT_MT,
                                                            0 | CV_HAAR_SCALE_IMAGE,cv::Size(100,100),cv::Size(300,300));
                    }
                    outputFrames->numberOfMT[i] = rectMT.size();
                    nMT.clear();

                    std::vector<int> del_i;

                    if(toggleDebug == true){
                        for(int j = 0; j < rectMT.size() ;j++)
                        {
                            switch(i){
                            case 0:
                                (rectMT)[j].x += crop.x1[0];
                                (rectMT)[j].y += crop.y1[0];
                                break;
                            case 1:
                                (rectMT)[j].x += crop.x2[0];
                                (rectMT)[j].y += crop.y2[0];
                                break;
                            case 2:
                                (rectMT)[j].x += crop.x3[0];
                                (rectMT)[j].y += crop.y3[0];
                                break;
                            case 3:
                                (rectMT)[j].x += crop.x4[0];
                                (rectMT)[j].y += crop.y4[0];
                                break;
                            }
                            cv::Point center = cv::Point(rectMT[j].x + rectMT[j].width/2, rectMT[j].y + rectMT[j].height/2);
                            cv::Point br_thresh = cv::Point(rectMT[j].br().x, rectMT[j].br().y - 15);
                            for(int fw_size = 0; fw_size < rectFW.size(); fw_size++){

                                if(rectFW[fw_size].contains(center) == true){
                                    del_i.push_back(j);
                                }
                            }
                        }
                        nMT = rectMT;
                        for(int k = 0; k < del_i.size();k++){
                            nMT.at(del_i[k]).x = 0;
                            nMT.at(del_i[k]).y = 0;
                        }

                        for(int k = 0; k < nMT.size();k++){
                            if(nMT[k].x == 0 && nMT[k].y == 0){
                                outputFrames->numberOfMT[i] --;
                            }else
                                cv::rectangle(matFrameDraw[i],(nMT)[k],cv::Scalar(0,255,0),8);
                        }
                         qDebug() <<"SIZE" << outputFrames->numberOfMT[i];
                    }

                }
            }else{
                outputFrames->numberOfMT[i] = 0;
            }

            if(classifier_flag_tr == true){
                if(cfg.classifierTR[i].empty() == false){
                    if(night_flag == false){
                        cfg.classifierTR[i].detectMultiScale(matFrameProcessed[i],rectTR,SCALE_FACTOR_TRUCK,MAX_NEIGHBOR_DAY_TR,
                                                          0 | CV_HAAR_SCALE_IMAGE,min_tr,max_tr);
                    }else{
                        cfg.classifierMT_n.detectMultiScale(matFrameProcessed[i],rectMT,SCALE_FACTOR_TRUCK,MAX_NEIGHBOR_NIGHT_TR,
                                                            0 | CV_HAAR_SCALE_IMAGE,min_tr,max_tr);
                    }
                    outputFrames->numberOfTR[i] = rectTR.size();
                    if(toggleDebug == true){
                        for(int j = 0; j < rectTR.size() ;j++)
                        {
                            switch(i){
                            case 0:
                                (rectTR)[j].x += crop.x1[0];
                                (rectTR)[j].y += crop.y1[0];
                                break;
                            case 1:
                                (rectTR)[j].x += crop.x2[0];
                                (rectTR)[j].y += crop.y2[0];
                                break;
                            case 2:
                                (rectTR)[j].x += crop.x3[0];
                                (rectTR)[j].y += crop.y3[0];
                                break;
                            case 3:
                                (rectTR)[j].x += crop.x4[0];
                                (rectTR)[j].y += crop.y4[0];
                                break;
                            }
                            for(int k = 0; k < rectFW.size();k++){
                                cv::Point tr_center = cv::Point(rectFW[k].x + rectFW[k].width/2, rectFW[k].y + rectFW[k].height/2);
                                if(rectTR[j].contains(tr_center)){
                                    rectTR[j].x = 0;
                                    rectTR[j].y = 0;
                                }
                            }
                        }
                        for(int k = 0; k < rectTR.size();k++){
                            if(rectTR[k].x == 0 && rectTR[k].y == 0){
                                outputFrames->numberOfTR[i] --;
                            }else{
                                rectangle(matFrameDraw[i],rectTR[k],cv::Scalar(255,0,0),8);
                            }
                        }

                    }

                }
            }else{
                outputFrames->numberOfTR[i] = 0;
            }
        }

    }


}
void OpenCvWorker::receiveSetupCFG(DetectionConfig _cfg)
{

//    cfg.maxSize_fw = _cfg.maxSize_fw;
//    cfg.maxSize_mt = _cfg.maxSize_mt;
//    cfg.maxSize_tr = _cfg.maxSize_tr;

//    cfg.minSize_fw = _cfg.minSize_fw;
//    cfg.minSize_mt = _cfg.minSize_mt;
//    cfg.minSize_tr = _cfg.minSize_tr;

}
void OpenCvWorker::cvtQimage()
{


    for(int i = 0; i < 4; i++)
    {
        if(matFrameOriginal[i].empty() == false){
            //cv::resize(matFrameOriginal[i],matFrameOriginal[i],matFrameOriginal[i].size() / 8);
            cv::cvtColor(matFrameOriginal[i],matFrameOriginal[i],cv::COLOR_BGR2RGB);
            outputFrames->original[i] = QImage((const unsigned char *)matFrameOriginal[i].data,
                           matFrameOriginal[i].cols,matFrameOriginal[i].rows,
                                               QImage::Format_RGB888).scaledToHeight(361).scaledToWidth(551);

            cv::cvtColor(matFrameDraw[i],matFrameDraw[i],cv::COLOR_BGR2RGB);
            outputFrames->processed[i] = QImage((const unsigned char *)matFrameDraw[i].data,
                       matFrameDraw[i].cols,matFrameDraw[i].rows,
                       QImage::Format_RGB888).scaledToHeight(361).scaledToWidth(551);
        }

    }

}

void OpenCvWorker::receiveToggleDebug()
{
    QMutexLocker lock(&m_mutex);
    toggleDebug = !toggleDebug;

}

void OpenCvWorker::receiveSrc1(QString src)
{
    QMutexLocker locker(&m_mutex);
    src1 = src;
    if(stream[0]->isOpened() == false){
        stream[0] = new cv::VideoCapture(src.toStdString());
    }else{
        stream[0]->release();
        stream[0]->open(src.toStdString());
        //(*stream[0]) >> matFrameOriginal[0];
    }
    flagMaskCreated = false;
    return;


}
void OpenCvWorker::receiveSrc2(QString src)
{
    QMutexLocker locker(&m_mutex);
    src2 = src;
    if(stream[1]->isOpened() == false){
        stream[1] = new cv::VideoCapture(src.toStdString());
    }else{
        stream[1]->release();
        stream[1]->open(src.toStdString());
    }
    flagMaskCreated = false;
    return;

}
void OpenCvWorker::receiveSrc3(QString src)
{
    QMutexLocker locker(&m_mutex);
    src3 = src;
    if(stream[2]->isOpened() == false){
        stream[2] = new cv::VideoCapture(src.toStdString());
    }else{
        stream[2]->release();
        stream[2]->open(src.toStdString());
    }
    flagMaskCreated = false;
    return;
}
void OpenCvWorker::receiveSrc4(QString src)
{
    QMutexLocker locker(&m_mutex);
    src4 = src;
    if(stream[3]->isOpened() == false){
        stream[3] = new cv::VideoCapture(src.toStdString());
    }else{
        stream[3]->release();
        stream[3]->open(src.toStdString());
    }
    flagMaskCreated = false;
    return;
}

void OpenCvWorker::receiveROI(JunctionPointsROI _jpROI)
{
    QMutexLocker locker(&m_mutex);
    jpROI->pSrc1 = _jpROI.pSrc1;
    jpROI->pSrc2 = _jpROI.pSrc2;
    jpROI->pSrc3 = _jpROI.pSrc3;
    jpROI->pSrc4 = _jpROI.pSrc4;
    flagMaskCreated = 0;



}

void OpenCvWorker::createMask()
{


    cv::Mat mask1(matFrameOriginal[0].size(),CV_8UC3);
    cv::Mat mask2(matFrameOriginal[1].size(),CV_8UC3);
    cv::Mat mask3(matFrameOriginal[2].size(),CV_8UC3);
    cv::Mat mask4(matFrameOriginal[3].size(),CV_8UC3);

    mask1 = cv::Mat::zeros(mask1.size(),CV_8UC3);
    mask2 = cv::Mat::zeros(mask2.size(),CV_8UC3);
    mask3 = cv::Mat::zeros(mask3.size(),CV_8UC3);
    mask4 = cv::Mat::zeros(mask4.size(),CV_8UC3);

    if(flag_receive_roi == false){

        /*****************************/
        if(matFrameOriginal[0].cols != jpROI->pSrc1.at(2).x){
            qDebug() << matFrameOriginal[0].rows;
            jpROI->pSrc1.at(1).x = matFrameOriginal[0].cols;
            jpROI->pSrc1.at(2).x = matFrameOriginal[0].cols;
            low_reso = true;
        }
        if(matFrameOriginal[0].rows != jpROI->pSrc1.at(2).y){
            jpROI->pSrc1.at(2).y = matFrameOriginal[0].rows;
            jpROI->pSrc1.at(3).y = matFrameOriginal[0].rows;
            low_reso = true;
        }

        if(matFrameOriginal[1].cols != jpROI->pSrc2.at(2).x){
            jpROI->pSrc2.at(1).x = matFrameOriginal[1].cols;
            jpROI->pSrc2.at(2).x = matFrameOriginal[1].rows;
            low_reso = true;
        }
        if(matFrameOriginal[1].rows != jpROI->pSrc2.at(2).y){
            jpROI->pSrc2.at(2).y = matFrameOriginal[1].rows;
            jpROI->pSrc2.at(3).y = matFrameOriginal[1].rows;
            low_reso = true;
        }


        if(matFrameOriginal[2].cols != jpROI->pSrc3.at(2).x){
            jpROI->pSrc3.at(1).x = matFrameOriginal[2].cols;
            jpROI->pSrc3.at(2).x = matFrameOriginal[2].cols;
            low_reso = true;
        }
        if(matFrameOriginal[1].rows != jpROI->pSrc3.at(2).y){
            jpROI->pSrc3.at(2).y = matFrameOriginal[2].rows;
            jpROI->pSrc3.at(3).y = matFrameOriginal[2].rows;
            low_reso = true;
        }

        if(matFrameOriginal[3].cols != jpROI->pSrc4.at(2).x){
            jpROI->pSrc4.at(1).x = matFrameOriginal[3].cols;
            jpROI->pSrc4.at(2).x = matFrameOriginal[3].cols;
            low_reso = true;
        }
        if(matFrameOriginal[3].rows != jpROI->pSrc4.at(2).y){
            jpROI->pSrc4.at(2).y = matFrameOriginal[3].rows;
            jpROI->pSrc4.at(3).y = matFrameOriginal[3].rows;
            low_reso = true;
        }

        cfg.minSize_fw = cv::Size(24,24);
    }else{
        low_reso = false;
        cfg.minSize_fw = cv::Size(64,64);
    }

    /*************************/
    std::vector<cv::Point> ROI_polySrc1,ROI_polySrc2,ROI_polySrc3,ROI_polySrc4;
    if(jpROI->pSrc1.empty() == false){
        cv::approxPolyDP(jpROI->pSrc1, ROI_polySrc1, 1.0, true);
        cv::fillConvexPoly(mask1, &ROI_polySrc1[0], ROI_polySrc1.size(), cv::Scalar(255,255,255), 8, 0);
        maskROI[0] = mask1.clone();
    }
    if(jpROI->pSrc2.empty() == false){
        cv::approxPolyDP(jpROI->pSrc2, ROI_polySrc2, 1.0, true);
        cv::fillConvexPoly(mask2, &ROI_polySrc2[0], ROI_polySrc2.size(), cv::Scalar(255,255,255), 8, 0);
        maskROI[1] = mask2.clone();
    }
    if(jpROI->pSrc3.empty() == false){
        cv::approxPolyDP(jpROI->pSrc3, ROI_polySrc3, 1.0, true);
        cv::fillConvexPoly(mask3, &ROI_polySrc3[0], ROI_polySrc3.size(), cv::Scalar(255,255,255), 8, 0);
        maskROI[2] = mask3.clone();
    }
    if(jpROI->pSrc4.empty() == false){
        cv::approxPolyDP(jpROI->pSrc4, ROI_polySrc4, 1.0, true);
        cv::fillConvexPoly(mask4, &ROI_polySrc4[0], ROI_polySrc4.size(), cv::Scalar(255,255,255), 8, 0);
        maskROI[3] = mask4.clone();
    }



}

void OpenCvWorker::receiveRequestROI_1()
{
    QMutexLocker locker(&m_mutex);
    ROI roi;
    cv::Mat tmp;
    char *src = "SOURCE 1";
    char choice = '\0';
    (*stream[0]) >> tmp;
    cv::namedWindow(src,CV_WINDOW_NORMAL);
    imshow(src,tmp);
    roi.set(src);
    while(choice != 'c'){

        if(roi.p1 == true && roi.p2 != true){
           cv::Mat matP1 = tmp.clone();
           line(matP1,roi.corners[0][0],roi.temporary,cv::Scalar(0,255,0),8);
           imshow(src,matP1);
        }
        if(roi.p2 == true && roi.p1 == true && roi.p3 != true){
            //draw p2 - mouse pointer
            cv::Mat matP2 = tmp.clone();
            line(matP2,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP2,roi.corners[0][1],roi.temporary,cv::Scalar(0,255,0),8);
            imshow(src,matP2);
        }
        if(roi.p3 == true && roi.p2 == true && roi.p1 == true && roi.p4 != true){
            //draw p3 - mouse pointer
            cv::Mat matP3 = tmp.clone();
            line(matP3,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][2],roi.temporary,cv::Scalar(0,255,0),8);
            line(matP3,roi.temporary,roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP3);
        }
        if(roi.p4 == true && roi.p2 == true && roi.p3 == true && roi.p1 == true){
            //draw p4 - p1
            cv::Mat matP4 = tmp.clone();
            line(matP4,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][2],roi.corners[0][3],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][3],roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP4);
        }

        choice = cv::waitKey(10);
    }
    cv::destroyWindow(src);
    jpROI->pSrc1 = {roi.corners[0][0],roi.corners[0][1],roi.corners[0][2],roi.corners[0][3]};
    flagMaskCreated = false;
    emit sendPointsToSave(*jpROI);
    return;
}
void OpenCvWorker::receiveRequestROI_2()
{
    ROI roi;

    cv::Mat tmp;
    char *src = "SOURCE 2";
    char choice = '\0';
    (*stream[1]) >> tmp;

    cv::namedWindow(src,CV_WINDOW_NORMAL);
    imshow(src,tmp);
    roi.set(src);
    while(choice != 'c'){

        if(roi.p1 == true && roi.p2 != true){
           cv::Mat matP1 = tmp.clone();
           line(matP1,roi.corners[0][0],roi.temporary,cv::Scalar(0,255,0),8);
           imshow(src,matP1);
        }
        if(roi.p2 == true && roi.p1 == true && roi.p3 != true){
            //draw p2 - mouse pointer
            cv::Mat matP2 = tmp.clone();
            line(matP2,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP2,roi.corners[0][1],roi.temporary,cv::Scalar(0,255,0),8);
            imshow(src,matP2);
        }
        if(roi.p3 == true && roi.p2 == true && roi.p1 == true && roi.p4 != true){
            //draw p3 - mouse pointer
            cv::Mat matP3 = tmp.clone();
            line(matP3,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][2],roi.temporary,cv::Scalar(0,255,0),8);
            line(matP3,roi.temporary,roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP3);
        }
        if(roi.p4 == true && roi.p2 == true && roi.p3 == true && roi.p1 == true){
            //draw p4 - p1
            cv::Mat matP4 = tmp.clone();
            line(matP4,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][2],roi.corners[0][3],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][3],roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP4);
        }

        choice = cv::waitKey(10);
    }
    cv::destroyWindow(src);
    jpROI->pSrc2 = {roi.corners[0][0],roi.corners[0][1],roi.corners[0][2],roi.corners[0][3]};
    flagMaskCreated = false;
    emit sendPointsToSave(*jpROI);
}
void OpenCvWorker::receiveRequestROI_3()
{
    ROI roi;

    cv::Mat tmp;
    char *src = "SOURCE 3";
    char choice = '\0';
    (*stream[2]) >> tmp;
    cv::namedWindow(src,CV_WINDOW_NORMAL);
    imshow(src,tmp);
    roi.set(src);
    while(choice != 'c'){

        if(roi.p1 == true && roi.p2 != true){
           cv::Mat matP1 = tmp.clone();
           line(matP1,roi.corners[0][0],roi.temporary,cv::Scalar(0,255,0),8);
           imshow(src,matP1);
        }
        if(roi.p2 == true && roi.p1 == true && roi.p3 != true){
            //draw p2 - mouse pointer
            cv::Mat matP2 = tmp.clone();
            line(matP2,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP2,roi.corners[0][1],roi.temporary,cv::Scalar(0,255,0),8);
            imshow(src,matP2);
        }
        if(roi.p3 == true && roi.p2 == true && roi.p1 == true && roi.p4 != true){
            //draw p3 - mouse pointer
            cv::Mat matP3 = tmp.clone();
            line(matP3,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][2],roi.temporary,cv::Scalar(0,255,0),8);
            line(matP3,roi.temporary,roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP3);
        }
        if(roi.p4 == true && roi.p2 == true && roi.p3 == true && roi.p1 == true){
            //draw p4 - p1
            cv::Mat matP4 = tmp.clone();
            line(matP4,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][2],roi.corners[0][3],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][3],roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP4);
        }

        choice = cv::waitKey(10);
    }
    cv::destroyWindow(src);
    jpROI->pSrc3 = {roi.corners[0][0],roi.corners[0][1],roi.corners[0][2],roi.corners[0][3]};
    flagMaskCreated = false;
    emit sendPointsToSave(*jpROI);
}
void OpenCvWorker::receiveRequestROI_4()
{

    ROI roi;

    cv::Mat tmp;
    char *src = "SOURCE 4";
    char choice = '\0';
    (*stream[3]) >> tmp;
    cv::namedWindow(src,CV_WINDOW_NORMAL);
    imshow(src,tmp);
    roi.set(src);
    while(choice != 'c'){

        if(roi.p1 == true && roi.p2 != true){
           cv::Mat matP1 = tmp.clone();
           line(matP1,roi.corners[0][0],roi.temporary,cv::Scalar(0,255,0),8);
           imshow(src,matP1);
        }
        if(roi.p2 == true && roi.p1 == true && roi.p3 != true){
            //draw p2 - mouse pointer
            cv::Mat matP2 = tmp.clone();
            line(matP2,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP2,roi.corners[0][1],roi.temporary,cv::Scalar(0,255,0),8);
            imshow(src,matP2);
        }
        if(roi.p3 == true && roi.p2 == true && roi.p1 == true && roi.p4 != true){
            //draw p3 - mouse pointer
            cv::Mat matP3 = tmp.clone();
            line(matP3,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP3,roi.corners[0][2],roi.temporary,cv::Scalar(0,255,0),8);
            line(matP3,roi.temporary,roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP3);
        }
        if(roi.p4 == true && roi.p2 == true && roi.p3 == true && roi.p1 == true){
            //draw p4 - p1
            cv::Mat matP4 = tmp.clone();
            line(matP4,roi.corners[0][0],roi.corners[0][1],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][1],roi.corners[0][2],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][2],roi.corners[0][3],cv::Scalar(0,255,0),8);
            line(matP4,roi.corners[0][3],roi.corners[0][0],cv::Scalar(0,255,0),8);
            imshow(src,matP4);
        }

        choice = cv::waitKey(10);
    }
    cv::destroyWindow(src);
    jpROI->pSrc4 = {roi.corners[0][0],roi.corners[0][1],roi.corners[0][2],roi.corners[0][3]};
    flagMaskCreated = false;
    emit sendPointsToSave(*jpROI);
}

void OpenCvWorker::findMax()
{
    int x1[2] = {0,0},x2[2] = {0,0},x3[2] = {0,0},x4[2] = {0,0};
    int y1[2] = {0,0},y2[2] = {0,0},y3[2] = {0,0},y4[2] = {0,0};

    for(int i = 0; i < 4;i++){
        for(int  j = 0; j < 4; j++){
            switch(i){
            case 0:
                if( jpROI->pSrc1.empty() == true)
                    break;
                if(j == 0){
                    x1[0] = jpROI->pSrc1[j].x;
                    y1[0] = jpROI->pSrc1[j].y;

                    x1[1] = jpROI->pSrc1[j].x;
                    y1[1] = jpROI->pSrc1[j].y;
                }else{

                    if(x1[0] > jpROI->pSrc1[j].x){
                        x1[0] = jpROI->pSrc1[j].x;
                    }
                    if(x1[1] < jpROI->pSrc1[j].x){
                        x1[1] = jpROI->pSrc1[j].x;
                    }

                    if(y1[0] > jpROI->pSrc1[j].y){
                        y1[0] = jpROI->pSrc1[j].y;
                    }
                    if(y1[1] < jpROI->pSrc1[j].y){
                        y1[1] = jpROI->pSrc1[j].y;
                    }

                }
                break;
            case 1:
                if( jpROI->pSrc2.empty() == true)
                    break;
                if(j == 0){
                    x2[0] = jpROI->pSrc2[j].x;
                    y2[0] = jpROI->pSrc2[j].y;

                    x2[1] = jpROI->pSrc2[j].x;
                    y2[1] = jpROI->pSrc2[j].y;
                }else{

                    if(x2[0] > jpROI->pSrc2[j].x){
                        x2[0] = jpROI->pSrc2[j].x;
                    }
                    if(x2[1] < jpROI->pSrc2[j].x){
                        x2[1] = jpROI->pSrc2[j].x;
                    }

                    if(y2[0] > jpROI->pSrc2[j].y){
                        y2[0] = jpROI->pSrc2[j].y;
                    }
                    if(y2[1] < jpROI->pSrc2[j].y){
                        y2[1] = jpROI->pSrc2[j].y;
                    }

                }
                break;
            case 2:
                if( jpROI->pSrc3.empty() == true)
                    break;
                if(j == 0){
                    x3[0] = jpROI->pSrc3[j].x;
                    y3[0] = jpROI->pSrc3[j].y;

                    x3[1] = jpROI->pSrc3[j].x;
                    y3[1] = jpROI->pSrc3[j].y;
                }else{

                    if(x3[0] > jpROI->pSrc3[j].x){
                        x3[0] = jpROI->pSrc3[j].x;
                    }
                    if(x3[1] < jpROI->pSrc3[j].x){
                        x3[1] = jpROI->pSrc3[j].x;
                    }

                    if(y3[0] > jpROI->pSrc3[j].y){
                        y3[0] = jpROI->pSrc3[j].y;
                    }
                    if(y3[1] < jpROI->pSrc3[j].y){
                        y3[1] = jpROI->pSrc3[j].y;
                    }

                }
                break;
            case 3:
                if( jpROI->pSrc4.empty() == true)
                    break;
                if(j == 0){
                    x4[0] = jpROI->pSrc4[j].x;
                    y4[0] = jpROI->pSrc4[j].y;

                    x4[1] = jpROI->pSrc4[j].x;
                    y4[1] = jpROI->pSrc4[j].y;
                }else{

                    if(x4[0] > jpROI->pSrc4[j].x){
                        x4[0] = jpROI->pSrc4[j].x;
                    }
                    if(x4[1] < jpROI->pSrc4[j].x){
                        x4[1] = jpROI->pSrc4[j].x;
                    }

                    if(y4[0] > jpROI->pSrc4[j].y){
                        y4[0] = jpROI->pSrc4[j].y;
                    }
                    if(y4[1] < jpROI->pSrc4[j].y){
                        y4[1] = jpROI->pSrc4[j].y;
                    }

                }
                break;
            }

        }
    }

    crop.x1[0] = x1[0]; crop.y1[0] = y1[0];
    crop.x1[1] = x1[1]; crop.y1[1] = y1[1];

    crop.x2[0] = x2[0]; crop.y2[0] = y2[0];
    crop.x2[1] = x2[1]; crop.y2[1] = y2[1];

    crop.x3[0] = x3[0]; crop.y3[0] = y3[0];
    crop.x3[1] = x3[1]; crop.y3[1] = y3[1];

    crop.x4[0] = x4[0]; crop.y4[0] = y4[0];
    crop.x4[1] = x4[1]; crop.y4[1] = y4[1];
    return;
}

void OpenCvWorker::receiveNightToggle()
{
    QMutexLocker locker(&m_mutex);
    night_flag = !night_flag;
    return;
}

void OpenCvWorker::receiveClassifierFWenable(bool _enabled)
{
    QMutexLocker locker(&m_mutex);
    classifier_flag_fw = _enabled;
    return;
}

void OpenCvWorker::receiveClassifierMTenable(bool _enabled)
{
    QMutexLocker locker(&m_mutex);
    classifier_flag_mt = _enabled;
    return;
}

void OpenCvWorker::receiveClassifierTRenable(bool _enabled)
{
    QMutexLocker locker(&m_mutex);
    classifier_flag_tr = _enabled;
    return;
}
void OpenCvWorker::receiveClassifierPath(QString _path, int _type)
{
    QMutexLocker locker(&m_mutex);
//    if(_type == 0){
//        if(night_flag)
//            cfg.classifierFW_n.load(_path.toStdString());
//        else
//            cfg.classifierFW.load(_path.toStdString());
//        return;
//    }
//    if(_type == 1){
//        if(night_flag)
//            cfg.classifierTR_n.load(_path.toStdString());
//        else
//            cfg.classifierTR.load(_path.toStdString());
//        return;
//    }
//    if(_type == 2){
//        if(night_flag)
//            cfg.classifierMT_n.load(_path.toStdString());
//        else
//            cfg.classifierMT.load(_path.toStdString());
//        return;
//    }
}

void OpenCvWorker::receiveFlagROI()
{
    QMutexLocker lock(&m_mutex);
    flag_receive_roi = true;
    return;

}

void OpenCvWorker::receiveChangeInMinMax(int _min_size,int _max_size,int id)
{
    QMutexLocker lock(&m_mutex);
    switch (id) {
    case 0:
        cfg.minSize_fw = cv::Size(_min_size,_min_size);
        cfg.maxSize_fw = cv::Size(_max_size,_max_size);
        break;
    case 1:
        cfg.minSize_mt = cv::Size(_min_size,_min_size + _min_size/2);
        cfg.maxSize_mt = cv::Size(_max_size,_max_size);
        break;
    case 2:
        cfg.minSize_tr = cv::Size(_min_size,_min_size);
        cfg.maxSize_tr = cv::Size(_max_size,_max_size);
        break;
    default:
        break;
    }
    return;
}

void OpenCvWorker::cleanBoundingBox(std::vector<Rect> &fw, std::vector<Rect> &mt,std::vector<Rect> &nmt)
{
    cv::Point center;
    nmt.clear();
    skip_these.clear();
    for(int i = 0; i < mt.size();i++){
        center = cv::Point(mt[i].tl().x + (mt[i].width/2),mt[i].tl().y + (mt[i].height/2));
        for(int j = 0; j < fw.size(); j++){
            if(!fw[j].contains(center)){
                qDebug() << "FOUND ONE";
                nmt.push_back(mt[i]);
                break;
            }

        }
    }



}

void OpenCvWorker::receiveEnableB()
{
    QMutexLocker locker(&m_mutex);
    b_flag = true;
    t_flag = false;

    max_fw = cv::Size(FW_MAX_B);
    max_mt = cv::Size(MT_MAX_B);
    max_tr = cv::Size(TR_MAX_B);

    min_fw = cv::Size(FW_MIN_B);
    min_mt = cv::Size(MT_MIN_B);
    min_tr = cv::Size(TR_MIN_B);

    cfg.classifierFW[0].load(CLASSIFIER_FW_PATH_1);
    cfg.classifierFW[1].load(CLASSIFIER_FW_PATH_2);
    cfg.classifierFW[2].load(CLASSIFIER_FW_PATH_3);
    cfg.classifierFW[3].load(CLASSIFIER_FW_PATH_4);

    cfg.classifierMT[0].load(CLASSIFIER_M_PATH);
    cfg.classifierMT[1].load(CLASSIFIER_M_PATH);
    cfg.classifierMT[2].load(CLASSIFIER_M_PATH);
    cfg.classifierMT[3].load(CLASSIFIER_M_PATH);

    cfg.classifierTR[0].load(CLASSIFIER_T_PATH);
    cfg.classifierTR[1].load(CLASSIFIER_T_PATH);
    cfg.classifierTR[2].load(CLASSIFIER_T_PATH);
    cfg.classifierTR[3].load(CLASSIFIER_T_PATH);

}
void OpenCvWorker::receiveEnableT()
{
    QMutexLocker locker(&m_mutex);
    t_flag = true;
    b_flag = false;
    min_fw = cv::Size(FW_MIN_T);
    min_tr = cv::Size(TR_MIN_T);
    min_mt = cv::Size(MT_MIN_T);
    max_fw = cv::Size(FW_MAX_T);
    max_tr = cv::Size(TR_MAX_T);
    max_mt = cv::Size(MT_MAX_T);


    cfg.classifierFW[0].load(CLASSIFIER_FW_PATH_4);
    cfg.classifierFW[1].load(CLASSIFIER_FW_PATH_4);
    cfg.classifierFW[2].load(CLASSIFIER_FW_PATH_4);

    cfg.classifierMT[0].load(CLASSIFIER_M_PATH);
    cfg.classifierMT[1].load(CLASSIFIER_M_PATH);
    cfg.classifierMT[2].load(CLASSIFIER_M_PATH);
    //cfg.classifierMT[3].load(CLASSIFIER_M_PATH);

    cfg.classifierTR[0].load(CLASSIFIER_T_PATH);
    cfg.classifierTR[1].load(CLASSIFIER_T_PATH);
    cfg.classifierTR[2].load(CLASSIFIER_T_PATH);
    //cfg.classifierTR[3].load(CLASSIFIER_FW_PATH_4);
    //cfg.classifierFW[3].load(CLASSIFIER_FW_PATH_4);
}
