
// TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/Unpack.hpp>
#include <terrama2/Config.hpp>

// STL
#include <iostream>

// QT
#include <QUrl>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStringList>


int main()
{
  std::string uri;
  std::string path;
  uri = "file://";
  uri += TERRAMA2_DATA_DIR;
  uri += "/PCD_serrmar_INPE/";
  uri += "SetoresCubatao_UTM_sad69.zip";

  if(!terrama2::core::Unpack::isCompressed(uri))
  {
    qDebug() << "File not compressed!";
  }
  else
  {
    auto remover = std::make_shared<terrama2::core::FileRemover>();
    path = terrama2::core::Unpack::decompress(uri, remover);
    qDebug() << "File descompressed!";

    QUrl uriLocal(path.c_str());
    QDir dir(uriLocal.path());

    dir.setNameFilters(QStringList()<<"*.*");
    QStringList fileList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    if (fileList.empty())
      qDebug() << "Test failed!";
    else
    {
      for (const QString& nameFile: fileList)
      {
        qDebug() << "Extracting: " << nameFile;
      }

      // Remove paste of download files.
      if (dir.exists())
        dir.removeRecursively();
    }
  }

  return 0;
}
