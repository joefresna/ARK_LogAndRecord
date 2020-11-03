#ifndef MYKILOBOTENVIRONMENT_H
#define MYKILOBOTENVIRONMENT_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <kilobotenvironment.h>
#include <QTime>

static const double MM_TO_PIXEL = 2000.0/2000.0;
static const double PIXEL_TO_MM = 2000.0/2000.0;

static const double M_TO_PIXEL = 1000.0;

struct option {
    unsigned int ID;
    int quality;
    float posX; // in pixel
    float posY; // in pixel
    float rad;  // in pixel
    QString color;
};


class mykilobotenvironment : public KilobotEnvironment
{
    Q_OBJECT
public:
    explicit mykilobotenvironment(QObject *parent = 0);

    // A vector containing the options
    QVector <option> m_Options;
    QVector <unsigned int> m_discovery;
    QPointF kPos;
    //discovery time flag
    bool m_timefordiscovery;
    bool m_discovery_flag=false;
    bool m_messages_sent=false;

signals:
    void errorMessage(QString);

public slots:
    void update(){}
    void updateVirtualSensor(Kilobot kilobot);

private:

};
#endif // MYKILOBOTENVIRONMENT_H
