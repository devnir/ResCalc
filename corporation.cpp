#include "corporation.h"
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>
//edd8c452-b501-5d54-77c5-e0fcfb6f5c9c
bool errPresent = false;
#include <QTime>

void Corporation::delay( int secondsToWait )
{
    QTime dieTime = QTime::currentTime().addSecs(secondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

int rndTmr;
int min = 1;
int NextWait = 5400;
QString buildName[] =
{
  "Торговый центр",
  "Отель",
  "Ресторан"
};

Corporation::Corporation(QObject *parent) :
  QObject(parent)
{
  getBuildingTimer = new QTimer(this);
  getBuildingTimer->setSingleShot(false);
  connect(getBuildingTimer, SIGNAL(timeout()), this, SLOT(slotBuildingsTimerTimeOut()));
}

void Corporation::start(QByteArray corpId, QByteArray uId)
{
  NextWait = 5400;
  errPresent = false;
  members.clear();
  myId = uId;
  rndTmr = qrand() % 2000;
  id.clear();
  id.append(corpId);
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=CorporationInterface&method=get");
  QByteArray param = id;
  param.prepend("[\"");
  param.append("\"]");

  emit this->signalRequest(addUrl, param, true);
}

void Corporation::updateData(QString answer)
{
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  rndTmr = qrand() % 2000;
  //
  if(bodyObj.contains("members"))
  {
    if(bodyObj["members"].isArray())
    {
      members.clear();
      QJsonArray  memArr = bodyObj["members"].toArray();
      for(int i =0; i < memArr.count(); i++)
      {
        TCorpMem tMem;
        QJsonObject jMem = memArr.at(i).toObject();
        tMem.name =jMem["name"].toString();
        tMem.uid.append(jMem["user_id"].toString());
        members.append(tMem);
        //qDebug() << "Add new mem";
      }
    }
    else
    {
      //qDebug() << "no any mem";
    }
    getBuildingTimer->start(rndTmr + 1000);
  }
  else if(bodyObj.contains("9")||bodyObj.contains("10")||bodyObj.contains("11"))
  {
    for(int i = 9; i < 12; i++)
    {
      if(bodyObj.contains(QString("%1").arg(i)))
      {
        QJsonObject build = bodyObj[QString("%1").arg(i)].toObject();
        int productionTime = build["productionTime"].toDouble();
        if(productionTime == 0)
        {
          //delay(1);
          TCorpMem tMem = members.at(0);
          getUserBonus(i, tMem.uid);
          QString str("Собрано с ");
          str.append(buildName[i-9]);
          str.append(" у ");
          str.append(tMem.name);          
          emit this->signalPutToLog(str);
        }
        else
        {
          if(productionTime < NextWait)
            NextWait = productionTime;
        }
      }
    }
    members.remove(0);
    errPresent = false;
    getBuildingTimer->start(rndTmr + 1000);
  }
  else if(bodyObj.contains("level")&& bodyObj.contains("type"))
  {
    int level = (int)bodyObj["level"].toDouble();
    int type = (int)bodyObj["type"].toDouble();
    int productionTime = (int)bodyObj["productionTime"].toDouble();
    if((type > 8) &&(type <12))
    {
      if(productionTime == 0)
      {
      }
      else
      {
        if(productionTime < NextWait)
          NextWait = productionTime;
      }
    }
  }
  else    
  {
    //qDebug() << answer;
    errPresent = true;
    QString str;
    str.sprintf("Ошибка разбора. Повтор через: %dс",
                NextWait);

    //emit this->signalPutToLog(str);
    getBuildingTimer->start(1000);
  }
}

void Corporation::getUserBuildings()
{
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=BuildingsInterface&method=getAll");
  //qDebug("Mem:");

  QByteArray param;
  TCorpMem mem = members.at(0);
  param.append(mem.uid);
  //qDebug() << mem.name;
  param.prepend("[\"");
  param.append("\"]");
  emit this->signalRequest(addUrl, param, true);

}

void Corporation::slotBuildingsTimerTimeOut()
{
  getBuildingTimer->stop();

  if(members.count() > 0)
  {
    getUserBuildings();
  }
  else
  {
    QString str;
    if(errPresent)
    {
      NextWait = 60 + qrand() % 60;
    }

    str.sprintf("Завершено. Следующая проверка через %dс",
                NextWait);

    emit this->signalPutToLog(str);    
    emit this->checkDone(NextWait);
    return ;
  }
}

void Corporation::getUserBonus(int buildId, QByteArray userId)
{
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=BuildingsInterface&method=collect");
  QString str;
  QByteArray param;
  str.sprintf("[%d", buildId);
  param.append(str);
  if(myId == userId)
  {
    param.append("]");
    //qDebug() << param;
    emit this->signalRequest(addUrl, param, true);
  }
  else
  {
    param.append(",\"");
    param.append(userId);
    param.append("\"]");
    emit this->signalRequest(addUrl, param, false);
  }

}

void Corporation::stop()
{
  getBuildingTimer->stop();
}

