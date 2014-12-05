#include "getxmldata.h"
#include <QDebug>
QString getFactoryName(int factoryId, int cityId, int factoryDirection)
{
  QDomDocument doc("mydocument");
  QFile *file = new QFile("languagedata.xml");
  QString factoryName = "-----";
  QString fId;
  fId.sprintf("IDS_FACT_%03d", factoryId);

  if (!file->open(QIODevice::ReadOnly))
    return " ";
  if (!doc.setContent(file))
  {
    file->close();
    return " ";
  }

  QDomNodeList list = doc.elementsByTagName("tu");
  for(int i = 0; i < list.count(); i++)
  {
    QDomNode tu = list.at(i);
    if(tu.hasAttributes() && tu.hasChildNodes())
    {
      QString attrib = tu.attributes().namedItem("tuid").nodeValue();
      if(attrib == fId)
      {
        QDomNodeList child = tu.childNodes();
        factoryName = child.at(2).toElement().text();
        file->close();
        factoryName = factoryName + " " + getCityName(cityId) + " " + getFactoryDirection(factoryDirection);
        return factoryName;
      }
    }
  }
  file->close();
  return factoryName;
}


QString getCityName(int id)
{
  QString townNames[64];
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
    return townNames[id];
}

QString getFactoryDirection(int direction)
{
  QString direct;
  QDomDocument doc("factDirDoc");
  QFile *file = new QFile("languagedata.xml");
  QString fId;
  fId.sprintf("IDS_FACTORY_NAME_DIR_%d", direction);

  if (!file->open(QIODevice::ReadOnly))
    return " ";
  if (!doc.setContent(file))
  {
    file->close();
    return " ";
  }

  QDomNodeList list = doc.elementsByTagName("tu");
  for(int i = 0; i < list.count(); i++)
  {
    QDomNode tu = list.at(i);
    if(tu.hasAttributes() && tu.hasChildNodes())
    {
      QString attrib = tu.attributes().namedItem("tuid").nodeValue();
      if(attrib == fId)
      {
        qDebug() << "Bingo";
        QDomNodeList child = tu.childNodes();
        direct = child.at(2).toElement().text();
        file->close();
        return direct;
      }
    }
  }
  file->close();
  return direct;
}
