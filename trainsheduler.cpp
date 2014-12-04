#include "trainsheduler.h"
#include "ui_trainsheduler.h"
#include <QDebug>
#include <QSettings>

trainSheduler::trainSheduler(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::trainSheduler)
{
  ui->setupUi(this);
  scene = new QGraphicsScene;
  ui->mapViev->setScene(scene);
  readTownNames();
}

trainSheduler::~trainSheduler()
{
  delete ui;
}

void trainSheduler::changeEvent(QEvent *e)
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


void trainSheduler::init(TServer server)
{
  this->srv = server;
  mgr = new QNetworkAccessManager(this);
}


void trainSheduler::on_pushButton_clicked()
{
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=TrainInterface&method=getTrains");
  QByteArray param("[true]");
  sendQuery(addUrl, param, true);

}

void trainSheduler::sendQuery(QByteArray addUrl, QByteArray param, bool enReq)
{
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
  QNetworkReply *netReply = mgr->post(req, post);
  connect(netReply, SIGNAL(readChannelFinished()),this, SLOT(on_readChannelFinished()));

}

void trainSheduler::on_readChannelFinished()
{
  QObject *from = sender();

  if(from == NULL)
  {
    qDebug("NULL, returning");
    return;
  }

  QNetworkReply *repl;
  repl = (QNetworkReply*)from;
  QString answer = QString::fromUtf8(repl->readAll());

  QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
  QJsonObject jsonObject = jsonResponse.object();
  if(jsonObject.value("Errorcode").toInt() == 0)
  {
    qDebug("ALL OK");
    QUrl url = repl->url();
    QUrlQuery query(url.query());
    if(query.queryItemValue("interface").contains("TrainInterface") &&
       query.queryItemValue("method").contains("getTrains"))
    {
      getTrains(jsonObject);
    }
    if(query.queryItemValue("interface").contains("LocationInterface") &&
       query.queryItemValue("method").contains("get"))
    {
      getLocations(jsonObject);
    }

    if(query.queryItemValue("interface").contains("RailInterface") &&
       query.queryItemValue("method").contains("get"))
    {
      getRoads(jsonObject);
    }
  }
  else
  {
    qDebug("QUERY ERROR!!!!!!!");
  }
  repl->deleteLater();
}



void trainSheduler::getTrains(QJsonObject jObj)
{
  QJsonArray bodyObj = jObj["Body"].toArray();
  myTrains.clear();
  for(int i = 0; i < bodyObj.count(); i++)
  {
    QJsonObject train = bodyObj.at(i).toObject();
    TTrainCard tc;
    tc.id.append(train.value("ID").toString());
    tc.name.append(train.value("name").toString());
    if(tc.name.isEmpty())
      tc.name.append("NoName");
    myTrains.append(tc);
  }
  guiPrinTrainList();
}

void trainSheduler::guiPrinTrainList()
{
  ui->trainsTree->clear();
  for(int i = 0; i < myTrains.count(); i++)
  {
    //Создаем переменную класса управления элементом treeWidget
    // и показываем ему компонент на форме
    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->trainsTree);
    QTreeWidgetItem *chld = new QTreeWidgetItem();
    //Записываем в treeWidget в первую колонку(она имеет код "0")
    // наименование группы
    itm->setText(0, myTrains.at(i).name);
    chld->setText(0, "Id");
    chld->setText(1, myTrains.at(i).id);
    itm->addChild(chld);
  }
}

void trainSheduler::on_pushButton_2_clicked()
{
  //location
  //http://s6.railnation.ru/web/rpc/flash.php?interface=LocationInterface&method=get
  QByteArray addUrl;
  addUrl.append("/web/rpc/flash.php?interface=LocationInterface&method=get");
  QByteArray param("[true]");
  sendQuery(addUrl, param, true);

  addUrl.clear();
  param.clear();

}

void trainSheduler::getLocations(QJsonObject jObj)
{
  QJsonArray bodyObj = jObj["Body"].toArray();
  globalMap.clear();
  for(int i = 0; i < bodyObj.count(); i++)
  {
    QJsonObject location = bodyObj.at(i).toObject();
    TLocation loc;
    int type = location.value("type").toInt();
    int tX = location.value("x").toInt();
    int tY = location.value("y").toInt();

    loc.id.append(location.value("ID").toString());
    loc.type = type;
    loc.x = location.value("x").toInt();
    loc.y = location.value("y").toInt();
    if(loc.type == 2)
    {
      loc.name.append(townNames[location.value("name").toInt()]);
      if(loc.id == srv.townId)
      {
        homeTown =  loc;
      }
    }
    globalMap.append(loc);
  }

  QByteArray  addUrl;
  addUrl.append("/web/rpc/flash.php?interface=RailInterface&method=get");
  QByteArray param;
  param.append("[\"");
  param.append(srv.userId);
  param.append("\"]");
  sendQuery(addUrl, param, true);

  printMiniMap();
}


void trainSheduler::printMiniMap()
{
  scene->clear();
  for(int i = 0; i < globalMap.count(); i++)
  {
    TLocation loc = globalMap.at(i);
    QBrush brush(Qt::SolidPattern);
    QColor color;
    QPen pen;
    pen.setColor(Qt::black);
    brush.setColor(color);
    if(loc.type == 2)
    {
      color = QColor::fromRgb(0xFF, 0, 0x00);
    }
    else
    {
      color = QColor::fromRgb(0xA0, 0xA0, 0xA0);
    }
    cityItem *itm = new cityItem;
    pen.setColor(color);
    brush.setColor(color);
    itm->setPen(pen);
    itm->setBrush(brush);
    itm->setRect(loc.x * 2 - 4, loc.y * 2-4., 8., 8.);
    scene->addItem(itm);
  }

}

