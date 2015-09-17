#include <QString>

struct CommonData
{
  int servicePort;
  QString address_;
  QString logFile_;
  QString cmd_;
  QString params_;
};

enum DatabaseDriver
{
  DRIVER_PostGIS,
  DRIVER_PostgreSQL,
  DRIVER_MySQL
};

struct Database
{
  QString host_;
  int port_;
  QString user_;
  QString password_;
  QString dbName_;
  DatabaseDriver driver_;
  bool study_;
};

struct Collection: public CommonData
{
  int timeout_;
  QString dirPath_;
};
