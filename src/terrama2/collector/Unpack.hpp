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
  \file terrama2/collector/Unpack.hpp

  \brief This file contains include headears for the TerraMA2 Unpack file.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_COLLECTOR_UNPACK_HPP__
#define __TERRAMA2_COLLECTOR_UNPACK_HPP__

// STL
#include <cstdlib>
#include <iostream>

//Boost
#include <boost/noncopyable.hpp>

// QT
#include <QFileInfo>

//TerraMA2
#include "TransferenceData.hpp"

namespace terrama2
{
  namespace collector
  {
    /*!
       \brief This class is responsible for decompressing files with  gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension
              and addition of uncompressed files in tranferenceDataVec Vector.
   */

    class Unpack : public boost::noncopyable
    {
    public:

        /*!
       * \brief unpackList - update of transferenceDataVec after the descompressing
       *        a file gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension.
       *
       * \param TransferenceDataVec is struct holds the collection data of a core::DataSetItem.
       */
      void unpackList(std::vector<TransferenceData>& transferenceDataVec);

      /*!
       * \brief isGzipCompress - Verify is a file with extension gz.
       * \param fileinfo - Compressed file.
       * \return Returns - True if the file has the gz extension, or false if it is not with gz extension.
       */
      static bool isGzipCompress(const QFileInfo fileinfo);

      /*!
       * \brief isTarCompress - Verify is a file with extension tar.
       * \param fileinfo - Compressed file.
       * \return Returns - True if the file has the tar extesion, or false if it is not with tar extension.
       */
      static bool isTarCompress(const QFileInfo fileinfo);

      /*!
       * \brief isBzipCompress - Verify is file with extension bz2.
       * \param fileinfo - Compressed file.
       * \return Returns True if the file has the bz2 extension, or false if it is not with bz2 extension.
       */
      static bool isBzipCompress(const QFileInfo fileinfo);

      /*!
       * \brief isZipCompress - Verify is file with extension zip.
       * \param fileinfo - Compressed file.
       * \return Returns - True if the file has the zip extension, or false if it is not with zip extension.
       */
      static bool isZipCompress(const QFileInfo fileinfo);

      /*!
       * \brief nameFileUncompressed - Name file uncompressed.
       * \param fileinfo - Compressed file.
       * \return Returns - The name of the uncompressed file.
       */
      static QString nameFileUncompressed(const QFileInfo fileinfo);

      /*!
       * \brief parseoct - Parse an octal number, ignoring leading and trailing nonsense.
       * \param p - Octal number.
       * \param n - base Octal.
       * \return Returns - Decimal number.
       */
      static int parseoct(const char *p, size_t n);

      /*!
       * \brief is_end_of_archive - Verify it is the end of a tar file.
       * \param p - Number of bytes.
       * \return Returns - True if this is 512 zero bytes.
       */
      static int is_end_of_archive(const char *p);

      /*!
       * \brief create_dir - Create a directory, including parent directories as necessary.
       * \param pathname - It contains the file path.
       * \param mode - It contains the building permission mode directory.
       */
      static void create_dir(char *pathname, int mode);

      /*!
       * \brief create_file - Create a file, including parent directory as necessary.
       * \param pathname - It contains the file path.
       * \param mode - It contains the building permission mode directory.
       * \param savePath - It contains the path where the file is saved.
       * \param vFiles - vector with the name of the unpack files.
       * \return - Returns Create file.
       */
      static FILE* create_file(char *pathname, int mode, std::string savePath, std::vector<std::string> &vFiles);

      /*!
       * \brief verify_checksum - Verify the tar checksum.
       * \param p - Number of bytes.
       * \return Returns - The default signing a tar file.
       */
      static int verify_checksum(const char *p);

      /*!
       * \brief untar - Extract a tar file.
       * \param path - It contains the path of the compressed file.
       * \return Returns - Vector with the name of the unpack files.
       */
      static std::vector<std::string> untar(const std::string& path);

      /*!
       * \brief uncompressGz - Uncompress a GZ file.
       * \param saveName - It contains the absolute path where the file uncompressed is saved.
       * \param fileName - It contains the absolute path of file compressed.
       * \return Returns - vector with the name of the unpack files.
       */
      QString uncompressGz(QString saveName, QFileInfo fileName);

      /*!
       * \brief uncompressBzip - Uncompress a Bzip2 file.
       * \param saveName - It contains the absolute path where the file uncompressed is saved.
       * \param fileName - It contains the absolute path of file compressed.
       * \return Returns - The absolute path where the file uncompressed is saved.
       */
      QString uncompressBzip(QString saveName, QFileInfo fileName);

      /*!
       * \brief uncompressZip - Uncompress a Zip file.
       * \param saveName - It contains the absolute path where the file uncompressed is saved.
       * \param fileName - It contains the absolute path of file compressed.
       * \return Returns - The absolute path where the file uncompressed is saved.
       */
      std::vector<std::string> uncompressZip(QString saveName, QFileInfo fileName);
    };
  } // end namespace collector
}   // end namespace terrama2

#endif  // __TERRAMA2_COLLECTOR_UNPACK_HPP__
