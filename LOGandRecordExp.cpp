#include "LOGandRecordExp.h"
#include "LOGandRecordEnv.h"
#include <QDebug>
#include <QThread>

// widgets
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QtMath>
#include <QSpinBox>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QTableWidgetItem>
#include <QPainter>

// return pointer to interface!
// mykilobotexperiment can and should be completely hidden from the application
extern "C" LOGANDRECORDEXPSHARED_EXPORT KilobotExperiment *createExpt()
{
    return new mykilobotexperiment();
}

mykilobotexperiment::mykilobotexperiment() {

    // setup the environments here
    this->serviceInterval = 100; // timestep in ms?

}

// GUI creation function: Working properly

QWidget * mykilobotexperiment::createGUI() {
    QFrame * frame = new QFrame;
    QVBoxLayout * lay = new QVBoxLayout;
    frame->setLayout(lay);

    // add a check box to record the experiment
    QCheckBox * saveImages_ckb = new QCheckBox("Record experiment");
    saveImages_ckb->setChecked(true);// make the box not checked by default
    lay->addWidget(saveImages_ckb);
    toggleSaveImages(saveImages_ckb->isChecked());

    // add a check box to log the experiment
    QCheckBox * logExp_ckb = new QCheckBox("Log experiment");
    logExp_ckb->setChecked(true); // make the box checked by default
    lay->addWidget(logExp_ckb);
    toggleLogExp(logExp_ckb->isChecked());

    QGroupBox * formGroupExpno = new QGroupBox(tr("Expno:"));
    QFormLayout * layout2 = new QFormLayout;
    formGroupExpno->setLayout(layout2);
    expno_spin = new QSpinBox();
    expno_spin->setMinimum(1);
    expno_spin->setMaximum(10);
    expno_spin->setValue(1);
    layout2->addRow(new QLabel(tr("Exp no:")), expno_spin);
    lay->addWidget(formGroupExpno);



    // signal-slot connections
    connect(saveImages_ckb, SIGNAL(toggled(bool)),this, SLOT(toggleSaveImages(bool)));
    connect(logExp_ckb, SIGNAL(toggled(bool)),this, SLOT(toggleLogExp(bool)));

    lay->addStretch();

    connect(this,SIGNAL(destroyed(QObject*)), lay, SLOT(deleteLater()));


    return frame;
}


// Initialization function: Working properly
void mykilobotexperiment::initialise(bool isResume) {


    //qDebug() << "Entered initializatioN%%%%";

    // Generate Environments:
    setupEnvironments();

    // Initialize variables

    m_log_period=1;

    m_expno=expno_spin->value();

    // Initialize Kilobot States:
    if (!isResume) {
        // init stuff
        emit getInitialKilobotStates();
    } else {
    }

    emit setTrackingType(POS);

    QThread::currentThread()->setPriority(QThread::HighestPriority);

    // Init Log File operations
    if (logExp){
        if (log_file.isOpen()){
            log_file.close(); // if it was open I close and re-open it (erasing old content!! )
        }


       QString log_filename = (log_filename_prefix.arg(m_expno))+log_filename_suffix;
//        QString log_filename = log_filename_prefix + "_" + QDate::currentDate().toString("yyMMdd") + "_" + QTime::currentTime().toString("hhmmss") + ".txt";
        log_file.setFileName( log_filename );
        if ( !log_file.open(QIODevice::WriteOnly) ) { // open file
            qDebug() << "ERROR(!) in opening file" << log_filename;
        } else {
            qDebug () << "Log file" << log_file.fileName() << "opened.";
            log_stream.setDevice(&log_file);
        }
    }

    savedImagesCounter = 0;
    this->time = 0;


    //save initial image
    if (saveImages) {
        emit saveImage(im_filename_prefix.arg(m_expno,1)+im_filename_suffix.arg(savedImagesCounter++, 5,10, QChar('0')));
    }

    //log initial state of the robots
    if (logExp){
        log_stream << this->time;
        for (int i = 0; i < allKiloIDs.size(); ++i){
            kilobot_id kID = allKiloIDs[i];
            log_stream << "\t" << kID << "\t" << allKilos[kID].position.x() << "\t" << allKilos[kID].position.y();
        }
        log_stream << endl;
    }


    clearDrawings();
}


// Stop function working properly
void mykilobotexperiment::stopExperiment() {
    if (log_file.isOpen()){
        qDebug() << "Closing file" << log_file.fileName();
        log_file.close();
    }
}

void mykilobotexperiment::run()
{

    this->time += 0.1; // 100 ms in sec

    // Update Kilobot States:
    emit updateKilobotStates();

    //Save image and log data once every second
      if (qRound(time*10.0f) % qRound(m_log_period*10.0f) == 0)   { // every 1s
        if (saveImages) {
            emit saveImage(im_filename_prefix.arg(m_expno,1)+im_filename_suffix.arg(savedImagesCounter++, 5,10, QChar('0')));
        }
        if (logExp){
            log_stream << this->time;
            for (int i = 0; i < allKiloIDs.size(); ++i){
                kilobot_id kID = allKiloIDs[i];
                log_stream << "\t" << kID << "\t" << allKilos[kID].position.x() << "\t" << allKilos[kID].position.y();
            }
            log_stream << endl;
        }
        }

    clearDrawings();
    clearDrawingsOnRecordedImage();
    plotEnvironment();

}

// Setup the Initial Kilobot Environment:
//   This is run once for each kilobot after emitting getInitialKilobotStates() signal.
//   This assigns kilobots to an environment.
void mykilobotexperiment::setupInitialKilobotState(Kilobot kilobotCopy) {
    // Assigns all kilobots to environment pheroEnv:
//    this->setCurrentKilobotEnvironment(&m_optionsEnv);


    kilobot_id kID = kilobotCopy.getID();

    if (kID > allKilos.size()-1){
        allKilos.resize(kID+1);
    }

    KiloLog kLog(kID, kilobotCopy.getPosition(), 0, OFF);
    allKilos[kID] = kLog;
    if (!allKiloIDs.contains(kID)) allKiloIDs.append(kID);

}

// run once for each kilobot after emitting updateKilobotStates() signal
void mykilobotexperiment::updateKilobotState(Kilobot kilobotCopy) {
    kilobot_id kID = kilobotCopy.getID();
    QPointF kPos = kilobotCopy.getPosition();
    // update values for logging
    if (logExp){
        allKilos[kID].updateAllValues(kID, kPos, 0, OFF);
    }
}


// Setup Environment
void mykilobotexperiment::setupEnvironments( ) {
   plotEnvironment();
}

// Draw the options:
void mykilobotexperiment::plotEnvironment() {
}
