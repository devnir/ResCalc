#include "rescalc.h"
#include "ui_rescalc.h"
#include <QDateTime>
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
  bonTimeLeft = 0;
  updTimer = new QTimer(this);
  localTimer = new QTimer(this);
  postTimer= new QTimer(this);
  mgr = new QNetworkAccessManager(this);
  //onceInit();
  loadSettings();
  updTimer->setSingleShot(false);
  updTimer->setInterval(1000);
  localTimer->setSingleShot(false);
  localTimer->setInterval(1000);
  postTimer->setSingleShot(true);
  bonColTimer = new QTimer(this);
  bonColTimer->setInterval(1000);
  bonColTimer->setSingleShot(false);
  connect(bonColTimer, SIGNAL(timeout()), this, SLOT(slotBonColTimeOut()));
  connect(&corp, SIGNAL(checkDone(int)), this, SLOT(slotBonColDone(int)));
  connect(&corp, SIGNAL(signalPutToLog(QString)), this, SLOT(slotBonLog(QString)));
  qsrand (QDateTime::currentMSecsSinceEpoch());
  connect(updTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
  connect(localTimer, SIGNAL(timeout()), this, SLOT(showLocalTime()));
  connect(postTimer, SIGNAL(timeout()), this, SLOT(postTimeout()));
  connect(&bonus, SIGNAL(signalRequest(QByteArray,QByteArray, bool)), this, SLOT(slotBonusRequest(QByteArray,QByteArray, bool)));
  connect(&corp, SIGNAL(signalRequest(QByteArray,QByteArray, bool)), this, SLOT(slotCorpRequest(QByteArray,QByteArray, bool)));
  connect(&trainRep, SIGNAL(signalRequest(QByteArray,QByteArray,bool)),this, SLOT(slotTrainRequest(QByteArray,QByteArray,bool)));
  connect(&trainRep, SIGNAL(signalPutToLog(QString)), this, SLOT(slotTrainLog(QString)));
  localTimer->start();


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
void ResCalc::onceInit()
{
  TServer srv;
  srv.name.append("Паровой котел");
  srv.town.append("Брантон");
  srv.townId.append("e3804d69-9ded-4caf-9b5e-48d7397f192d");
  srv.url.append("http://s1.railnation.ru");
  srv.userAgent.append("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.124 Safari/537.36");
  srv.cookie.append("PHPSESSID=59dta47m37l9vusvp920u55sl6; _ym_visorc_22363723=b");
  srv.checkSum.append("ea24d4af2c566004782f750f940615e5");
  settings.server.clear();
  settings.server.append(srv);

  settings.forum.Name.append("Статистика АПа");
  settings.forum.id.append("4329b12f-087c-417b-a9f3-66c5b10cc917");
  saveSettings();
}

void ResCalc::saveSettings()
{
  QSettings set("settings", QSettings::IniFormat);
  set.beginGroup("Servers");
  set.beginWriteArray("List");
  for(int i = 0; i < settings.server.count(); i++)
  {
    set.setArrayIndex(i);
    TServer srv = settings.server.at(i);
    set.setValue("Name", srv.name);
    set.setValue("Town", srv.town);
    set.setValue("TownId", srv.townId);
    set.setValue("ServerUrl", srv.url);
    set.setValue("UAgent", srv.userAgent);
    set.setValue("Cookie", srv.cookie);
    set.setValue("CheckSum", srv.checkSum);
  }
  set.endArray();
  set.endGroup();
  set.beginGroup("Forum");
  set.setValue("Name", settings.forum.Name);
  set.setValue("Id", settings.forum.id);
  set.endGroup();
}

void ResCalc::loadSettings()
{
  settings.server.clear();
  QSettings set("settings.dat", QSettings::IniFormat);

  int count = set.beginReadArray("List");
  for(int i = 0; i < count; i++)
  {
    set.setArrayIndex(i);
    set.beginGroup("Servers");
    TServer srv;
    srv.name = set.value("Name", "").toString();
    srv.town = set.value("Town", "").toString();
    srv.townId = set.value("TownId", "").toByteArray();
    srv.url = set.value("ServerUrl", "").toByteArray();
    srv.userAgent = set.value("").toByteArray();
    srv.cookie = set.value("Cookie","").toByteArray();
    srv.checkSum = set.value("CheckSum", "").toByteArray();
    srv.userId = set.value("UserId", "").toByteArray();
    srv.corpId = set.value("CorpId", "").toByteArray();
    set.endGroup();
    set.beginGroup("Forum");
    settings.forum.Name = set.value("Name", "").toByteArray();
    settings.forum.id = set.value("Id", "").toByteArray();
    set.endGroup();
    settings.server.append(srv);
  }
  set.endArray();

  if(count == 0)
  {
      set.beginGroup("Servers");
      TServer srv;
      srv.name = set.value("Name", "Блок Цилиндров").toString();
      srv.town = set.value("Town", "Кэмптаун").toString();
      srv.townId = set.value("TownId", "b4f57fb2-16bc-441e-8ef5-51ccde0fd754").toByteArray();
      srv.url = set.value("ServerUrl", "http://s6.railnation.ru").toByteArray();
      srv.userAgent = set.value("UAgent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.101 Safari/537.36").toByteArray();
      srv.cookie = set.value("Cookie","_ym_visorc_23073562=b; _ym_visorc_22363723=b; PHPSESSID=el7pe740e2hp5o4fsklsi48ng2").toByteArray();
      srv.checkSum = set.value("CheckSum", "ea24d4af2c566004782f750f940615e5").toByteArray();
      srv.userId = set.value("UserId", "98c378ee-3b4c-c6b0-0f33-e5c8e8e1e709").toByteArray();
      srv.corpId = set.value("CorpId", "49ca6200-ee27-0ce0-abe5-489dc1ba89ac").toByteArray();
      set.endGroup();
      set.beginGroup("Forum");
      settings.forum.Name = set.value("Name", "UP Statistic").toByteArray();
      settings.forum.id = set.value("Id", "40a4ddb3-186d-9829-f4b9-462a6907c77f").toByteArray();
      set.endGroup();
      settings.server.append(srv);
  }
  ui->serverName->clear();
  for(int i = 0; i < settings.server.count(); i++)
  {
    ui->serverName->addItem(settings.server.at(i).name);
  }
}


void ResCalc::forumReadyReadReply()
{
  QString answer = QString::fromUtf8(forumReply->readAll());
  //qDebug() << answer;
}

void ResCalc::forumReplyFinished()
{
  forumReply->deleteLater();
}

void ResCalc::ReadyReadReply()
{
  QString answer = QString::fromUtf8(reply->readAll());
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();
  QJsonArray res = bodyObj["resources"].toArray();
  QJsonObject town = bodyObj["town"].toObject();

  int lastUpdate = (int)town["lastConsumption"].toDouble();
  //qDebug() << lastUpdate;
  int timeOut = 902 + lastUpdate;
  secCounter = timeOut;

  int ind = town["level"].toDouble();
  upStat.lvl = ind;
  upStat.res.clear();
  for(int i = 0; i < res.count(); i++)
  {
    QJsonObject val = res.at(i).toObject();
    resData.putData(val);
    if( val["priority"].toDouble() == 1.)
    {
      int type = val["type"].toDouble();
      int poloj = (int)val["amount"].toDouble() - (int)potreb_low[ind]/* - (int)val["consume_amount"].toDouble()*/;      
      TRes r;
      r.id = type;
      r.count = poloj;
      r.trend = (float)val["trend"].toDouble();
      upStat.res.append(r);
      //qDebug() << "---------" + name_res[r.id] + "----------";
      //qDebug() << val;
    }
  }
  resData.calcTrend();
  QString msg;
  for(int i = 0; i < upStat.res.count(); i++)
  {
    TRes res = upStat.res.at(i);
    QString str;
    QString s;
    s.sprintf("%d", res.count);
    QString tab_sp = "";

    str = name_res[res.id] + ":";
    while(str.length() < 30)
      str.append(" ");

    tab_sp.append("\t");

    str.append(tab_sp + s);    
    msg.append(str);
    str.clear();
    int trend = resData.getTrendById(res.id);
    str.sprintf("\t Т: %dт. \r", trend);
    msg.append(str);
  }
  ui->log->append(msg);

  updTimer->start();
  int r = qrand() % 10;
  postTimer->start(1000*r);
}

void ResCalc::ReadyReadFirstReply()
{
  QString answer = QString::fromUtf8(reply->readAll());
  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  QJsonObject bodyObj = jsonObject["Body"].toObject();

  QJsonObject town = bodyObj["town"].toObject();
  //qDebug() << answer;
  int lastUpdate = (int)town["lastConsumption"].toDouble();
  //qDebug() << lastUpdate;
  int timeOut = 902 + lastUpdate;
  if(lastUpdate == 0)
    timeOut = 7;

  secCounter = timeOut;
  QString str;
  str.sprintf("Ожидание следующего пересчета: %d", timeOut);
  ui->log->append(str);
  updTimer->start();
}

void ResCalc::ReplyFinished()
{
  reply->deleteLater();
}


void ResCalc::on_pushButton_2_clicked()
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  if(ui->pushButton_2->isChecked())
  {
//    if(ui->upStatCheck->isChecked())
      reqPrepare(true);

    if(ui->bonusCollect->isChecked())
      corp.start(srv.corpId, srv.userId);

    if(ui->repairCheckBox->isChecked())
    {
      trainRep.start(srv.userId, ui->reabil->value());
    }

    ui->reabil->setEnabled(false);
    ui->repairCheckBox->setEnabled(false);
    //ui->upStatCheck->setEnabled(false);
    ui->bonusCollect->setEnabled(false);
    ui->pushButton_2->setText("Stop");
  }
  else
  {
    updTimer->stop();
    postTimer->stop();
    bonColTimer->stop();
    trainRep.stop();
    ui->reabil->setEnabled(true);
    ui->pushButton_2->setText("Go!");
    //ui->upStatCheck->setEnabled(true);
    ui->bonusCollect->setEnabled(true);
    ui->repairCheckBox->setEnabled(true);
  }
}

