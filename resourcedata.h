#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

#include <QObject>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

typedef struct
{
  double  amount;//: 1635
  double  capacity;//: 5300
  double  consume_amount;//: 260
  QString factions;//: null
  double  income;///: 1
  double  price;//: 198
  double  price_factor;//: 0.95
  double  priority;//: 1
  double  resource_type;//: 7
  double  trend;//: -0.25
  double  type;//: 7
}TResource;

typedef struct
{
    double  type;
    double  trend;
}TTrend;

class resourceData : public QObject
{
    Q_OBJECT
  public:
    explicit resourceData(QObject *parent = 0);
    void clearCurrData();
    void putData(QJsonObject res);
    void getData();
    void calcTrend();
    void copy();
    double getTrendById(double id);
    TResource getPrev(double type, bool &result);
    QVector<TResource> prevRes;
    QVector<TResource> curRes;
    QVector<TTrend>    trendStr;
  signals:

  public slots:

};

#endif // RESOURCEDATA_H
