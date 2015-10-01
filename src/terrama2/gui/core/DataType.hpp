#include <QString>

struct CommonData
{
  int servicePort_;
  QString address_;
  QString logFile_;
  QString cmd_;
  QString params_;
};

struct Database
{
  QString host_;
  int port_;
  QString user_;
  QString password_;
  QString dbName_;
  QString driver_;
  QString study_;
  QString name_;
  QString version_;
};

struct Collection: public CommonData
{
  int timeout_;
  QString dirPath_;
};
