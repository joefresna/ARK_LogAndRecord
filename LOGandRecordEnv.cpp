#include "LOGandRecordEnv.h"
#include <QVector>
#include <QLineF>
#include <QDebug>
#include <QtMath>

#include "kilobot.h"

mykilobotenvironment::mykilobotenvironment(QObject *parent) : KilobotEnvironment(parent) {
    qDebug() << "Environment is up and running. :-)";
}

// generate virtual sensor readings & send to KB
void mykilobotenvironment::updateVirtualSensor(Kilobot kilobot) {

    kilobot_id kID = kilobot.getID();

    if(m_timefordiscovery){
    if(m_discovery[kID]!=0){
        kilobot_message msg;
        msg.id=kID;
        msg.type=m_discovery[kID];
        msg.data=m_Options[m_discovery[kID]-1].quality;
        emit transmitKiloState(msg);
        m_discovery[kID]=0;
        m_messages_sent=true;
        //qDebug() << "messege sent to robot------>" << kID ;

    }
    }
    else{
    kPos = kilobot.getPosition();

    option Op;
    for (int i=0;i<m_Options.size();i++ ){
    Op=m_Options[i];
    float rad=Op.rad;
    float x=Op.posX;
    float y=Op.posY;

    if((pow(kPos.x()-x,2)+pow(kPos.y()-y,2)) < pow(rad,2) ) {m_discovery[kID]=Op.ID;
    //qDebug() << "Kb" << kID << "discovered option " << Op.ID ;
    m_discovery_flag = true;
    }

    }
 }
}




