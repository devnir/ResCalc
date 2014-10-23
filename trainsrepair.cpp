#include "trainsrepair.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDateTime>

trainsRepair::trainsRepair(QObject *parent) :
  QObject(parent)
{
  timer.setSingleShot(false);
  timer.setInterval(1000 * 60 * (25 + qrand() % 10));
  connect(&timer, SIGNAL(timeout()), this, SLOT(TimeOut()));
  reliability = 75.;
}

void trainsRepair::start(QByteArray id, double reabil)
{
  uId = id;
  reliability = reabil;
  TimeOut();
}

void trainsRepair::stop()
{
  timer.stop();
}

void trainsRepair::TimeOut()
{
   QByteArray addUrl;
//   addUrl.append("/web/rpc/flash.php?interface=TrainInterface&method=getMyTrains");
//   QByteArray param("[true]");
   addUrl.append("/web/rpc/flash.php?interface=PersonalityInterface&method=getAuctions");
   QByteArray param("[]");
   emit this->signalRequest(addUrl, param, true);
   int interval = 1000 * 60 * (25 + qrand() % 10);
   int m = interval /1000 / 60;
   int s = interval /1000 - (m * 60);

   timer.setInterval(interval);
   QString str;
   str.sprintf("Ожидание следующего расчета: %02d:%02d", m, s);
   emit this->signalPutToLog(str);
   timer.start();
}

void trainsRepair::updateData(QString answer)
{  
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonArray bodyObj = jsonObject["Body"].toArray();

  QJsonObject to = jsonObject["Body"].toObject();
  qDebug() << answer;

  for(int i = 0; i < bodyObj.count(); i++)
  {
    QJsonObject train = bodyObj.at(i).toObject();
    double reliab = train["reliability"].toDouble();
    if(reliab < reliability)
    {
      QByteArray addUrl;
      addUrl.append("/web/rpc/flash.php?interface=TrainInterface&method=doMaintenance");
      QByteArray param;
      param.append(train["ID"].toString());
      param.prepend("[\"");
      param.append("\"]");
      emit this->signalRequest(addUrl, param, true);
      QString str;
      str.sprintf("%0.2f", reliab);
      str.prepend("Ремонт поезда: \"" + train["name"].toString() + "\". Состояние до ремонта:");
      emit this->signalPutToLog(str);
      delay(qrand() % 5);
    }
  }
}

void trainsRepair::delay( int secondsToWait )
{
    QTime dieTime = QTime::currentTime().addSecs(secondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}
