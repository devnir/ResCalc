#ifndef SETTINGSADD_H
#define SETTINGSADD_H

#include <QWidget>

typedef struct
{
  QString    name;
  QString    town;
  QByteArray townId;
  QByteArray url;
  QByteArray userAgent;
  QByteArray cookie;
  QByteArray checkSum;
  QByteArray userId;
  QByteArray corpId;
}TServer;

typedef struct
{
  QByteArray Name;
  QByteArray id;
}TForum;

namespace Ui {
  class settingsAdd;
}

class settingsAdd : public QWidget
{
    Q_OBJECT

  public:
    explicit settingsAdd(QWidget *parent = 0);
    ~settingsAdd();

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_pushButton_clicked();

  private:
    Ui::settingsAdd *ui;
};

#endif // SETTINGSADD_H
