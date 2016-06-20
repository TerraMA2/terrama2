
// TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/Unpack.hpp>

// STL
#include <iostream>

// QT
#include <QUrl>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStringList>


int main(int argc, char* argv[])
{
  std::string uri;
  std::string path;
  uri = "file://";
  uri += TERRAMA2_DATA_DIR;
  uri += "/PCD_serrmar_INPE/";
  uri += "SetoresCubatao_UTM_sad69.zip";
  //uri += "30885.txt";

  if(!terrama2::core::Unpack::verifyCompressFile(uri))
  {
    qDebug() << "File not compressed!";
  }
  else
  {
    path = terrama2::core::Unpack::unpackList(uri);
    qDebug() << "File descompressed!";

    QUrl uriLocal(path.c_str());
    QDir dir(uriLocal.path());

    dir.setNameFilters(QStringList()<<"*.*");
    QStringList fileList = dir.entryList();

    if (fileList.empty())
      qDebug() << "Test failed!";
    else
    {
      for (QString nameFile: fileList)
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
