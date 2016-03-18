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
  \file terrama2/collector/Unpack.cpp

  \brief This class is responsible for unpacking files with gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension.

  \author Evandro Delatin
*/


// TerraMA2
#include "Unpack.hpp"
#include "../core/Logger.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include "FileOpener.hpp"

// STL
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <zlib.h>
#include <zconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <sys/stat.h>

// QT
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QLocale>
#include <QFile>
#include <QByteArray>
#include <quazip/quazip.h>
#include <quazip/JlCompress.h>

// Boost
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>


QString terrama2::collector::Unpack::uncompressGz(QString saveName, QFileInfo fileName)
{
  saveName = nameFileUncompressed(fileName);

  std::ifstream inFile(fileName.absoluteFilePath().toStdString(), std::ios_base::in);
  std::ofstream outFile(saveName.toStdString(), std::ios_base::out);
  boost::iostreams::filtering_streambuf< boost::iostreams::input> inStreamBuff;
  inStreamBuff.push( boost::iostreams::gzip_decompressor());
  inStreamBuff.push( inFile );
  boost::iostreams::copy(inStreamBuff, outFile);

  return saveName;
}

QString terrama2::collector::Unpack::uncompressBzip(QString saveName, QFileInfo fileName)
{
  saveName = nameFileUncompressed(fileName);

  std::ifstream inFile(fileName.absoluteFilePath().toStdString(), std::ios_base::in);
  std::ofstream outFile(saveName.toStdString(), std::ios_base::out);
  boost::iostreams::filtering_streambuf< boost::iostreams::input> inStreamBuff;
  inStreamBuff.push(boost::iostreams::bzip2_decompressor());
  inStreamBuff.push(inFile);
  boost::iostreams::copy(inStreamBuff, outFile);

  return saveName;
}

std::vector<std::string> terrama2::collector::Unpack::uncompressZip(QString saveName, QFileInfo fileName)
{
  std::vector<std::string> vectorFiles;
  std::size_t found = saveName.toStdString().find_last_of("/\\");
  std::string savePath = saveName.toStdString().substr(0,found);
  savePath.append("/");

  QStringList listFiles = JlCompress::getFileList(fileName.absoluteFilePath().toStdString().c_str());
  JlCompress::extractDir(fileName.absoluteFilePath().toStdString().c_str(), savePath.c_str());

  for(QString nameFile: listFiles)
  {
    QString saveFilePath = savePath.c_str()+nameFile;
    vectorFiles.push_back(saveFilePath.toStdString().c_str());
  }

  return vectorFiles;
}

void terrama2::collector::Unpack::unpackList(std::vector<TransferenceData>& transferenceDataVec)
{
  if (transferenceDataVec.empty())
    return;

  std::vector<terrama2::collector::TransferenceData> newTransferenceDataVec;

  try
  {
    for(TransferenceData& transferenceData : transferenceDataVec)
    {
      QUrl uri(transferenceData.uriTemporary.c_str());
      QFileInfo fileName(uri.path());
      QString saveName = fileName.absoluteFilePath();
      std::vector<std::string> vectorFiles;

      if (isGzipCompress(saveName))
      {
        saveName = uncompressGz(saveName, fileName);
      }

      if (isBzipCompress(saveName))
      {
        saveName = uncompressBzip(saveName, fileName);
      }

      if (isZipCompress(saveName))
      {
        vectorFiles = uncompressZip(saveName, fileName);
      }

      else if (isTarCompress(saveName))
      {
        vectorFiles = untar(saveName.toStdString());
      }
      else // Only a compressed file.
      {
        vectorFiles.push_back(saveName.toStdString().c_str());
      }
    }

    transferenceDataVec = newTransferenceDataVec;

  }
  catch(terrama2::Exception&)
  {
    //logged on throw
  }
  catch(te::common::Exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
  catch(std::exception& e)
  {
    TERRAMA2_LOG_ERROR() << e.what();
  }
}

bool terrama2::collector::Unpack::isGzipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("gz");
}

bool terrama2::collector::Unpack::isTarCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("tar");
}

bool terrama2::collector::Unpack::isBzipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("bz2");
}

bool terrama2::collector::Unpack::isZipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("zip");
}

QString terrama2::collector::Unpack::nameFileUncompressed(const QFileInfo fileinfo)
{
  if (isGzipCompress(fileinfo) || isBzipCompress(fileinfo))
  {
    return fileinfo.absolutePath() + "/" + fileinfo.completeBaseName();
  }
  else
  {
    return fileinfo.absoluteFilePath();
  }
}

