/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/core/utility/Unpack.cpp

  \brief This class is responsible for unpacking files with gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension.

  This code was copied from: https://github.com/libarchive/libarchive/blob/master/contrib/untar.c
  has to be reviewed and credited correctly

  \author Evandro Delatin
*/


#include <JlCompress.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/operations.hpp>

#include <terralib/Exception.h>
// STL
#include <fstream>
#include <iostream>
#include <string>
// Qt
#include <QString>

#include "../../Exception.hpp"
#include "FileRemover.hpp"
#include "Logger.hpp"
#include "Raii.hpp"
#include "Utils.hpp"
// TerraMA2
#include "Unpack.hpp"

QString terrama2::core::Unpack::decompressGz(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover)
{
  QString saveName = temporaryFolder+"/"+nameFileDecompressed(fileInfo);
  remover->addTemporaryFile(saveName.toStdString());

  std::ifstream inFile(fileInfo.absoluteFilePath().toStdString(), std::ios_base::in);
  std::ofstream outFile(saveName.toStdString(), std::ios_base::out);
  boost::iostreams::filtering_streambuf< boost::iostreams::input> inStreamBuff;
  inStreamBuff.push(boost::iostreams::gzip_decompressor());
  inStreamBuff.push(inFile);
  boost::iostreams::copy(inStreamBuff, outFile);

  return saveName;
}

QString terrama2::core::Unpack::decompressBzip(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover)
{
  QString saveName = temporaryFolder+"/"+nameFileDecompressed(fileInfo);
  remover->addTemporaryFile(saveName.toStdString());

  std::ifstream inFile(fileInfo.absoluteFilePath().toStdString(), std::ios_base::in);
  std::ofstream outFile(saveName.toStdString(), std::ios_base::out);
  boost::iostreams::filtering_streambuf< boost::iostreams::input> inStreamBuff;
  inStreamBuff.push(boost::iostreams::bzip2_decompressor());
  inStreamBuff.push(inFile);
  boost::iostreams::copy(inStreamBuff, outFile);

  return saveName;
}

void terrama2::core::Unpack::decompressZip(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> /*remover*/)
{
  JlCompress::extractDir(fileInfo.absoluteFilePath(), temporaryFolder);
}

bool terrama2::core::Unpack::isCompressed(std::string uri)
{
  QUrl url(uri.c_str());
  QFileInfo fileInfo(url.path());
  QString saveName = fileInfo.absoluteFilePath();

  if((isGzipCompress(saveName)) || (isBzipCompress(saveName)) || (isZipCompress(saveName)))
    return true;
  else return false;

}

std::string terrama2::core::Unpack::decompress(std::string uri,
                                               std::shared_ptr<terrama2::core::FileRemover> remover,
                                               const std::string& temporaryFolder)
{
  QString unpackFolder = QString::fromStdString(getTemporaryFolder(remover, temporaryFolder));
  // Its important to remove scheme from unpack folder due
  // boost::iostreams does not seem to work with.
  QUrl wrapUnpackFolder(unpackFolder);
  unpackFolder = wrapUnpackFolder.toString(QUrl::RemoveScheme);

  try
  {
    QUrl url(uri.c_str());
    QFileInfo fileInfo(url.path());
    QString filePath = fileInfo.absoluteFilePath();

    if(isGzipCompress(filePath))
    {
      filePath = decompressGz(fileInfo, unpackFolder, remover);
      fileInfo.setFile(filePath);
    }

    if(isBzipCompress(filePath))
    {
      filePath = decompressBzip(fileInfo, unpackFolder, remover);
      fileInfo.setFile(filePath);
    }

    if(isZipCompress(filePath))
    {
      decompressZip(fileInfo, unpackFolder, remover);
    }

    if(isTarCompress(filePath))
    {
      untar(fileInfo, unpackFolder, remover);
    }

  }
  catch(const terrama2::Exception&)
  {
    //logged on throw
  }
  catch(const te::Exception& e)
  {
    QString errMsg;
    auto errStr = boost::get_error_info<te::ErrorDescription>(e);
    if(errStr)
      errMsg.append(QString::fromStdString(*errStr));
    errMsg.append(e.what());
    TERRAMA2_LOG_ERROR() << errMsg;
  }
  catch(const std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }

  return unpackFolder.toStdString();
}

bool terrama2::core::Unpack::isGzipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("gz");
}

bool terrama2::core::Unpack::isTarCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("tar");
}

bool terrama2::core::Unpack::isBzipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("bz2");
}

bool terrama2::core::Unpack::isZipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("zip");
}

QString terrama2::core::Unpack::nameFileDecompressed(const QFileInfo fileinfo)
{
  if(isGzipCompress(fileinfo) || isBzipCompress(fileinfo))
  {
    return fileinfo.completeBaseName();
  }
  else
  {
    return fileinfo.absoluteFilePath();
  }
}

