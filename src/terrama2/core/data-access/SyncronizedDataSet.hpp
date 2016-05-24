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
  \file terrama2/core/data-access/SyncronizedDataSet.hpp

  \brief Thread-safe dataset access.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_CORE_SYNCRONIZED_DATASET_HPP__
#define __TERRAMA2_CORE_SYNCRONIZED_DATASET_HPP__

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>

// STL
#include <memory>
#include <mutex>

// Forward declaration
namespace te
{
  namespace rst
  {
    class Raster;
  }
}

namespace terrama2
{
  namespace core
  {

    /*!
      \class SyncronizedDataSet
      \brief Provide syncronized access to a terralib dataset.

     */
    class SyncronizedDataSet
    {
      public:
        /*!
          \brief Constructor.
          \param dataset A smart point to the TerraLib dataset that will have the access syncronized.
        */
        SyncronizedDataSet(std::shared_ptr<te::da::DataSet> dataset);

        /*! \brief Virtual destructor. */
        virtual ~SyncronizedDataSet();

        /*!
          \brief Method for retrieving a geometric attribute value.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The geometric attribute value in the given position.
        */
        std::shared_ptr<te::gm::Geometry> getGeometry(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a string value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The string attribute value in the given position.
        */
        std::string getString(std::size_t row, std::size_t columnIndex) const;


        /*!
          \brief Method for retrieving a numeric value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The numeric attribute value in the given position.
        */
        std::string getNumeric(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a double value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The double attribute value in the given position.
        */
        double getDouble(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a bool value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The bool attribute value in the given position.
        */
        bool getBool(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a int16_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The int16_t attribute value in the given position.
        */
        int16_t getInt16(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a int32_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The int32_t attribute value in the given position.
        */
        int32_t getInt32(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a int64_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The int64_t attribute value in the given position.
        */
        int64_t getInt64(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a raster attribute value.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The raster attribute value in the given position.


          \exception Exception It can throw an exception if:
                     <ul>
                     <li>something goes wrong during the data retrieval</li>
                     <li>the maximum number of simultaneous accesses has been reached</li>
                     </ul>
        */
        std::shared_ptr<te::rst::Raster> getRaster(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a date and time attribute value.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return The date and time attribute value in the given position.
        */
        std::shared_ptr<te::dt::DateTime> getDateTime(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief It checks if the attribute value is NULL.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return True if the attribute value is NULL.
        */
        bool isNull(std::size_t row, std::size_t columnIndex) const;

        /*!
          \brief Method for retrieving a geometric attribute value.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The geometric attribute value in the given position.
        */
        std::shared_ptr<te::gm::Geometry> getGeometry(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a string value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The string attribute value in the given position.
        */
        std::string getString(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a numeric value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The numeric attribute value in the given position.
        */
        std::string getNumeric(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a double value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The double attribute value in the given position.
        */
        double getDouble(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a bool value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The bool attribute value in the given position.
        */
        bool getBool(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a int16_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The int16_t attribute value in the given position.
        */
        int16_t getInt16(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a int32_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The int32_t attribute value in the given position.
        */
        int32_t getInt32(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a int64_t value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The int64_t attribute value in the given position.
        */
        int64_t getInt64(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a raster attribute value.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The raster attribute value in the given position.


          \exception Exception It can throw an exception if:
                     <ul>
                     <li>something goes wrong during the data retrieval</li>
                     <li>the maximum number of simultaneous accesses has been reached</li>
                     </ul>
        */
        std::shared_ptr<te::rst::Raster> getRaster(std::size_t row, std::string columnName) const;

        /*!
          \brief Method for retrieving a string value attribute.

          \param row The position the dataset internal pointer must be set up.
          \param columnName The name of the column of interest.

          \return The string attribute value in the given position.
        */
        std::shared_ptr<te::dt::DateTime> getDateTime(std::size_t row, std::string columnName) const;

        /*!
          \brief It checks if the attribute value is NULL.

          \param row The position the dataset internal pointer must be set up.
          \param columnIndex The column index of interest.

          \return True if the attribute value is NULL.
        */
        bool isNull(std::size_t row, std::string columnName) const;

        /*!
          \brief It returns the collection size, if it is known.

          It may return std::string::npos if the size is not known,
          or it would be too costly to compute it.

          \return The size of the collection, if it is known.
        */
        std::size_t size() const;

        /*!
          \brief It returns a smart pointer to the TerraLib dataset.

          \return The smart pointer to the TerraLib dataset.
        */
        std::shared_ptr<te::da::DataSet> dataset() const;

      protected:
        std::shared_ptr<te::da::DataSet> dataset_; //!< Smart pointer to the TerraLib dataset.
        mutable std::mutex mutex_; //!< Mutex to syncronize the dataset access.

    };
  } // end namespace core
}   // end namespace terrama2

#endif //__TERRAMA2_CORE_SYNCRONIZED_DATASET_HPP__
