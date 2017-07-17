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
  \file terrama2/services/alert/core/AlertExecutor.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__

#include "../../../core/Shared.hpp"
#include "../../../core/utility/Service.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/data-model/Risk.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "Typedef.hpp"
#include "Alert.hpp"
#include "AlertLogger.hpp"
#include "DataManager.hpp"

#include <terralib/memory/DataSet.h>

#include <unordered_map>

namespace te
{
  namespace da
  {
    //! Terralib DataSet forward declaration
    class DataSet;
  } /* da */
} /* te */

namespace terrama2
{
  namespace core{
    class TeDataSetFKJoin;
  }

  namespace services
  {
    namespace alert
    {
      namespace core
      {
        struct comparatorAbstractData
        {
          bool operator()(const std::shared_ptr<te::dt::AbstractData>& a, const std::shared_ptr<te::dt::AbstractData>& b) const
          {
            return a->toString() < b->toString();
          }
        };

        class AlertExecutor : public QObject
        {
            Q_OBJECT

          public:

            //! Default constructor
            AlertExecutor();

            //! Default destructor
            ~AlertExecutor() = default;

            //! Default copy constructor
            AlertExecutor(const AlertExecutor& other) = delete;

            //! Default move constructor
            AlertExecutor(AlertExecutor&& other) = delete;

            //! Default const assignment operator
            AlertExecutor& operator=(const AlertExecutor& other) = delete;

            //! Default assignment operator
            AlertExecutor& operator=(AlertExecutor&& other) = delete;

            /*!
            \brief Method to execute the alert.
            */
            void runAlert(terrama2::core::ExecutionPackage executionPackage,
                          std::shared_ptr< AlertLogger > logger,
                          std::weak_ptr<DataManager> weakDataManager,
                          const std::map<std::string, std::string>& serverMap);

          protected:

            //! Get the name of the property used as unique key of the DataSet
            std::string getIdentifierPropertyName(terrama2::core::DataSetPtr dataSet, terrama2::core::DataSeriesPtr dataSeries);
            //! Get the propper function to evaluate the risk level of a value.
            std::function<std::tuple<int, std::string, std::string>(size_t pos)> createGetRiskFunction(terrama2::core::LegendPtr legend, std::shared_ptr<te::da::DataSet> teDataSet);

            std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, terrama2::services::alert::core::comparatorAbstractData>
            getResultMap(terrama2::core::LegendPtr risk,
                         size_t pos,
                         te::dt::Property* idProperty,
                         std::string datetimeColumnName,
                         std::shared_ptr<te::da::DataSet> teDataset,
                         std::vector<std::shared_ptr<te::dt::DateTime> > vecDates);

            std::shared_ptr<te::mem::DataSet> populateMonitoredObjectAlertDataset(std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                                  std::map<std::shared_ptr<te::dt::AbstractData>, std::map<std::string, std::pair<std::shared_ptr<te::dt::AbstractData>, uint32_t> >, comparatorAbstractData> riskResultMap,
                                                                                  AlertPtr alertPtr,
                                                                                  te::dt::Property* fkProperty,
                                                                                  std::shared_ptr<te::da::DataSetType> alertDataSetType);

            std::shared_ptr<te::mem::DataSet> populateGridAlertDataset( terrama2::core::DataSetPtr dataset,
                                                                        AlertPtr alertPtr,
                                                                        terrama2::core::LegendPtr legend,
                                                                        std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                        std::shared_ptr<te::da::DataSet> teDataset,
                                                                        std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                        std::string datetimeColumnName);

            void addAdditionalData(std::shared_ptr<te::mem::DataSet> alertDataSet,
                                   const std::vector<AdditionalData>& additionalDataVector,
                                   std::unordered_map<std::string, terrama2::core::TeDataSetFKJoin> additionalDataMap);

            std::shared_ptr<te::mem::DataSet> monitoredObjectAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                                   std::string datetimeColumnName,
                                                                   std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                                   AlertPtr alertPtr,
                                                                   terrama2::core::LegendPtr legend,
                                                                   terrama2::core::Filter filter,
                                                                   terrama2::core::DataSetPtr dataset,
                                                                   std::shared_ptr<te::da::DataSet> teDataset,
                                                                   te::dt::Property* idProperty,
                                                                   const std::vector<AdditionalData>& additionalDataVector,
                                                                   std::unordered_map<DataSeriesId, std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr> > tempAdditionalDataVector,
                                                                   std::shared_ptr<terrama2::core::FileRemover> remover);

            std::shared_ptr<te::mem::DataSet> gridAlert(std::shared_ptr<te::da::DataSetType> dataSetType,
                                                        std::string datetimeColumnName,
                                                        std::vector<std::shared_ptr<te::dt::DateTime> > vecDates,
                                                        AlertPtr alertPtr,
                                                        terrama2::core::LegendPtr legend,
                                                        terrama2::core::Filter filter,
                                                        terrama2::core::DataSetPtr dataset,
                                                        std::shared_ptr<te::da::DataSet> teDataset);

            std::shared_ptr<te::da::DataSetType> createAlertDataSetType(AlertPtr alertPtr,
                                                                        terrama2::core::DataSetPtr dataset);

            //! Create an alert document and return the uri
            std::string makeDocument(ReportPtr reportPtr, const Notification& notification, const terrama2::core::ExecutionPackage& executionPackage, std::shared_ptr< AlertLogger > logger) const;

            //! Send alert notification
            void sendNotification(const std::map<std::string, std::string>& serverMap,
                                  ReportPtr reportPtr,
                                  const Notification& notification,
                                  const std::string& documentURI,
                                  terrama2::core::ExecutionPackage executionPackage,
                                  std::shared_ptr< AlertLogger > logger) const ;

          signals:
            //! Signal to notify that a analysis execution has finished.
            void alertFinished(size_t, std::shared_ptr< te::dt::TimeInstantTZ >, bool, QJsonObject = QJsonObject());
        };

      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_RUN_ALERT_HPP__
