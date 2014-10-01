#include "rescalc.h"
#include "ui_rescalc.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>

QByteArray name_res[] =
  {
   "Все товары","Уголь","Пшеница","Железо","Дерево",
   "Доски","Железная руда","Хлопок","Тексиль","Скот",
   "Колбаса","Нить","Инструменты","Бумага","Кожа","Хлеб",
   "Мука","Медная руда","Кварц","Медь","Сталь","Обувь",
   "Стекло","Проволка","Трубы","Упаковка","Окна",
   "Листовой металл","Силикаты","Еда","Нефть","Лампы",
   "Химикаты","Одежда","Нержавеющая сталь","Бокситы",
   "Двигатели","Пластик","Алюминий","Керамика",
   "Стальные балки","Бензин","Автомобили","Хоз. товары",
   "Электроника","Игрушки","Спортивные товары",
   "Сантехника","Лекарства"
  };
TForumMsg forumMsg;
int potreb_low[] =
{
  0, 2345, 2680, 3, 3551, 4087, 4690, 5360, 6063, 6800, 7571, 8375, 9212, 10083,
  10988, 11926, 12897, 13902, 14941, 16013, 17119, 18257, 19430, 20636, 21875,
  25, 24455, 25795, 28, 28575, 30016, 31490, 32998, 33, 36113, 37721, 39362,
  41037, 42746, 44488, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
  54, 55, 56, 57, 58, 59, 60
};


ResCalc::ResCalc(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ResCalc)
{
  ui->setupUi(this);
  mgr = new QNetworkAccessManager(this);
  replayTimer = new QTimer(this);
  replayTimer->setSingleShot(false);
  connect(replayTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
  firstRun = false;
  replayTimer->setInterval(1000);
  loadSettings();
}

ResCalc::~ResCalc()
{
  delete ui;
}

void ResCalc::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
    break;
    default:
    break;
  }
}

void ResCalc::getReplyFinished()
{
  reply->deleteLater();
}

void ResCalc::readyReadReply()
{
  if(firstRun == true)
  {
    firstPrepare();
  }
  else
  {
    prepareData();
  }
}

void ResCalc::correctionReply()
{
  QString answer = QString::fromUtf8(reply->readAll());
  QString strReply = answer;

  QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonArray res = bodyObj["resources"].toArray();
  QJsonObject town = bodyObj["town"].toObject();


  int lastUpdate = (int)town["lastConsumption"].toDouble();
  int timeOut = 900 + lastUpdate + 1;
  firstRun = false;
  secCounter = timeOut;
  QString s;
  s.sprintf("-- Корректировка, осталось: %d с.", secCounter);
}

/*
void MainWindow::on_pushButton_2_clicked()
{

}
*/


void ResCalc::loadSettings()
{
  serverList.clear();
  QSettings set("settings", QSettings::IniFormat);
  set.beginGroup("Server");

  int count = set.beginReadArray("List");
  for(int i = 0; i < count; i++)
  {
    set.setArrayIndex(i);
    TServerList sL;
    sL.serverName = set.value("ServerName", "NoName").toString();
    sL.serverUrl = set.value("ServerUrl", "").toByteArray();
    sL.userAgent = set.value("UAgent", "").toByteArray();
    sL.cookie = set.value("Cookie","").toByteArray();
    sL.townName = set.value("TownName", "").toString();
    sL.post = set.value("Post", "").toByteArray();
    serverList.append(sL);
  }
  set.endArray();
  set.endGroup();

  ui->serverCombo->clear();
  for(int i = 0; i < serverList.count(); i++)
  {
    ui->serverCombo->addItem(serverList.at(i).serverName);
  }
}

void ResCalc::saveSettings()
{
  TServerList sL;
  sL.serverName = "Blok Cilindrov";
  sL.serverUrl = "http://s6.railnation.ru/web/rpc/flash.php?interface=TownInterface&method=getDetails";
  sL.userAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.103 Safari/537.36";
  sL.cookie = "PHPSESSID=g3825c410ciog3cfpre0f3vt55; _ym_visorc_22363723=b";
  sL.post = "{\"hash\":\"08eb3018b7d1da740aab72ca2b98570a\",\"checksum\":\"ea24d4af2c566004782f750f940615e5\",\"client\":1,\"parameters\":[\"ca95cb22-9695-425d-aae2-fa8f04eb0935\"]}";
  serverList.append(sL);

  QSettings set("settings", QSettings::IniFormat);
  set.beginGroup("Server");
  set.beginWriteArray("List");
  for(int i = 0; i < 2; i++)
  {
    set.setArrayIndex(i);
    TServerList slt;
    slt = serverList.at(0);
    set.setValue("ServerName", slt.serverName);
    set.setValue("TownName", "Longbridg");
    set.setValue("UAgent", slt.userAgent);
    set.setValue("ServerUrl", slt.serverUrl);
    set.setValue("Cookie", slt.cookie);
    set.setValue("Post", slt.post);
  }
  set.endArray();
  set.endGroup();


}