void ResCalc::timeOut()
{
  int sec, min;
  min = secCounter / 60;
  sec = secCounter - (min * 60);
  if(secCounter == 63)
  {
    reqPrepare(true);
  }
  if(secCounter == 23)
  {
    reqPrepare(true);
  }

  if(secCounter == 0)
  {
    reqPrepare(false);
  }

  if(secCounter < -10)
  {
    reqPrepare(true);
  }

  QString str;
  str.sprintf("%02d:%02d", min, sec);
  ui->timeLbl->setText(str);
  secCounter--;
}

void ResCalc::reqPrepare(bool correction)
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  QByteArray reqUrl;
  reqUrl.append(srv.url);
  reqUrl.append("/web/rpc/flash.php?interface=TownInterface&method=getDetails");
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",srv.userAgent);
  req.setRawHeader("Cookie", srv.cookie);

  QByteArray strForHash;
  strForHash.append("[\"");
  strForHash.append(srv.townId);
  strForHash.append("\"]");

  QByteArray hash;
  hash = QCryptographicHash::hash ((strForHash),QCryptographicHash::Md5).toHex();
  QByteArray post("{\"parameters\":");
  post.append(strForHash);
  post.append(",\"client\":1,\"hash\":\"");
  post.append(hash);
  post.append("\",\"checksum\":\"");
  post.append(srv.checkSum);
  post.append("\"}");
  reply = mgr->post(req, post);
  connect(reply, SIGNAL(finished()),this, SLOT(ReplyFinished()));
  if(correction)
  {
    ui->log->append("Синхронизация времени.");
    connect(reply, SIGNAL(readyRead()), this, SLOT(ReadyReadFirstReply()));
  }
  else
  {
    connect(reply, SIGNAL(readyRead()), this, SLOT(ReadyReadReply()));
  }
}

