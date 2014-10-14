#ifndef RESCALC_H
#define RESCALC_H

#include <QMainWindow>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkConfiguration>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QSettings>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include "resourcedata.h"
#include "bonuses.h"
#include "corporation.h"
#include "settingsadd.h"
#include "trainsrepair.h"
/*
 * Структура для ремонта поездов
 */
typedef struct
{
    QByteArray id;
    QString    name;
    double     reliability;
}TMyTrain;
/*
 * Структура настроек
 */
typedef struct
{
    int id;
    int count;
    float trend;
}TRes;

typedef struct
{
    int lvl;
    QVector<TRes> res;
}TUpStat;



typedef struct
{
    QVector<TServer> server;
    TForum  forum;
}TSettings;

namespace Ui {
  class ResCalc;
}

class ResCalc : public QMainWindow
{
    Q_OBJECT

  public:
    explicit ResCalc(QWidget *parent = 0);
    QTimer   *updTimer;
    QTimer   *postTimer;
    QTimer   *localTimer;
    QTimer   *bonColTimer;
    int       secCounter;
    int       bonCounter;
    int       bonTimeLeft;
    ~ResCalc();

  protected:
    void changeEvent(QEvent *e);

  private slots:

    void forumReplyFinished();
    void forumReadyReadReply();
    void ReplyFinished();
    void ReadyReadFirstReply();
    void ReadyReadReply();
    void timeOut();
    void showLocalTime();
    void postTimeout();
    void postStr(QString msg);
    void reqPrepare(bool correction);
    void trainReadyReadReply();
    void trainReplyFinished();
    void slotBonusRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void bonusReadyReadReply();
    void bonusReplyFinished();
    void corpReadyReadReply();
    void corpReplyFinished();
    void slotCorpRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void slotBonColTimeOut();
    void slotBonColDone(int nextWait);
    void slotBonLog(QString str);

    void slotTrainRequest(QByteArray addUrl, QByteArray param, bool enReq);
    void slotTrainLog(QString str);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();



    void on_repairBtn_clicked();







  private:
    Ui::ResCalc   *ui;
    TSettings      settings;
    TUpStat        upStat;    
    QNetworkAccessManager *mgr;
    QNetworkReply *forumReply;
    QNetworkReply *reply;
    QNetworkReply *trainReply;
    QNetworkReply *bonusReply;
    QNetworkReply *corpReply;
    QVector<TMyTrain> myTrain;
    resourceData   resData;
    bonuses        bonus;
    Corporation    corp;
    settingsAdd    setForm;
    trainsRepair   trainRep;

    void loadSettings();
    void saveSettings();
    void onceInit();
    void myRepairReq(TMyTrain train);
};

#endif // RESCALC_H
