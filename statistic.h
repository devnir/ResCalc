#ifndef STATISTIC_H
#define STATISTIC_H

#include <QObject>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QVector>

typedef enum
{
  FirstRun = 0,
  TimeCorrection,
  StatCalc
}TState;

typedef struct
{
    int type;
    int amount;
    int capacity;
    int priority;
    double trend;
    int tend;
}TRes;

typedef struct
{
    int lvl;
    QVector<TRes> res;
}TUpStat;



class Statistic : public QObject
{
    Q_OBJECT
  public:
    explicit Statistic(QObject *parent = 0);
    void start(QByteArray townId);
    void updateData(QString answer);
    int timeOut;
private:
    QTimer *timer;
    QByteArray cityId;
    TState state;
    QVector<TRes> curRes;
    QVector<TRes> prevRes;
    TUpStat       upStat;

    void firstUpdate(QString answer);
    void calcStat(QString answer);
    void calcVariables();
    void printLog();
  signals:
    void signalRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void signalPutToLog(QString str);
  public slots:
    void slotTimeOut();
};

#endif // STATISTIC_H
