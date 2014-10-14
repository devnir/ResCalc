#ifndef BONUSES_H
#define BONUSES_H

#include <QObject>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
//11 - ресторан
//9 - торгаш
//10 - отель
typedef struct
{
  double type;
  double productionTime;
}TBonuses;

class bonuses : public QObject
{
    Q_OBJECT
  public:
    explicit bonuses(QObject *parent = 0);    
    void start(QByteArray user);
    void updateData(QString answer);
  signals:
    void signalRequest(QByteArray addUrl, QByteArray param, bool enReq);
  private:
    QVector<TBonuses> myBonus;
    QByteArray userId;
    int waitTimeout;
    void getMyBonus(int id);

};

#endif // BONUSES_H
