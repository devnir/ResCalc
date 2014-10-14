#include "resourcedata.h"
#include <QDebug>
resourceData::resourceData(QObject *parent) :
  QObject(parent)
{
   prevRes.clear();
   curRes.clear();
   trendStr.clear();
}


void resourceData::putData(QJsonObject res)
{
  TResource tmp;
  tmp.amount = res["amount"].toDouble();
  tmp.capacity = res["capacity"].toDouble();
  tmp.consume_amount = res["consume_amount"].toDouble();
  tmp.factions = res["factions"].toString();
  tmp.income = res["income"].toDouble();
  tmp.price = res["price"].toDouble();
  tmp.price_factor = res["price_factor"].toDouble();
  tmp.priority = res["priority"].toDouble();
  tmp.resource_type = res["resource_type"].toDouble();
  tmp.trend = res["trend"].toDouble();
  tmp.type = res["type"].toDouble();
  curRes.append(tmp);
}

void resourceData::getData()
{

}

void resourceData::calcTrend()
{
  trendStr.clear();
  for(int i = 0; i < curRes.count(); i++)
  {
    TResource tmp = curRes.at(i);
    if(tmp.priority == 1)
    {
      bool find = false;
      TResource prev = getPrev(tmp.type, find);
      if(find == true)
      {
        //qDebug("find == true");
        TTrend trend;
        trend.type = tmp.type;
        trend.trend = tmp.amount - prev.amount;
        trendStr.append(trend);
      }
      else
      {
        //qDebug("find == false");
      }
    }
  }
  this->copy();
}

void resourceData::clearCurrData()
{
  curRes.clear();
}

TResource resourceData::getPrev(double type, bool &result)
{
  result = true;
  for(int i = 0; i < prevRes.count(); i++)
  {
    TResource prev;
    prev = prevRes.at(i);
    if(prev.type == type)
      return prev;
  }
  TResource noRes;
  result = false;
  return noRes;
}

double resourceData::getTrendById(double id)
{
  for(int i = 0; i < trendStr.count(); i++)
  {
    TTrend trend = trendStr.at(i);
    if(trend.type == id)
    {
      return trend.trend;
    }
  }
  return 0.;
}

void resourceData::copy()
{
  prevRes.clear();
  for(int i = 0; i < curRes.count(); i++)
  {
    TResource old = curRes.at(i);
    prevRes.append(old);
  }
  curRes.clear();
}











