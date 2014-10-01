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

typedef struct
{
  QString serverName;
  QString townName;
  QByteArray serverUrl;
  QByteArray userAgent;
  QByteArray cookie;
  QByteArray post;
}TServerList;
typedef struct
{
    int id;
    int count;
}TRes;

typedef struct
{
  int  lvl;
  TRes res[4];
}TForumMsg;


namespace Ui {
  class ResCalc;
}

class ResCalc : public QMainWindow
{
    Q_OBJECT

  public:
    void loadSettings();
    void saveSettings();
    void prepareData();
    void firstPrepare();
    void putRequest();
    void prepareCorrection();
    explicit ResCalc(QWidget *parent = 0);
    QTimer *replayTimer;
    QNetworkAccessManager *mgr;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QVector<TServerList> serverList;
    bool firstRun;
    int secCounter;
    ~ResCalc();

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void getReplyFinished();
    void readyReadReply();
    void ReplyFinishedDBG();
    void readyReadReplyDBG();
    void correctionReply();
    void timeOut();

    void on_StartBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

  private:
    Ui::ResCalc *ui;
};

#endif // RESCALC_H
