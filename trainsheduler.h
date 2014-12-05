#ifndef TRAINSHEDULER_H
#define TRAINSHEDULER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkConfiguration>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include "settingsadd.h"
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QPen>
#include <QBrush>
#include <QGraphicsItem>
#include <QFont>
#include <QPainterPath>
#include "getxmldata.h"


namespace Ui {
  class trainSheduler;
}

typedef struct
{
  QByteArray id;
  QByteArray name;
  int x;
  int y;
  int type;
}TLocation;

typedef struct
{
    TLocation loc1;
    TLocation loc2;
}TRoads;
class cityItem : public QGraphicsEllipseItem
{
  protected:
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

typedef struct
{
  QByteArray id;
  QByteArray name;
}TTrainCard;



class trainSheduler : public QWidget
{
    Q_OBJECT

  public:
    explicit trainSheduler(QWidget *parent = 0);
    ~trainSheduler();
    void init(TServer server);

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_pushButton_clicked();
    void on_readChannelFinished();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

  private:
    Ui::trainSheduler *ui;
    TServer srv;
    QNetworkAccessManager *mgr;
    QGraphicsScene *scene;
    void sendQuery(QByteArray addUrl, QByteArray param, bool enReq);
    void getTrains(QJsonObject jObj);
    void getLocations(QJsonObject jObj);
    void getRoads(QJsonObject jObj);
    void printMiniMap();
    void printHomeLoc(int scale, int size);
    void addRoad(QByteArray from, QByteArray to);

    TLocation homeTown;
    QVector<TTrainCard>  myTrains;
    QVector<TLocation>   globalMap;
    QVector<TRoads>      myRoads;
//GUI
    void guiPrinTrainList();
};

#endif // TRAINSHEDULER_H
