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
  \file terrama2/ws/collector/core/Collector.hpp

  \brief Collector...

  \author Gilberto Ribeiro de Queiroz, Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_COLLECTOR_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_COLLECTOR_HPP__

#include "Format.hpp"

// STL
#include <memory>
#include <string>

// QT
#include <QJsonObject>
#include <QFile>



namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        class StorageStrategy;

        class Collector
        {
        public:

          /*!
          * \brief Constructor.
          */
          Collector(const int& id, const std::string& name, const std::string& description, const std::string& type, const Format& format, const int& updateFreqMinutes, const std::string& prefix, const int& srid, const std::string& mask, const std::string& unit, const std::string& timeZone, const QJsonObject& dynamicMetadata);

          Collector();

          /*!
          * \brief Destructor
          */
          virtual ~Collector();

          /*!
            \brief It returns the identifier.

            \return The identifier.
          */
          int getId() const;

          /**
          * \brief  It sets the identifier
          *
          * \param id The identifier
          */
          void setId(const int& id);

          /*!
            \brief It returns the name of the collector.

            \return The name of the collector.
          */
          std::string getName() const;

          /**
          * \brief  It sets the name of the collector
          *
          * \param name The name of the collector
          */
          void setName(const std::string& name);

          /*!
            \brief It returns the description of the collector.

            \return The description of the collector.
          */
          std::string getDescription() const;

          /*!
            \brief It sets the description of the collector.

            \param The description of the collector.
          */
          void setDescription(const std::string& description);

          /*!
            \brief It returns the format of the file collected.

            \return The format of the file collected.
          */
          Format getFormat() const;

          /*!
            \brief It sets the format of the file collected.

            \param The format of the file collected.
          */
          void setFormat(const Format& format);

          /*!
            \brief It returns the update frequency in minutes.

            \return The update frequency in minutes.
          */
          int getUpdateFreqMinutes() const;

          /*!
            \brief It sets the update frequency in minutes.

            \param The update frequency in minutes.
          */
          void setUpdateFreqMinutes(const int& updateFreqMinutes);

          /*!
            \brief It returns the prefix to be used in the name of the collected file.

            \return The prefix to be used in the name of the collected file.
          */
          std::string getPrefix() const;

          /*!
            \brief It sets the prefix to be used in the name of the collected file.

            \param The prefix to be used in the name of the collected file.
          */
          void setPrefix(const std::string& prefix);

          /*!
            \brief It returns the srid of the projection.

            \return The srid of the projection.
          */
          int getSrid() const;

          /*!
            \brief It sets the srid of the projection.

            \param The srid of the projection.
          */
          void setSrid(const int& srid);

          /*!
            \brief It returns the mask to be used to filter the files to be collected.

            \return The mask to be used to filter the files to be collected.
          */
          std::string getMask() const;

          /*!
            \brief It sets the mask to be used to filter the files to be collected.

            \param The mask to be used to filter the files to be collected.
          */
          void setMask(const std::string& mask);

          /*!
            \brief It returns the unit of the acquired data.

            \return The unit of the acquired data.
          */
          std::string getUnit() const;

          /*!
            \brief It sets the unit of the acquired data.

            \param The unit of the acquired data.
          */
          void setUnit(const std::string& unit);

          /*!
            \brief It returns the timezone of the collector.

            \return The timezone of the collector.
          */
          std::string getTimeZone() const;

          /*!
            \brief It sets the timezone of the collector.

            \param The timezone of the collector.
          */
          void setTimeZone(const std::string& timeZone);

          /*!
            \brief It returns the dynamic metadata.

            \return The dynamic metadata.
          */
          QJsonObject getDynamicMetadata();

          /*!
            \brief It sets the dynamic metadata.

            \param The dynamic metadata.
          */
          void setDynamicMetadata(const QJsonObject& dynamicMetadata);

          /*!
            \brief It returns the storage strategy for this collector.

            \return The storage strategy for this collector.
          */
          std::shared_ptr<StorageStrategy> getStorageStrategy() const;

          /*!
            \brief It sets the storage strategy for this collector.

            \param The storage strategy for this collector.
          */
          void setStorageStrategy(std::shared_ptr<StorageStrategy> storageStrategy);

          /*!
            \brief This method should be implemented by each subclass
            in order to read the collected file and store it in a format that can be used in terralib.
          */
          virtual void collect(const std::string& file) = 0;

        protected:
          int id_;
          std::string name_;
          std::string description_;
          std::string type_;
          Format format_;
          int updateFreqMinutes_;
          std::string prefix_;
          int srid_;
          std::string mask_;
          std::string unit_;
          std::string timeZone_;
          QJsonObject dynamicMetadata_;
          std::shared_ptr<StorageStrategy> storageStrategy_;

        };
      } // core
    } // collector
  } // ws
} // terrama2

#endif // __TERRAMA2_WS_COLLECTOR_CORE_COLLECTOR_HPP__

