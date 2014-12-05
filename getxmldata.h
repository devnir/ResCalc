#ifndef GETXMLDATA_H
#define GETXMLDATA_H
#include <QFile>
#include <QDomDocument>
#include <QString>
extern  QString getFactoryName(int factoryId, int cityId, int factoryDirection);
extern  QString getCityName(int id);
extern  QString getFactoryDirection(int direction);

#endif // GETXMLDATA_H
