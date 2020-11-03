#ifndef TESTLIB_H
#define TESTLIB_H

#include "global.h"

//
#include <QObject>
#include <QFile>
//#include <QTextStream>

// Project includes
#include "kilobot.h"
#include "kilobotexperiment.h"
#include "kilobotenvironment.h"
#include "LOGandRecordEnv.h"

// OpenCV includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTableWidget>
#include <QSpinBox>


using namespace cv;

class KiloLog {
public:
    // constructors
    KiloLog() {}
    KiloLog(kilobot_id id, QPointF pos, double rot, kilobot_colour col) : id(id), position(pos), orientation(rot), colour(col) {}

    // methods
    void updateAllValues(kilobot_id id, QPointF pos, double rot, kilobot_colour col){
        this->id = id;
        this->position = pos;
        this->orientation = rot;
        this->colour = col;
    }
    void setPos(QPointF pos){
        this->position = pos;
    }
    void setOrientation(double rot){
        this->orientation = rot;
    }
    void setCol(kilobot_colour col){
        this->colour = col;
    }

    // variables
    kilobot_id id;
    QPointF position;
    double orientation;
    kilobot_colour colour;
};

class LOGANDRECORDEXPSHARED_EXPORT mykilobotexperiment : public KilobotExperiment
{
    Q_OBJECT
public:
    mykilobotexperiment();
    virtual ~mykilobotexperiment() {}

    QWidget * createGUI();

    //
    QVector < Kilobot * > mykilobot;


signals:
    void errorMessage(QString);
//    void setExptImage(QPixmap);

public slots:
    void initialise(bool);
    void run();
    void stopExperiment();

//    void setupExperiment();
    void toggleSaveImages(bool toggle) { saveImages = toggle; }
    void toggleLogExp(bool toggle){ logExp = toggle; }
private:
    void updateKilobotState(Kilobot kilobotCopy);
    void setupInitialKilobotState(Kilobot kilobotCopy);

    //
    void setupEnvironments();
    void plotEnvironment();

    //Image saving variables
    bool saveImages;
    int savedImagesCounter = 0;
    QString im_filename_prefix="/home/areina/KBArenaExpsData/run%1";
    QString im_filename_suffix="/frame_%1.jpg";



    //Logging period in seconds
    float m_log_period;
    int m_expno=0;

    //Log variables
    bool logExp;
    QFile log_file;
    QString log_filename_prefix="/home/areina/KBArenaExpsData/run%1";
    QString log_filename_suffix="/log_robots.txt";
    QTextStream log_stream;
    QVector < kilobot_id >  allKiloIDs;
    QVector <KiloLog> allKilos;

    //GUI
    QSpinBox * expno_spin;


};



#endif // TESTLIB_H