void trainSheduler::printHomeLoc(int scale, int size)
{
  scene->clear();
  QBrush brush(Qt::SolidPattern);
  QColor color;
  QPen pen;
  int minX = 0, minY = 0;
  int maxX = 0, maxY = 0;
  minX = homeTown.x - size;
  minY = homeTown.y - size;
  maxX = homeTown.x + size;
  maxY = homeTown.y + size;
/*
  for(int i = 0; i < globalMap.count(); i++)
  {
    TLocation loc = globalMap.at(i);
    pen.setColor(Qt::black);
    brush.setColor(color);
    if(loc.x > minX && loc.x < maxX &&
       loc.y > minY && loc.y < maxY)
    {
      if(loc.type == 2)
      {
        color = QColor::fromRgb(0xFF, 0, 0x00);
      }
      else
      {
        color = QColor::fromRgb(0xA0, 0xA0, 0xA0);
      }
      cityItem *itm = new cityItem;
      pen.setColor(color);
      brush.setColor(color);
      itm->setPen(pen);
      itm->setBrush(brush);
      itm->setRect((loc.x - minX) * scale - 12, (loc.y - minY) * scale - 12., 24., 24.);
      scene->addItem(itm);
    }
  }
*/
  for(int i = 0; i < myRoads.count(); i++)
  {
    qDebug("tryRoad");
    TRoads road = myRoads.at(i);
    if(road.loc1.x > minX && road.loc1.x < maxX &&
       road.loc1.y > minY && road.loc1.y < maxY &&
       road.loc2.x > minX && road.loc2.x < maxX &&
       road.loc2.y > minY && road.loc2.y < maxY
       )
    {
      qDebug("print road");
      color = QColor::fromRgb(0x00, 0x00, 0x00);
      pen.setColor(color);
      QGraphicsLineItem *itm = new QGraphicsLineItem;
      itm->setPen(pen);
      itm->setLine((road.loc1.x - minX) * scale,
                   (road.loc1.y - minY) * scale,
                   (road.loc2.x - minX) * scale,
                   (road.loc2.y - minY) * scale);
      scene->addItem(itm);
    }
  }
}

void trainSheduler::readTownNames()
{
  townNames[1]="Нью Корк";
  townNames[2]="Синевилль";
  townNames[3]="Вишингтон";
  townNames[4]="Миллтон";
  townNames[5]="Окхилл";
  townNames[6]="Вестхилл";
  townNames[7]="Нью Васюки";
  townNames[8]="Петертаун";
  townNames[9]="Лонкильн";
  townNames[10]="Олдтаун";
  townNames[11]="Дорри";
  townNames[12]="Котэбург";
  townNames[13]="Лонгбридж";
  townNames[14]="Нью Грин";
  townNames[15]="Статенборо";
  townNames[16]="Хартфилд";
  townNames[17]="Сандпорт";
  townNames[18]="Болкотт";
  townNames[19]="Брантон";
  townNames[20]="Холсуорт";
  townNames[21]="Стокпорт";
  townNames[22]="Арчвилль";
  townNames[23]="Мейбури";
  townNames[24]="Кэмптаун";
  townNames[25]="Лонкастер";
  townNames[26]="Ромси";
  townNames[27]="Клок Хевен";
  townNames[28]="Чистый залив";
  townNames[29]="Либерти";
  townNames[30]="Солт Спрингс";
  townNames[31]="Эйт Спрингс";
  townNames[32]="Блюкрик";
  townNames[33]="Хагстон";
  townNames[34]="Элсфилд";
  townNames[35]="Кирктаун";
  townNames[36]="Старлинг";
  townNames[37]="Оксбури";
  townNames[38]="Форт Кроу";
  townNames[39]="Джонсборо";
  townNames[40]="Ст. Никлас";
  townNames[41]="Спрингвилль";
  townNames[42]="Стилуотер";
  townNames[43]="Роксдейл";
  townNames[44]="Бларингтон";
  townNames[45]="Честерфилд";
  townNames[46]="Куртвилль";
  townNames[47]="Слейтон";
  townNames[48]="Стамфорд";
  townNames[49]="Таффингтон";
  townNames[50]="Беллхам";
}


void trainSheduler::on_pushButton_3_clicked()
{
  printHomeLoc(ui->scaleSpin->value(), ui->sizeSpin->value());
}

void trainSheduler::getRoads(QJsonObject jObj)
{
  QJsonArray bodyObj = jObj["Body"].toArray();
  myRoads.clear();
  for(int i = 0; i < bodyObj.count(); i++)
  {
    QJsonObject road = bodyObj.at(i).toObject();
    QByteArray bgn, end;
    bgn.clear();
    end.clear();
    bgn.append(road.value("location_id1").toString());
    end.append(road.value("location_id1").toString());
    addRoad(bgn, end);
  }
}

void trainSheduler::addRoad(QByteArray from, QByteArray to)
{
  TRoads road;
  for(int i = 0; i <globalMap.count(); i++)
  {
    TLocation tmp = globalMap.at(i);
    if(tmp.id == from)
    {
      road.loc2 = tmp;
    }
    if(tmp.id == to)
    {
      road.loc1 = tmp;
    }
  }
  myRoads.append(road);

}

void trainSheduler::on_pushButton_4_clicked()
{

}