void ResCalc::on_pushButton_3_clicked()
{
    ui->log->clear();
    //setForm.show();
}

void ResCalc::postTimeout()
{
  QString str;
  QString msg;
  str.sprintf("--АП %d -> %d ур. --\\r", upStat.lvl, upStat.lvl + 1);
  str.toUpper();
  msg.append(str);
  str.clear();
  QDateTime dt = QDateTime::currentDateTimeUtc();
  str = dt.toString("-- dd.MM.yyyy hh:mm -- UTC --\\r");
  msg.append(str);

  for(int i = 0; i < upStat.res.count(); i++)
  {
    TRes res = upStat.res.at(i);
    str.clear();
    QString s;
    int trend = resData.getTrendById(res.id);
    s.sprintf("%d    (Т: %d т.)\\r", res.count, trend);
    QString tab_sp = "";

    str = name_res[res.id] + ":";
    while(str.length() < 20)
      str.append(" ");

    tab_sp.append("\\t");

    str.append(tab_sp + s);
    msg.append(str);
  }
  postStr(msg);
}

void ResCalc::postStr(QString msg)
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  TForum  forum = settings.forum;
  QByteArray reqUrl;
  reqUrl.append(srv.url);
  reqUrl.append("/web/rpc/flash.php?interface=CorporationForumInterface&method=send");
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",srv.userAgent);
  req.setRawHeader("Cookie", srv.cookie);

  QByteArray strForHash;
  strForHash.append("[\"");
  strForHash.append(forum.Name);
  strForHash.append("\",\"");
  strForHash.append(msg);
  strForHash.append("\",\"");
  strForHash.append(forum.id);
  strForHash.append("\"]");

  QByteArray hash;
  hash = QCryptographicHash::hash ((strForHash),QCryptographicHash::Md5).toHex();
  QByteArray post("{\"parameters\":");
  post.append(strForHash);
  post.append(",\"client\":1,\"hash\":\"");
  post.append(hash);
  post.append("\",\"checksum\":\"");
  post.append(srv.checkSum);
  post.append("\"}");

  //qDebug() << post;  
  bool postEnable = false;
  for(int i = 0; i < upStat.res.count(); i++)
  {
    TRes res = upStat.res.at(i);
    if(res.count > (ui->upBoxVount->value() * -1))
        postEnable = true;
  }
  if(ui->upStatCheck->isChecked() == false)
      postEnable = false;
  if(postEnable)
  {
    forumReply = mgr->post(req, post);
    connect(forumReply, SIGNAL(finished()),this, SLOT(forumReplyFinished()));
    connect(forumReply, SIGNAL(readyRead()), this, SLOT(forumReadyReadReply()));
  }

}

