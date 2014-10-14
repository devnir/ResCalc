#include "bonuses.h"
#include <QDebug>

bonuses::bonuses(QObject *parent) :
  QObject(parent)
{
  myBonus.clear();
  waitTimeout = 3600;
}

void bonuses::start(QByteArray user)
{
  userId.clear();
  userId.append("[\"e4135b18-9b4b-9379-0fec-cebe808fec4b\"]");
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=BuildingsInterface&method=getAll");
  QByteArray param;
  param.append(userId);
  emit this->signalRequest(addUrl, param, true);
}

void bonuses::updateData(QString answer)
{
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();

  for(int i = 9; i < 12; i++)
  {
    QString str;
    str.sprintf("%d", i);
    QJsonObject building = bodyObj[str].toObject();
    {
      int productionTime = building["productionTime"].toDouble();
      if(productionTime == 0)
      {
        getMyBonus(i);
      }
      else if(productionTime > 0)
      {
        if(productionTime < waitTimeout)
        {
          waitTimeout = productionTime;
        }
      }
    }
  }
  qDebug() << waitTimeout;
}

void bonuses::getMyBonus(int id)
{
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=BuildingsInterface&method=collect");
  QString str;
  str.sprintf("%d", id);
  str.prepend("[");
  str.append("]");
  QByteArray param;
  param.append(str);
  emit this->signalRequest(addUrl, param, false);
}
