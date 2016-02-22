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
  \file terrama2/collector/UnpackGZ.cpp

  \brief This class is responsible for unpacking files with .gz extension.

  \author Evandro Delatin
*/


// TerraMA2
#include "UnpackGZ.hpp"
#include "../core/Logger.hpp"
#include "Exception.hpp"

// STL
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <zlib.h>
#include <zconf.h>

// QT
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QLocale>


void terrama2::collector::UnpackGZ::unpackList(std::vector<TransferenceData>& transferenceDataVec)
{
  if(transferenceDataVec.empty())
    return;

  std::string compressed, uncompressed;
  std::vector<terrama2::collector::TransferenceData> newTransferenceDataVec;

  try
  {
    for(TransferenceData& transferenceData : transferenceDataVec)
    {
      QUrl uri(transferenceData.uriTemporary.c_str());
      QFileInfo fileName(uri.path());
      QString saveName = fileName.absoluteFilePath();

      if (isGzipCompress(fileName))
      {
        if (!loadBinaryFile(fileName.absoluteFilePath().toStdString(),compressed))
        {
          QString errMsg = QObject::tr("Error loading input file. \n\n");
          TERRAMA2_LOG_ERROR() << errMsg;
          throw UnpackGZException() << ErrorDescription(errMsg);
        }
        else
        {
          QString errMsg = QObject::tr("Loaded, starting decompressing. \n\n");
          TERRAMA2_LOG_DEBUG() << errMsg;

          if (!gzipInflate(compressed,uncompressed))
          {
            QString errMsg = QObject::tr("Error descompressing file. \n\n");
            TERRAMA2_LOG_ERROR() << errMsg;
          }
          else
          {
            QString errMsg = QObject::tr("Complete decompressing, writing to disk. \n\n");
            TERRAMA2_LOG_DEBUG() << errMsg;

            saveName = nameGzipUncompressed(fileName);

            if(!createBinaryFile(saveName.toStdString(), uncompressed))
            {
              QString errMsg = QObject::tr("Error creating file. \n\n");
              TERRAMA2_LOG_ERROR() << errMsg;
              throw UnpackGZException() << ErrorDescription(errMsg);
            }
            else
            {
              QString errMsg = QObject::tr("Complete writing. \n\n");
              TERRAMA2_LOG_DEBUG() << errMsg;
            }
          }
        }
      }
      terrama2::collector::TransferenceData transferenceDataTemp;
      transferenceDataTemp.uriTemporary = saveName.toStdString().c_str();
      transferenceDataTemp.uriOrigin = transferenceData.uriOrigin;

      newTransferenceDataVec.push_back(transferenceDataTemp);
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

bool terrama2::collector::UnpackGZ::isGzipCompress(const QFileInfo fileinfo)
{
  return fileinfo.absoluteFilePath().endsWith("gz");
}

// Reads a file into memory
bool terrama2::collector::UnpackGZ::loadBinaryFile(const std::string &fullPath, std::string& contents )
{
  //Open the gzip file in binary mode
  FILE* f = fopen( fullPath.c_str(), "rb" );

  //Clear existing bytes in output vector
  contents.clear();

  //Read all the bytes in the file
  int c = fgetc( f );
  while ( c != EOF )
  {
    contents +=  (char) c ;
    c = fgetc( f );
  }

  fclose (f);

  return true ;
}

bool terrama2::collector::UnpackGZ::gzipInflate( const std::string& compressedBytes, std::string& uncompressedBytes )
{
  // Empty File
  if ( compressedBytes.size() == 0 )
  {
    uncompressedBytes = compressedBytes ;
    return true ;
  }

  uncompressedBytes.clear();

  unsigned full_length = compressedBytes.size() ;
  unsigned half_length = compressedBytes.size() / 2;

  unsigned uncompLength = full_length ;
  char* uncomp = (char*) calloc( sizeof(char), uncompLength );

  z_stream strm;
  strm.next_in = (Bytef *) compressedBytes.c_str();
  strm.avail_in = compressedBytes.size() ;
  strm.total_out = 0;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;

  bool done = false ;

  if (inflateInit2(&strm, (16+MAX_WBITS)) != Z_OK)
  {
    free( uncomp );
    return false;
  }

  while (!done)
  {
    // If our output buffer is too small
    if (strm.total_out >= uncompLength )
    {
      // Increase size of output buffer
      char* uncomp2 = (char*) calloc( sizeof(char), uncompLength + half_length );
      memcpy( uncomp2, uncomp, uncompLength );
      uncompLength += half_length ;
      free( uncomp );
      uncomp = uncomp2 ;
    }

    strm.next_out = (Bytef *) (uncomp + strm.total_out);
    strm.avail_out = uncompLength - strm.total_out;

    // Inflate another chunk.
    int err = inflate (&strm, Z_SYNC_FLUSH);
    if (err == Z_STREAM_END) done = true;
    else if (err != Z_OK)
    {
      break;
    }
  }

  if (inflateEnd (&strm) != Z_OK)
  {
    free( uncomp );
    return false;
  }

  for ( size_t i=0; i<strm.total_out; ++i )
  {
    uncompressedBytes += uncomp[ i ];
  }
  free( uncomp );
  return true ;
}


QString terrama2::collector::UnpackGZ::nameGzipUncompressed(const QFileInfo fileinfo)
{
  if (isGzipCompress(fileinfo))
  {
    return fileinfo.absolutePath() + "/" + fileinfo.completeBaseName();
  }
  else
  {
    return fileinfo.absoluteFilePath();
  }
}

bool terrama2::collector::UnpackGZ::createBinaryFile( const std::string& fullPath, std::string& contents )
{
  //Open the gzip file in binary mode
  FILE* f = fopen( fullPath.c_str(), "w+b" );

  fwrite (contents.c_str() , sizeof(char), contents.size(), f);

  if ( f == NULL )
    return false;

  fclose (f);

  return true;
}
