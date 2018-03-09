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
  \file terrama2/core/utility/Unpack.hpp

  \brief This file contains include headears for the TerraMA2 Unpack file.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_CORE_UTILITY_UNPACK_HPP__
#define __TERRAMA2_CORE_UTILITY_UNPACK_HPP__

// STL
#include <cstdlib>
#include <iostream>
#include <memory>

#include "FileRemover.hpp"

// QT
#include <QFileInfo>

namespace terrama2
{
  namespace core
  {
    /*!
       \brief This class is responsible for decompressing files with  gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension.
    */

    namespace Unpack
    {
      /*!
         * \brief Descompress a file gz, zip, bz2, tar, tar (tar.gz, tar.bz2) extension.
         * \param Absolute file path to the compressed file.
         * \param Returns the path of the temporary folder whith the content of the compressed file.
         */
      TMCOREEXPORT std::string decompress(std::string uri,
                             std::shared_ptr<terrama2::core::FileRemover> remover,
                             const std::string& temporaryFolder = "");

      /*!
         * \brief checks if the file is compressed or not.
         * \param uri - It contains absolute file path.
         * \return Returns true if the file compressed, or false if the file decompressed.
         */
      TMCOREEXPORT bool isCompressed(std::string uri);

      /*!
         * \brief isGzipCompress - Verify is a file with extension gz.
         * \param fileinfo - Compressed file.
         * \return Returns true if the file has the gz extension, or false if it is not with gz extension.
         */
      TMCOREEXPORT bool isGzipCompress(const QFileInfo fileinfo);
      /*!
         * \brief isTarCompress - Verify is a file with extension tar.
         * \param fileinfo - Compressed file.
         * \return Returns true if the file has the tar extesion, or false if it is not with tar extension.
         */
      TMCOREEXPORT bool isTarCompress(const QFileInfo fileinfo);
      /*!
         * \brief isBzipCompress - Verify is file with extension bz2.
         * \param fileinfo - Compressed file.
         * \return Returns true if the file has the bz2 extension, or false if it is not with bz2 extension.
         */
      TMCOREEXPORT bool isBzipCompress(const QFileInfo fileinfo);
      /*!
         * \brief isZipCompress - Verify is file with extension zip.
         * \param fileinfo - Compressed file.
         * \return Returns true if the file has the zip extension, or false if it is not with zip extension.
         */
      TMCOREEXPORT bool isZipCompress(const QFileInfo fileinfo);
      /*!
         * \brief nameFileDecompressed - Name file decompressed.
         * \param fileinfo - Compressed file.
         * \return Returns the name of the decompressed file.
         */
      TMCOREEXPORT QString nameFileDecompressed(const QFileInfo fileinfo);

      /*!
         * \brief parseOct - Parse an octal number, ignoring leading and trailing nonsense.
         * \param p - Octal number.
         * \param n - base Octal.
         * \return Returns decimal number.
        */
      TMCOREEXPORT int parseOct(const char* p, size_t n);
      /*!
         * \brief isEndOfArchive - Verify it is the end of a tar file.
         * \param p - Number of bytes.
         * \return Returns true if this is 512 zero bytes.
         */
      TMCOREEXPORT int isEndOfArchive(const char* p);

      /*!
         * \brief createFile - Create a file, including parent directory as necessary.
         * \param pathname - It contains the file path.
         * \param mode - It contains the building permission mode directory.
         * \param savePath - It contains the path where the file is saved.
         * \param vFiles - vector with the name of the unpack files.
         * \return Returns Create file.
         */
      TMCOREEXPORT FILE* createFile(std::string savePath, int mode);
      /*!
         * \brief verifyChecksum - Verify the tar checksum.
         * \param p - Number of bytes.
         * \return Returns The default signing a tar file.
         */
      TMCOREEXPORT int verifyChecksum(const char* p);

      /*!
         * \brief untar - Extract a tar file.
         * \param path - It contains the path of the compressed file.
         *
        */
      TMCOREEXPORT void untar(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover);
      /*!
         * \brief decompressGz - Decompress a GZ file.
         * \param saveName - It contains the absolute path where the file decompressed is saved.
         * \param fileName - It contains the absolute path of file compressed.
         * \return Returns - The name of the unpack files.
        */
      TMCOREEXPORT QString decompressGz(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover);
      /*!
         * \brief decompressBzip - Decompress a Bzip2 file.
         * \param saveName - It contains the absolute path where the file decompressed is saved.
         * \param fileName - It contains the absolute path of file compressed.
         * \return Returns - The name of the unpack files.
        */
      TMCOREEXPORT QString decompressBzip(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover);
      /*!
         * \brief decompressZip - Decompress a Zip file.
         * \param saveName - It contains the absolute path where the file decompressed is saved.
         * \param fileName - It contains the absolute path of file compressed.
         *
         */
      TMCOREEXPORT void decompressZip(QFileInfo fileInfo, QString temporaryFolder, std::shared_ptr<terrama2::core::FileRemover> remover);
    }
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_UTILITY_UNPACK_HPP__
