#include "statistic.h"
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>
#include <QDateTime>
QByteArray name_res[] =
  {
  "Все товары",
  "Уголь",
  "Пшеница",
  "Железо",
  "Дерево",
  "Доски",
  "Железная руда",
  "Хлопок",
  "Тексиль",
  "Скот",
  "Колбаса",
  "Нить",
  "Инструменты",
  "Бумага",
  "Кожа",
  "Хлеб",
  "Мука",
  "Медная руда",
  "Кварц",
  "Медь",
  "Сталь",
  "Обувь",
   "Стекло",
  "Проволка",
  "Трубы",
  "Упаковка",
  "Окна",
  "Листовой металл",
  "Силикаты","Еда",
  "Нефть","Лампы",
  "Химикаты",
  "Одежда",
  "Нержавеющая сталь",
  "Бокситы",
  "Двигатели",
  "Пластик",
  "Алюминий",
  "Керамика",
  "Стальные балки",
  "Бензин",
  "Автомобили",
  "Хоз. товары",
  "Электроника",
  "Игрушки",
  "Спорт. товары",
  "Сантехника",
  "Лекарства"
  };

int potreb_low[] =
{
  0, 2345, 2680, 3, 3551, 4087, 4690, 5360, 6063, 6800, 7571, 8375, 9212, 10083,
  10988, 11926, 12897, 13902, 14941, 16013, 17119, 18257, 19430, 20636, 21875,
  25, 24455, 25795, 28, 28575, 30016, 31490, 32998, 33, 36113, 37721, 39362,
  41037, 42746, 44488, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  54, 55, 56, 57, 58, 59, 60
};

Statistic::Statistic(QObject *parent) :
  QObject(parent)
{
  timer = new QTimer(this);
  forumPostTimer = new QTimer(this);
  forumPostTimer->setSingleShot(true);
  timer->setSingleShot(false);
  timer->setInterval(1000);
  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
  timeOut = 0;
  timer->start(1000);
  enPrintToForum = false;
  minResCount = 1000;
  forumId.clear();
  isFinal = false;
  connect(forumPostTimer, SIGNAL(timeout()), this, SLOT(slotForumPostTimeout()));
}

