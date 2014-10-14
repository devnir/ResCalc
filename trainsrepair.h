#ifndef TRAINSREPAIR_H
#define TRAINSREPAIR_H

#include <QObject>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

class trainsRepair : public QObject
{
    Q_OBJECT
  public:
    explicit trainsRepair(QObject *parent = 0);
    void updateData(QString answer);
    void start(QByteArray id, double reabil);
    void stop();
  signals:
    void signalRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void signalPutToLog(QString str);
  private:
    QTimer timer;
    QByteArray uId;
    double reliability;
    void delay(int secondsToWait );
  public slots:
    void TimeOut();

};

#endif // TRAINSREPAIR_H
