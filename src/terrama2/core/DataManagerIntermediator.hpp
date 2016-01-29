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
  \file terrama2/core/DataManagerIntermediator.hpp

  \brief Helper function to convert from JSON and to JSON, TerraMA² data.

  \author Jano Simas
*/

//Qt
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

namespace terrama2
{
  namespace core
  {
    //! Class with functions to convert from JSON and to JSON, TerraMA² data to be sent via socket.
    class DataManagerIntermediator
    {
    public:
      //! Constructor for a intermediator for an instance of a service.
      DataManagerIntermediator(const std::string& instanceName);

      //! Prepare terrama2::core::* classes to be sent via socket.
      template <class T>
      DataManagerIntermediator& operator<<(const T& obj)
      {
        QJsonObject jsonObj = obj.toJson();
        jsonPackage_.append(jsonObj);

        return *this;
      }

      /*!
         \brief Send prepared data do remote service instace.
       */
      void commit();

      //! Clear prepared data.
      void abort() noexcept;

      /*!
         \brief Parses a QJsonArray and call the apropriate methods of the core::DataManager.
       */
      static void fromJSON(const QJsonArray& jsonArray);
      /*!
         \brief Parses a QJsonObject and call the apropriate methods of the core::DataManager.
       */
      static void fromJSONValue(const QJsonValue& jsonValue);

    private:
      /*!
         \brief Process a QJsonObject of a DataProvider.

         If it's a valid core::DataProvider and there is already a DataProvider with the same id, updates the core::DataProvider;
         if there isn't a core::DataProvider, add a new one.

         If it's not a valid core::DataProvider but there is one with the same id, removes it.
       */
      static void processDataProvider(const QJsonObject& jsonObject);

      /*!
         \brief Process a QJsonObject of a DataSet.

         If it's a valid core::DataSet and there is already a DataSet with the same id, updates the core::DataSet;
         if there isn't a core::DataSet, add a new one.

         If it's not a valid core::DataSet but there is one with the same id, removes it.
       */
      static void processDataSet(const QJsonObject& jsonObject);

      std::string instanceName_; //!< Name of an instace of a service.
      QJsonArray jsonPackage_;//!< JSON string to be sent.
    };
  }
}
