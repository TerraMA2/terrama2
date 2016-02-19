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
  \file terrama2/collector/UnpackGZ.hpp

  \brief This file contains include headears for the TerraMA2 Unpack GZ.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_COLLECTOR_UNPACKGZ_HPP__
#define __TERRAMA2_COLLECTOR_UNPACKGZ_HPP__

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
  /*!
     \brief This class is responsible for decompressing files with .gz extension
            and addition of uncompressed files in tranferenceDataVec Vector.
 */
  namespace collector
  {
    class UnpackGZ : public boost::noncopyable
    {
    public:

        /*!
       * \brief unpackList - update of transferenceDataVec after the descompressing a file.
       *
       * \param TransferenceDataVec is struct holds the collection data of a core::DataSetItem.
       */
      void unpackList(std::vector<TransferenceData>& transferenceDataVec);

      /*!
       * \brief isGzipCompress - verify is a file with extension gz.
       * \param fileinfo - compressed file.
       * \return Returns true if the file has the gz extension, or false if it is not with gz extension.
       */
      static bool isGzipCompress(const QFileInfo fileinfo);

      /*!
       * \brief loadBinaryFile - reads a file into memory.
       * \param filename Absolute file path.
       * \param contents Compressed binary file.
       * \return Returns true for success and false for failure.
       */
      static bool loadBinaryFile(const std::string& filename, std::string& contents);

      /*!
       * \brief gzipInflate - uncompressed bytes of file.
       * \param compressedBytes Binary archive.
       * \param uncompressedBytes Binary uncompressed file.
       * \return Returns true for success and false for failure.
       */
      static bool gzipInflate( const std::string& compressedBytes, std::string& uncompressedBytes);

      /*!
       * \brief nameGzipUncompressed - Name file uncompressed.
       * \param fileinfo Compressed file.
       * \return Returns the name of the uncompressed file.
       */
      static QString nameGzipUncompressed(const QFileInfo fileinfo);

      /*!
       * \brief createBinaryFile - create uncompressed file.
       * \param filename Name that will be the uncompressed file.
       * \param contents Binary file uncompressed.
       * \return Returns true for success and false for failure.
       */
      static bool createBinaryFile( const std::string& filename, std::string& contents );
    };
  } // end namespace collector
}   // end namespace terrama2

#endif  // __TERRAMA2_COLLECTOR_UNPACK_HPP__