void ResCalc::showLocalTime()
{
  QString str;
  QDateTime dt = QDateTime::currentDateTimeUtc();
  str = dt.toString("hh:mm:ss (UTC)");
  ui->localTime->setText(str);

  if(bonTimeLeft >=0)
  {
    int sec, min;
    min = bonTimeLeft / 60;
    sec = bonTimeLeft - (min * 60);
    str.sprintf("%02d:%02d", min, sec);
    ui->bonusTimer->setText(str);
    bonTimeLeft--;
  }
}


void ResCalc::trainReadyReadReply()
{
  QString answer = QString::fromUtf8(trainReply->readAll());
  trainRep.updateData(answer);
}

void ResCalc::trainReplyFinished()
{

}

void ResCalc::on_repairBtn_clicked()
{
  for(int i = 0; i < myTrain.count(); i++)
  {
    TMyTrain tTr = myTrain.at(i);
    if(tTr.reliability < ui->reabil->value())
    {
      myRepairReq(tTr);
    }
  }
}

void ResCalc::myRepairReq(TMyTrain train)
{

}

void ResCalc::slotBonusRequest(QByteArray addUrl, QByteArray param, bool enReq)
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  QByteArray reqUrl;
  reqUrl.append(srv.url);
  reqUrl.append(addUrl);
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",srv.userAgent);
  req.setRawHeader("Cookie", srv.cookie);

  QByteArray hash;
  hash = QCryptographicHash::hash ((param),QCryptographicHash::Md5).toHex();
  QByteArray post("{\"parameters\":");
  post.append(param);
  post.append(",\"client\":1,\"hash\":\"");
  post.append(hash);
  post.append("\",\"checksum\":\"");
  post.append(srv.checkSum);
  post.append("\"}");

  bonusReply = mgr->post(req, post);

  if(enReq ==true)
  {
    connect(bonusReply, SIGNAL(readyRead()), this, SLOT(bonusReadyReadReply()));
    connect(bonusReply, SIGNAL(finished()),this, SLOT(bonusReplyFinished()));
  }
}


