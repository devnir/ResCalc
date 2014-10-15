#include "rescalc.h"
#include "ui_rescalc.h"
#include <QDateTime>

int bonCollected = 0;
ResCalc::ResCalc(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ResCalc)
{
  bonCollected = 0;
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

  connect(&bonus, SIGNAL(signalRequest(QByteArray,QByteArray, bool)), this, SLOT(slotBonusRequest(QByteArray,QByteArray, bool)));
  connect(&corp, SIGNAL(signalRequest(QByteArray,QByteArray, bool)), this, SLOT(slotCorpRequest(QByteArray,QByteArray, bool)));
  connect(&corp, SIGNAL(takedBonus()), this, SLOT(slotTakedBonus()));
  connect(&trainRep, SIGNAL(signalRequest(QByteArray,QByteArray,bool)),this, SLOT(slotTrainRequest(QByteArray,QByteArray,bool)));
  connect(&trainRep, SIGNAL(signalPutToLog(QString)), this, SLOT(slotTrainLog(QString)));

  connect(&statistic, SIGNAL(signalRequest(QByteArray,QByteArray,bool)),this, SLOT(slotStatisticRequest(QByteArray,QByteArray,bool)));
  connect(&statistic, SIGNAL(signalPutToLog(QString)), this, SLOT(slotStatisticLog(QString)));

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
    srv.userAgent = set.value("UAgent", "").toByteArray();
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
    setWindowTitle(windowTitle() + " Marivanna edition");
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

void ResCalc::ReplyFinished()
{
  reply->deleteLater();
}


void ResCalc::on_pushButton_2_clicked()
{
  TServer srv = settings.server.at(ui->serverName->currentIndex());
  if(ui->pushButton_2->isChecked())
  {
    statistic.start(srv.townId);

    if(ui->bonusCollect->isChecked())
    {
      bonCollected = 0;
      corp.start(srv.corpId, srv.userId);
    }

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
    connect(reply, SIGNAL(readChannelFinished()), this, SLOT(ReadyReadFirstReply()));
  }
  else
  {
    connect(reply, SIGNAL(readChannelFinished()), this, SLOT(ReadyReadReply()));
  }
}

void ResCalc::on_pushButton_3_clicked()
{
    ui->log->clear();
    //setForm.show();
}


void ResCalc::postStr(QString msg)
{
 /* TServer srv = settings.server.at(ui->serverName->currentIndex());
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
    connect(forumReply, SIGNAL(readChannelFinished()), this, SLOT(forumReadyReadReply()));
  }
*/
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
  {
    int sec, min;
    min = statistic.timeOut / 60;
    sec = statistic.timeOut - (min * 60);
    str.sprintf("%02d:%02d", min, sec);
    ui->timeLbl->setText(str);

    str.clear();
    str.sprintf("Собрано: %d", bonCollected);
    ui->bonColl->setText(str);
  }
}

void ResCalc::trainReadyReadReply()
{
  QString answer = QString::fromUtf8(trainReply->readAll());
  trainRep.updateData(answer);
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
    connect(bonusReply, SIGNAL(readChannelFinished()), this, SLOT(bonusReadyReadReply()));
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
  //corpReply->re
  //qDebug() << answer;
  corp.updateData(answer);

}

void ResCalc::corpReplyFinished()
{
  QString answer = QString::fromUtf8(corpReply->readAll());
  //corpReply->re
  //qDebug() << answer;
  corp.updateData(answer);
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

  qDebug() << post;
  corpReply = mgr->post(req, post);
  corpReply->setReadBufferSize(0);


  if(enReq == true)
  {
    connect(corpReply, SIGNAL(readChannelFinished()), this, SLOT(corpReadyReadReply()));
  }
  else
  {
    disconnect(corpReply, SIGNAL(readChannelFinished()), this, SLOT(corpReadyReadReply()));
  }
}


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
  //connect(trainReply, SIGNAL(finished()),this, SLOT(trainReplyFinished()));

  if(enReq == true)
  {
    connect(trainReply, SIGNAL(readChannelFinished()), this, SLOT(trainReadyReadReply()));
  }
  else
  {
    disconnect(trainReply, SIGNAL(readChannelFinished()), this, SLOT(trainReadyReadReply()));
  }
}

void ResCalc::slotStatisticLog(QString str)
{
  QString msg("Статистика. ");
  msg.append(str);

  QDateTime dt = QDateTime::currentDateTimeUtc();
  str = dt.toString("[hh:mm:ss] ");
  msg.prepend(str);
  ui->log->append(msg);
}

void ResCalc::slotStatisticRequest(QByteArray addUrl, QByteArray param, bool enReq)
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

  statisticReply = mgr->post(req, post);
  //connect(trainReply, SIGNAL(finished()),this, SLOT(trainReplyFinished()));
  connect(statisticReply, SIGNAL(readChannelFinished()), this, SLOT(statisticReadyReadReply()));
}

void ResCalc::statisticReadyReadReply()
{
  QString answer = QString::fromUtf8(statisticReply->readAll());
  statistic.updateData(answer);
}

void ResCalc::slotTakedBonus()
{
  bonCollected++;
}