void ResCalc::on_StartBtn_clicked()
{
  firstRun = true;
  TServerList slt;
  slt = serverList.at(ui->serverCombo->currentIndex());
  QString url(slt.serverUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  //req.setRawHeader("Accept", "application/json");
  //req.setRawHeader("Origin", serverUrl);
  req.setRawHeader("User-Agent",slt.userAgent);
  //req.setRawHeader("Referer", referer);
  //req.setRawHeader("Accept-Encoding", "gzip,deflate,sdch");
  //req.setRawHeader("Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4");
  req.setRawHeader("Cookie", slt.cookie);
  reply = manager.post(req, slt.post);
  connect(reply, SIGNAL(finished()),this, SLOT(getReplyFinished()));
  connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadReply()));
}

void ResCalc::firstPrepare()
{
  QString answer = QString::fromUtf8(reply->readAll());
  QString strReply = answer;

  QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonArray res = bodyObj["resources"].toArray();
  QJsonObject town = bodyObj["town"].toObject();

  int lastUpdate = (int)town["lastConsumption"].toDouble();
  int timeOut = 900 + lastUpdate;
  secCounter = timeOut;
  //ui->logData->append("-----TimerInit-----");
  QString str;
  str.sprintf("Wait next recalc: %d", timeOut);
  ui->logData->append(str);
  qDebug() << str;
  //replayTimer->setInterval();
  firstRun = false;
  replayTimer->start(999);
}

void ResCalc::prepareData()
{
  QString answer = QString::fromUtf8(reply->readAll());
  QString strReply = answer;

  QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonArray res = bodyObj["resources"].toArray();
  QJsonObject town = bodyObj["town"].toObject();


  int lastUpdate = (int)town["lastConsumption"].toDouble();
  int timeOut = 900 + lastUpdate + 1;
  //replayTimer->setInterval();
  firstRun = false;
  secCounter = timeOut;
  int ind = town["level"].toDouble();
  QString s;
  s.sprintf("--АП %d-%d ур.\n Погрешность: %d c.", ind, ind + 1, lastUpdate);
  forumMsg.lvl = ind;
  ui->logData->append(s);
  for(int i = 0; i < res.count(); i++)
  {
    QJsonObject val = res.at(i).toObject();

    if( val["priority"].toDouble() == 1.)
    {
      int type = val["type"].toDouble();
      int poloj = (int)val["amount"].toDouble() - (int)potreb_low[ind]/* - (int)val["consume_amount"].toDouble()*/;
      QString str;
      str.sprintf("%d", poloj);
      QString strOut;
      strOut = name_res[type] + ": " + str;
      ui->logData->append(strOut);
      forumMsg.res[i].id = type;
      forumMsg.res[i].count = poloj;
    }
  }
  on_pushButton_clicked();
}

void ResCalc::timeOut()
{
  int sec, min;
  min = secCounter / 60;
  sec = secCounter - (min * 60);
  if(secCounter == 20)
  {
    prepareCorrection();
  }

  if(secCounter < 0)
  {
    putRequest();
  }

  QString str;
  str.sprintf("Time left: %02d:%02d", min, sec);
  ui->label->setText(str);
  secCounter--;

}

void ResCalc::prepareCorrection()
{
  TServerList slt;
  slt = serverList.at(ui->serverCombo->currentIndex());
  QString url(slt.serverUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  //req.setRawHeader("Accept", "application/json");
  //req.setRawHeader("Origin", serverUrl);
  req.setRawHeader("User-Agent",slt.userAgent);
  //req.setRawHeader("Referer", referer);
  //req.setRawHeader("Accept-Encoding", "gzip,deflate,sdch");
  //req.setRawHeader("Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4");
  req.setRawHeader("Cookie", slt.cookie);
  reply = manager.post(req, slt.post);
  connect(reply, SIGNAL(finished()),this, SLOT(getReplyFinished()));
  connect(reply, SIGNAL(readyRead()), this, SLOT(correctionReply()));

}

void ResCalc::putRequest()
{
  TServerList slt;
  slt = serverList.at(ui->serverCombo->currentIndex());
  QString url(slt.serverUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",slt.userAgent);
  req.setRawHeader("Cookie", slt.cookie);
  reply = manager.post(req, slt.post);
  connect(reply, SIGNAL(finished()),this, SLOT(getReplyFinished()));
  connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadReply()));
}

