#ifndef CORPORATION_H
#define CORPORATION_H

#include <QObject>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
typedef struct
{
  QString name;
  QByteArray uid;

}TCorpMem;
class Corporation : public QObject
{
    Q_OBJECT
  public:
    explicit Corporation(QObject *parent = 0);
    void start(QByteArray corpId, QByteArray uId);
    void stop();
    void updateData(QString answer);
    void getUserBuildings();
    void getUserBonus(int buildId, QByteArray userId);
  signals:
    void signalRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void checkDone(int nextWait);
    void signalPutToLog(QString str);
    void takedBonus();
  public slots:
    void slotBuildingsTimerTimeOut();
  private:
    QByteArray id;
    QByteArray myId;
    void delay(int secondsToWait );
    QVector<TCorpMem> members;
    QTimer *getBuildingTimer;



};

#endif // CORPORATION_H
