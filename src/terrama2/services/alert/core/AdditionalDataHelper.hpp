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
  \file terrama2/services/alert/core/AdditionalDataHelper.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_ADDITIONAL_DATA_HELPER_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_ADDITIONAL_DATA_HELPER_HPP__


#include "Shared.hpp"
#include "Alert.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"

namespace te
{
  namespace da
  {
    class DataSetType;
  } /* da */
  namespace mem
  {
    class DataSetItem;
  } /* mem */
} /* te */

namespace terrama2
{
  namespace core
  {
    class DataSetMapper;
  } /* core */

  namespace services
  {
    namespace alert
    {
      namespace core
      {
        /*!
          \brief Helper class to access Alert AdditionalData.

          This class is used to add attributes and values to the output alertDataSet
        */
        class AdditionalDataHelper
        {
          public:
            /*!
              \brief Contructor, stores AdditionalData and DataManagerPtr.

              \warning Must call prepareData() after
            */
            AdditionalDataHelper(AdditionalData additionalData, DataManagerPtr dataManager);

            //! Default destructor
            ~AdditionalDataHelper() = default;
            AdditionalDataHelper(const AdditionalDataHelper& other) = default;
            AdditionalDataHelper(AdditionalDataHelper&& other) = default;
            AdditionalDataHelper& operator=(const AdditionalDataHelper& other) = default;
            AdditionalDataHelper& operator=(AdditionalDataHelper&& other) = default;

            //! Access the data and initialize the additional data te::da::DataSet.
            bool prepareData(terrama2::core::Filter filter);

            //! Add the additional attributes to the te::da::DataSetType of the alert output dataset.
            void addAdditionalAttributesColumns(std::shared_ptr<te::da::DataSetType> alertDataSetType) const;
            //! Add the value of the additional attributes to the te::mem::DataSetItem of the alert output dataset.
            void addAdditionalValues(te::mem::DataSetItem* item, const std::string& fkValue) const;

          private:
            bool isDataReady_ = false;//!< Flag that mark as true if prepareData() was called and data was loaded.
            AdditionalData additionalData_;//!< AdditionalData to be included in the Alert Report.
            terrama2::core::DataSeriesPtr dataSeries_;//!< DataSeries of the AdditionalData
            terrama2::core::DataProviderPtr dataProvider_;//!< DataProvider of the AdditionalData
            std::shared_ptr<terrama2::core::FileRemover> remover_; //Temporary files remover.

            std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries> dataMap_;//!< DataSetSeries map of the AdditionalData
            std::unordered_map<terrama2::core::DataSetPtr, std::shared_ptr<terrama2::core::DataSetMapper> > mapperMap_;//!< DataSetMapper map of the AdditionalData
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_ALERT_CORE_ADDITIONAL_DATA_HELPER_HPP__
