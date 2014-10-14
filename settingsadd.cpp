#include "settingsadd.h"
#include "ui_settingsadd.h"
#include "QSettings"

settingsAdd::settingsAdd(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::settingsAdd)
{
  ui->setupUi(this);
}

settingsAdd::~settingsAdd()
{
  delete ui;
}

void settingsAdd::changeEvent(QEvent *e)
{
  QWidget::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
    break;
    default:
    break;
  }
}

void settingsAdd::on_pushButton_clicked()
{
  QSettings set("settings-add", QSettings::IniFormat);
  set.beginWriteArray("List");
  TServer srv;
  TForum  forum;
  srv.name = ui->serverName->text();
  srv.town = ui->townName->text();
  srv.townId.append(ui->townId->text());
  srv.url.append(ui->serverUrl->text());
  srv.userAgent.append(ui->userAgent->text());
  srv.cookie.append(ui->cookie->text());
  srv.checkSum.append(ui->checksum->text());
  srv.userId.append(ui->userId->text());
  srv.corpId.append(ui->corpId->text());
  forum.Name.append(ui->themeName->text());
  forum.id.append(ui->theneId->text());

  for(int i = 0; i < 1; i++)
  {
    set.setArrayIndex(i);
    set.beginGroup("Servers");
    set.setValue("Name", srv.name);
    set.setValue("Town", srv.town);
    set.setValue("TownId", srv.townId);
    set.setValue("ServerUrl", srv.url);
    set.setValue("UAgent", srv.userAgent);
    set.setValue("Cookie", srv.cookie);
    set.setValue("CheckSum", srv.checkSum);
    set.setValue("UserId", srv.userId);
    set.setValue("CorpId", srv.corpId);
    set.endGroup();
    set.beginGroup("Forum");
    set.setValue("Name", forum.Name);
    set.setValue("Id", forum.id);
    set.endGroup();
  }
  set.endArray();
  this->close();

}