// Parse an octal number, ignoring leading and trailing nonsense.
int terrama2::core::Unpack::parseOct(const char* p, size_t n)
{
  int i = 0;

  while(*p < '0' || *p > '7')
  {
    ++p;
    --n;
  }

  while(*p >= '0' && *p <= '7' && n > 0)
  {
    i *= 8;
    i += *p - '0';
    ++p;
    --n;
  }

  return i;
}

// Returns true if this is 512 zero bytes.
int terrama2::core::Unpack::isEndOfArchive(const char* p)
{
  int n;
  for(n = 511; n >= 0; --n)
    if(p[n] != '\0')
      return 0;
  return 1;
}

//! Create a file, including parent directory as necessary.
// unused parameter from the original code, commented to silence warning
// obs: original file link at the beggining of the file
std::FILE* terrama2::core::Unpack::createFile(std::string savePath, int /*mode*/)
{
  std::FILE* newFile = std::fopen(savePath.c_str(), "w+");

  if(newFile == nullptr)
  {
    auto pos = savePath.find_last_of('/');
    if(pos != std::string::npos)
    {
      auto folder = savePath.substr(0, savePath.size()-pos);
      QDir dir(QString::fromStdString(folder));
      dir.mkdir(".");
      newFile = std::fopen(savePath.c_str(), "w+");
    }
  }

  return newFile;
}

// Verify the tar checksum.
int terrama2::core::Unpack::verifyChecksum(const char* p)
{
  int n, u = 0;

  for(n = 0; n < 512; ++n)
  {
    if(n < 148 || n > 155)
      // Standard tar checksum adds unsigned bytes.
      u += ((unsigned char*)p)[n];
    else
      u += 0x20;
  }

  return (u == parseOct(p + 148, 8));
}

// Extract a tar archive.
void terrama2::core::Unpack::untar(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover)
{
  std::string path = fileInfo.absoluteFilePath().toStdString();
  char buff[512];
  std::FILE* fileDecompressed = nullptr;

  FilePtr fileCompressed(path.c_str(),"r");
  std::string savePath = temporaryFolder.toStdString();

  for(;;)
  {
    size_t bytesRead = std::fread(buff, 1, 512, fileCompressed.file());

    if(bytesRead < 512)
    {
      QString errMsg = QObject::tr("Short read on %1: Expected 512, got %2\n").arg(path.c_str()).arg(bytesRead);
      TERRAMA2_LOG_DEBUG() << errMsg;
      return;
    }

    if(isEndOfArchive(buff))
    {
      QString errMsg = QObject::tr("End of %1").arg(path.c_str());
      TERRAMA2_LOG_DEBUG() << errMsg;
      return;
    }

    if(!verifyChecksum(buff))
    {
      QString errMsg = QObject::tr("Checksum failure.\n");
      TERRAMA2_LOG_ERROR() << errMsg;
      return;
    }

    int fileSize = parseOct(buff + 124, 12);

    switch(buff[156])
    {
      case '1':
      {
        QString errMsg = QObject::tr("Ignoring hardlink %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        break;
      }
      case '2':
      {
        QString errMsg = QObject::tr("Ignoring symlink %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        break;
      }
      case '3':
      {
        QString errMsg = QObject::tr("Ignoring character device %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        break;
      }
      case '4':
      {
        QString errMsg = QObject::tr("Ignoring block device %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        break;
      }
      case '5':
      {
        QString errMsg = QObject::tr("Extracting dir %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        fileSize = 0;
        break;
      }
      case '6':
      {
        QString errMsg = QObject::tr("Ignoring FIFO %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        break;
      }
      default:
      {
        QString errMsg = QObject::tr("Extracting file %1").arg(buff);
        TERRAMA2_LOG_DEBUG() << errMsg;
        std::string filePath = savePath+"/"+buff;
        remover->addTemporaryFile(filePath);
        fileDecompressed = createFile(filePath, parseOct(buff + 100, 8));
        break;
      }
    }

    while(fileSize > 0)
    {
      bytesRead = std::fread(buff, 1, 512, fileCompressed.file());
      if(bytesRead < 512)
      {
        QString errMsg = QObject::tr("Short read on %1: Expected 512, got %2\n").arg(path.c_str()).arg(bytesRead);
        TERRAMA2_LOG_DEBUG() << errMsg;
        return;
      }

      if(fileSize < 512)
        bytesRead = fileSize;

      if(fileDecompressed != nullptr)
      {
        if(std::fwrite(buff, 1, bytesRead, fileDecompressed) != bytesRead)
        {
          QString errMsg = QObject::tr("Failed write.\n");
          TERRAMA2_LOG_ERROR() << errMsg;
          std::fclose(fileDecompressed);
          fileDecompressed = nullptr;
        }
      }
      fileSize -= bytesRead;
    }

    if(fileDecompressed != nullptr)
    {
      std::fclose(fileDecompressed);
      fileDecompressed = nullptr;
    }
  }
}