void Statistic::delay( int secondsToWait )
{
    QTime dieTime = QTime::currentTime().addSecs(secondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void Statistic::start(QByteArray townId)
{
  cityId = townId;
  QByteArray addUrl;
  QByteArray param("[]");
  state = GetForums;
  addUrl.append("/web/rpc/flash.php?interface=CorporationForumInterface&method=getThreads");
  emit this->signalRequest(addUrl, param, true);
}

void Statistic::updateData(QString answer)
{
  //qDebug() << answer;
  if(state == FirstRun)
  {
    firstUpdate(answer);
  }
  else if(state == TimeCorrection)
  {
    firstUpdate(answer);
  }
  else if(state == StatCalc)
  {
    calcStat(answer);
  }
  else if(state == GetForums)
  {
    forumThemes(answer);
  }
}


void Statistic::firstUpdate(QString answer)
{
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonObject town = bodyObj["town"].toObject();

  int lastUpdate = 900 + (int)town["lastConsumption"].toDouble();

  if(lastUpdate > 30)
  {
    timeOut = lastUpdate - 30;
    state = TimeCorrection;
  }
  else
  {
    timeOut = lastUpdate + 2;
    state = StatCalc;
  }
}

void Statistic::slotTimeOut()
{
  timer->stop();
  if(timeOut > 0)
  {
    timeOut--;
    timer->start(1000);
    return;
  }

  if(state == TimeCorrection)
  {
    QByteArray addUrl;
    addUrl.append("/web/rpc/flash.php?interface=TownInterface&method=getDetails");
    QByteArray param;
    param.append("[\"" + cityId + "\"]");
    emit this->signalRequest(addUrl, param, true);
  }
  else if(state == StatCalc)
  {
    QByteArray addUrl;
    addUrl.append("/web/rpc/flash.php?interface=TownInterface&method=getDetails");
    QByteArray param;
    param.append("[\"" + cityId + "\"]");
    emit this->signalRequest(addUrl, param, true);
    timeOut = 3;
  }
  timer->start(1000);
}

void Statistic::calcStat(QString answer)
{
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonObject town = bodyObj["town"].toObject();
  QJsonArray res = bodyObj["resources"].toArray();

  int lastUpdate = 900 + (int)town["lastConsumption"].toDouble();
  //qDebug() << (int)town["lastConsumption"].toDouble();
  if(lastUpdate == 900)
  {
    timeOut = 1;
    return;
  }

  if(lastUpdate > 30)
  {
    timeOut = lastUpdate - 30;
    state = TimeCorrection;
  }
  else
  {
    timeOut = lastUpdate + 2;
    state = StatCalc;
  }

  if(lastUpdate > 880)
  {
    upStat.res.clear();
    upStat.lvl = (int)town["level"].toDouble();
    for(int i = 0; i < res.count(); i++)
    {
      QJsonObject val = res.at(i).toObject();
      TRes r;
      r.amount = (int)val["amount"].toDouble();
      r.capacity = (int)val["capacity"].toDouble();
      r.priority = (int)val["priority"].toDouble();
      r.type = (int)val["type"].toDouble();
      r.trend = val["trend"].toDouble();
      r.tend = 0;
      curRes.append(r);
    }
    calcVariables();
  }
  else
  {
    qDebug() << "error calculation";
  }
}

void Statistic::calcVariables()
{
  for(int i = 0; i < curRes.count(); i++)
  {
    TRes cr = curRes.at(i);
// Tend Calc;
    for(int j = 0; j < prevRes.count(); j++)
    {
      TRes pr = prevRes.at(j);
      if(cr.type == pr.type)
      {
        cr.tend = cr.amount - pr.amount;
      }
    }
    if((cr.priority == 1)/*||(isFinal)*/)
    {
      upStat.res.append(cr);
    }
  }
  prevRes = curRes;
  printLog();
  curRes.clear();
}

void Statistic::printLog()
{
  QString msg;
  QString str;
  bool printEnable = false;
  msg.sprintf("\r          АП %d -> %d ур.          \r", upStat.lvl, upStat.lvl + 1);
  for(int i = 0; i < upStat.res.count(); i++)
  {
    TRes r = upStat.res.at(i);
    str.clear();
    str.append(name_res[r.type]);
    while(str.length() < 25)
    {
      str.append(" ");
    }
    QString s;
    s.sprintf("%d", r.amount - (int)(r.capacity * 0.67));
    if(((r.amount - (int)(r.capacity * 0.67))* -1) < minResCount)
      printEnable = true;
    while(s.length() < 10)
      s.append(" ");
    str.append(s);
    s.clear();
    s.sprintf("[%dт]\r", r.tend);
    str.append(s);
    msg.append(str);
  }
  emit this->signalPutToLog(msg);

  if(enPrintToForum && (!forumId.isEmpty()) &&(printEnable))
  {
    //delay(7 + (qrand() % 10));
    QByteArray addUrl;
    addUrl.append("/web/rpc/flash.php?interface=CorporationForumInterface&method=send");
    QByteArray param;
    msg.clear();
    str.clear();
    msg.sprintf("\\r          АП %d -> %d ур.          \\r", upStat.lvl, upStat.lvl + 1);
    QDateTime dt = QDateTime::currentDateTimeUtc();
    str = dt.toString("[hh:mm]");
    msg.append("          " + str + "(UTC)     \\r");

    for(int i = 0; i < upStat.res.count(); i++)
    {
      TRes r = upStat.res.at(i);
      str.clear();
      str.append(name_res[r.type]);
      while(str.length() < 25)
      {
        str.append(" ");
      }
      str.append("\\t\\t");
      QString s;
      s.sprintf("%d", r.amount - (int)(r.capacity * 0.67));
      while(s.length() < 10)
        s.append(" ");
      str.append(s);
      s.clear();
      s.sprintf("[%dт]\\r", r.tend);
      str.append(s);
      msg.append(str);
    }
    param.append("[\"");
    param.append(forumTheme);
    param.append("\",\"");
    param.append(msg);
    param.append("\",\"");
    param.append(forumId);
    param.append("\"]");
    //emit this->signalRequest(addUrl, param, false);
    forumPost = param;
    forumPostTimer->start((7 + qrand()%10) * 1000);
    //qDebug("print to forum");
    //qDebug() << param;
  }
}



void Statistic::setForumTheme(QString theme)
{
  //state == GetForums;
  forumTheme = theme;
  /*
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=CorporationForumInterface&method=getThreads");
  QByteArray param;
  param.append("[]");
  emit this->signalRequest(addUrl, param, true);
  */
}



void Statistic::forumThemes(QString answer)
{
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  if(jsonObject["Body"].isArray())
  {
    QJsonArray themes = jsonObject["Body"].toArray();
    for(int i = 0; i < themes.count(); i++)
    {
      QJsonValue th = themes.at(i);
      QJsonObject ob = th.toObject();
      QString name = ob["headline"].toString();
      if((!forumTheme.isEmpty())&&(name == forumTheme))
      {
        forumId.clear();
        forumId.append(ob["ID"].toString());
        //qDebug() << forumId;
      }
    }
  }

  QByteArray addUrl;
  QByteArray param;
  state = FirstRun;
  addUrl.append("/web/rpc/flash.php?interface=TownInterface&method=getDetails");
  param.append("[\"" + cityId + "\"]");
  emit this->signalRequest(addUrl, param, true);

}






void Statistic::slotForumPostTimeout()
{
  if(!forumPost.isEmpty())
  {
    QByteArray addUrl;
    addUrl.append("/web/rpc/flash.php?interface=CorporationForumInterface&method=send");
    emit this->signalRequest(addUrl, forumPost, false);
  }
}

void Statistic::slotIsFinalChange(bool final)
{
  isFinal = final;
}