// Parse an octal number, ignoring leading and trailing nonsense.
int terrama2::collector::Unpack::parseoct(const char *p, size_t n)
{
  int i = 0;

  while (*p < '0' || *p > '7')
  {
    ++p;
    --n;
  }

  while (*p >= '0' && *p <= '7' && n > 0)
  {
    i *= 8;
    i += *p - '0';
    ++p;
    --n;
  }

  return i;
}

// Returns true if this is 512 zero bytes.
int terrama2::collector::Unpack::is_end_of_archive(const char *p)
{
  int n;
  for (n = 511; n >= 0; --n)
    if (p[n] != '\0')
      return 0;
  return 1;
}

// Create a directory, including parent directories as necessary.
void terrama2::collector::Unpack::create_dir(char *pathname, int mode)
{
  char *p;
  int r;

  // Strip trailing '/'
  if (pathname[strlen(pathname) - 1] == '/')
    pathname[strlen(pathname) - 1] = '\0';

  // Try creating the directory.
  r = mkdir(pathname, mode); // pathname

  if (r != 0)
  {
    // On failure, try creating parent directory.
    p = strrchr(pathname, '/'); // pathname
    if (p != nullptr)
    {
      *p = '\0';
      create_dir(pathname, 0755); //pathname
      *p = '/';
      r = mkdir(pathname, mode); // pathname
    }
  }
  if (r != 0)
    fprintf(stderr, "Could not create directory %s\n", pathname);
}


// Create a file, including parent directory as necessary.
std::FILE* terrama2::collector::Unpack::create_file(char *pathname, int mode, std::string savePath, std::vector<std::string> &vFiles)
{
  std::FILE *newFile;

  char *absolutePath = const_cast <char*> ((savePath+pathname).c_str());

  newFile = std::fopen((savePath+pathname).c_str(), "w+");

  if (newFile == nullptr)
  {
    // Try creating parent dir and then creating file.
    char *p = strrchr(absolutePath, '/');
    if (p != nullptr)
    {
      *p = '\0';
      create_dir(absolutePath, 0755);
      *p = '/';
      newFile = std::fopen((savePath+pathname).c_str(), "w+");
    }
  }
  vFiles.push_back(absolutePath);
  return newFile;
}

// Verify the tar checksum.
int terrama2::collector::Unpack::verify_checksum(const char *p)
{
  int n, u = 0;

  for (n = 0; n < 512; ++n)
  {
    if (n < 148 || n > 155)
      // Standard tar checksum adds unsigned bytes.
      u += ((unsigned char *)p)[n];
    else
      u += 0x20;
  }

  return (u == parseoct(p + 148, 8));
}

// Extract a tar archive.
std::vector<std::string> terrama2::collector::Unpack::untar(const std::string& path)
{
  char buff[512];
  std::FILE *fileUncompressed = nullptr;
  size_t bytes_read;
  int filesize;
  std::vector<std::string> vectorFiles;

  FileOpener fileCompressed(path.c_str(),"r");

  std::size_t found = path.find_last_of("/\\");
  std::string savePath = path.substr(0,found);
  savePath.append("/");

  for (;;)
  {
    bytes_read = std::fread(buff, 1, 512, fileCompressed.file());

    if (bytes_read < 512)
    {
      std::fprintf(stderr,"Short read on %s: expected 512, got %d\n",path.c_str(), bytes_read);
      return vectorFiles;
    }

    if (is_end_of_archive(buff))
    {
      QString errMsg = QObject::tr("End of %1").arg(path.c_str());
      TERRAMA2_LOG_DEBUG() << errMsg;

      return vectorFiles;
    }

    if (!verify_checksum(buff))
    {
      std::fprintf(stderr, "Checksum failure\n");
      return vectorFiles;
    }

    filesize = parseoct(buff + 124, 12);

    switch (buff[156])
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
        filesize = 0;
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
        fileUncompressed = create_file(buff, parseoct(buff + 100, 8), savePath, vectorFiles);
        break;
      }
    }

    while (filesize > 0)
    {
      bytes_read = std::fread(buff, 1, 512, fileCompressed.file());
      if (bytes_read < 512)
      {
        std::fprintf(stderr,"Short read on %s: Expected 512, got %d\n",path.c_str(), bytes_read);
        return vectorFiles;
      }

      if (filesize < 512)
        bytes_read = filesize;

      if (fileUncompressed != nullptr)
      {
        if (std::fwrite(buff, 1, bytes_read, fileUncompressed) != bytes_read)
        {
          std::fprintf(stderr, "Failed write\n");
          std::fclose(fileUncompressed);
          fileUncompressed = nullptr;
        }
      }
      filesize -= bytes_read;
    }

    if (fileUncompressed != nullptr)
    {
      std::fclose(fileUncompressed);
      fileUncompressed = nullptr;
    }
  }
  return vectorFiles;
}

