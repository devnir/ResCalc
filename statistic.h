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
  StatCalc,
  GetForums
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
    int  timeOut;
    bool enPrintToForum;
    int  minResCount;
    bool isFinal;
    void setForumTheme(QString theme);
private:
    QTimer *timer;
    QByteArray cityId;
    QByteArray forumId;
    TState state;
    QVector<TRes> curRes;
    QVector<TRes> prevRes;
    TUpStat       upStat;
    QString forumTheme;
    QByteArray forumPost;
    QTimer    *forumPostTimer;

    void delay( int secondsToWait );
    void firstUpdate(QString answer);
    void calcStat(QString answer);
    void forumThemes(QString answer);
    void calcVariables();
    void printLog();
  signals:
    void signalRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void signalPutToLog(QString str);
  public slots:
    void slotTimeOut();
    void slotForumPostTimeout();
    void slotIsFinalChange(bool final);
};

#endif // STATISTIC_H