void ResCalc::bonusReplyFinished()
{
  bonusReply->deleteLater();
}

void ResCalc::bonusReadyReadReply()
{
  QString answer = QString::fromUtf8(bonusReply->readAll());
  bonus.updateData(answer);
}

void ResCalc::corpReadyReadReply()
{
  QString answer = QString::fromUtf8(corpReply->readAll());
  corp.updateData(answer);
}

void ResCalc::corpReplyFinished()
{
  //corpReply
}

void ResCalc::slotCorpRequest(QByteArray addUrl, QByteArray param, bool enReq)
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  QByteArray reqUrl;
  reqUrl.append(srv.url);
  reqUrl.append(addUrl);
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",srv.userAgent);
  req.setRawHeader("Cookie", srv.cookie);

  QByteArray hash;
  hash = QCryptographicHash::hash ((param),QCryptographicHash::Md5).toHex();
  QByteArray post("{\"parameters\":");
  post.append(param);
  post.append(",\"client\":1,\"hash\":\"");
  post.append(hash);
  post.append("\",\"checksum\":\"");
  post.append(srv.checkSum);
  post.append("\"}");

  //qDebug() << post;
  corpReply = mgr->post(req, post);
  connect(corpReply, SIGNAL(finished()),this, SLOT(corpReplyFinished()));

  if(enReq == true)
  {
    connect(corpReply, SIGNAL(readyRead()), this, SLOT(corpReadyReadReply()));
  }
  else
  {
    disconnect(corpReply, SIGNAL(readyRead()), this, SLOT(corpReadyReadReply()));
  }
}
/*
void ResCalc::on_tstBtn_clicked()
{
  setForm.show();
}
*/

void ResCalc::slotBonColDone(int nextWait)
{
  bonTimeLeft = nextWait;
  bonColTimer->start(nextWait * 1000);
}

void ResCalc::slotBonColTimeOut()
{
  bonColTimer->stop();
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  corp.start(srv.corpId, srv.userId);
}

void ResCalc::slotBonLog(QString str)
{
  QString msg("Сбор бонусов. ");
  msg.append(str);  

  QDateTime dt = QDateTime::currentDateTimeUtc();
  str = dt.toString("[hh:mm:ss] ");
  msg.prepend(str);
  ui->log->append(msg);
}

void ResCalc::slotTrainLog(QString str)
{
  QString msg("Авторемонт. ");
  msg.append(str);

  QDateTime dt = QDateTime::currentDateTimeUtc();
  str = dt.toString("[hh:mm:ss] ");
  msg.prepend(str);
  ui->log->append(msg);
}

void ResCalc::slotTrainRequest(QByteArray addUrl, QByteArray param, bool enReq)
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  QByteArray reqUrl;
  reqUrl.append(srv.url);
  reqUrl.append(addUrl);
  QString url(reqUrl);
  QNetworkRequest req(url);
  req.setRawHeader("Content-Type","application/json");
  req.setRawHeader("User-Agent",srv.userAgent);
  req.setRawHeader("Cookie", srv.cookie);

  QByteArray hash;
  hash = QCryptographicHash::hash ((param),QCryptographicHash::Md5).toHex();
  QByteArray post("{\"parameters\":");
  post.append(param);
  post.append(",\"client\":1,\"hash\":\"");
  post.append(hash);
  post.append("\",\"checksum\":\"");
  post.append(srv.checkSum);
  post.append("\"}");

  //qDebug() << post;
  trainReply = mgr->post(req, post);
  connect(trainReply, SIGNAL(finished()),this, SLOT(trainReplyFinished()));

  if(enReq == true)
  {
    connect(trainReply, SIGNAL(readyRead()), this, SLOT(trainReadyReadReply()));
  }
  else
  {
    disconnect(trainReply, SIGNAL(readyRead()), this, SLOT(trainReadyReadReply()));
  }
}