void ResCalc::on_pushButton_clicked()
{
  /*if(forumMsg.lvl == 0)
    return;
*/
  TServerList slt;
  slt = serverList.at(ui->serverCombo->currentIndex());

  //QByteArray reqUrl("http://s1.railnation.ru/web/rpc/flash.php?interface=CorporationForumInterface&method=send"); //форум отправить пост
  //QByteArray reqUrl("http://s1.railnation.ru/web/rpc/flash.php?interface=CorporationInterface&method=get"); //инфа про ассу
  QByteArray reqUrl("http://s1.railnation.ru/web/rpc/flash.php?interface=CorporationForumInterface&method=getThreads");
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",slt.userAgent);
  req.setRawHeader("Cookie", slt.cookie);
/*
  QString post;
  post.sprintf("{\"parameters\":[\"UpStatistic\",\"АП %d->%d ур.\r", forumMsg.lvl, forumMsg.lvl + 1);
  int resCount;
  if(forumMsg.lvl == 1)
  {
    resCount = 3;
  }
  else
  {
    resCount = 4;
  }

  for(int i = 0; i < resCount; i++)
  {
    post.append(name_res[forumMsg.res[i].id]);
    QString str;
    str.sprintf(":\t%d\r", forumMsg.res[i].id);
    post.append(str);
  }
  post.append("\",\"8f61fd0d-647c-9f6d-0941-4de25e0d928c\"],\"client\":1,\"checksum\":\"ea24d4af2c566004782f750f940615e5\",\"hash\":\"18266b5a590fca288b104cf87e8e3c76\"}");
  QByteArray send;
  send.append(post);
  qDebug() << send;*/

  //QByteArray send("{\"parameters\":[\"UpStatistic\",\"1235\",\"8f61fd0d-647c-9f6d-0941-4de25e0d928c\"],\"client\":1,\"hash\":\"46030d83f03a3295766414d3192a8c12\",\"checksum\":\"ea24d4af2c566004782f750f940615e5\"}"); // попытка поста
  //QByteArray send("{\"parameters\":[\"edd8c452-b501-5d54-77c5-e0fcfb6f5c9c\"],\"client\":1,\"hash\":\"7d607d5432f856a1e54bfaaaca6123ae\",\"checksum\":\"ea24d4af2c566004782f750f940615e5\"}"); // инфа про ассу
  //QByteArray send("{\"parameters\":[],\"client\":1,\"hash\":\"d751713988987e9331980363e24189ce\",\"checksum\":\"ea24d4af2c566004782f750f940615e5\"}");
  QByteArray send("{\"parameters\":[\"Flood\",\"1\",\"fe681ac5-d2b5-92b4-e81a-edec6122f5f0\"],\"client\":1,\"hash\":\"e234ddb382e76d28176fe7f6e0e35861\",\"checksum\":\"ea24d4af2c566004782f750f940615e5\"}");
  reply = manager.post(req, send);

  connect(reply, SIGNAL(finished()),this, SLOT(ReplyFinishedDBG()));
  connect(reply, SIGNAL(readyRead()), this, SLOT(readyReadReplyDBG()));
  forumMsg.lvl = 0;
}
void ResCalc::ReplyFinishedDBG()
{
  reply->deleteLater();
}

void ResCalc::readyReadReplyDBG()
{
  QString answer = QString::fromUtf8(reply->readAll());
  qDebug() << answer;
}


/*
e4135b18-9b4b-9379-0fec-cebe808fec4b     - user_id

*/

void ResCalc::on_pushButton_2_clicked()
{

  QByteArray str;
  str.append(ui->lineEdit->text());
  QByteArray etalon("e234ddb382e76d28176fe7f6e0e35861");
  QByteArray etalon1("28c8edde3d61a0411511d3b1866f0636");
  for(int i = 0; i < 1; i++)
  {
    str = QCryptographicHash::hash ((str),QCryptographicHash::Md5).toHex();
    qDebug() << str;
    if(str == etalon)
    {
      qDebug() << i;
      qDebug() << str;
    }
  }
  qDebug("Done");
}
