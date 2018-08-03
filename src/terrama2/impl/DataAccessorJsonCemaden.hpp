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
  \file terrama2/core/data-access/DataAccessorJsonCemaden.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_JSON_CEMADEN_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_JSON_CEMADEN_HPP__

//TerraMA2
#include "Config.hpp"
#include "../core/Shared.hpp"
#include "../core/data-access/DataAccessor.hpp"

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataAccessorJsonCemaden

      Readings expected format:

      {
        "codestacao": "351300906H",
        "latitude": -23.6087,
        "longitude": -46.8921,
        "cidade": "COTIA",
        "nome": "Cotia",
        "tipo": "Hidrológica",
        "uf": "SP",
        "chuva": 0,
        "nivel": null,
        "offset": 6.524,
        "dataHora": "2018-03-22 11:20:00.0"
      },

    */
    class TMIMPLEXPORT DataAccessorJsonCemaden : public virtual DataAccessor
    {
      public:
        DataAccessorJsonCemaden(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
          : DataAccessor(dataProvider, dataSeries)
        {}
        virtual ~DataAccessorJsonCemaden() = default;

        using terrama2::core::DataAccessor::getSeries;

        virtual std::map<DataSetId, std::string> getFiles(const Filter& filter, std::shared_ptr<FileRemover> remover) const override;

        virtual void getSeriesCallback(const Filter& filter, std::shared_ptr<FileRemover> remover, std::function<void(const DataSetId&, const std::string& /*uri*/)> processFile) const override;
        virtual DataSetSeries getSeries(const std::string& uri, const Filter& filter, DataSetPtr dataSet, std::shared_ptr<terrama2::core::FileRemover> remover) const override;


        static DataAccessorPtr make(DataProviderPtr dataProvider, DataSeriesPtr dataSeries)
        {
          return std::make_shared<DataAccessorJsonCemaden>(dataProvider, dataSeries);
        }

        static DataAccessorType dataAccessorType() { return "DCP-json_cemaden"; }
        /*!
           \brief Retrieve data from server.

           Retrieved data is subject to filter.

         */
        virtual std::string retrieveData(const DataRetrieverPtr dataRetriever,
                                         DataSetPtr dataSet, const Filter& filter, std::shared_ptr<FileRemover> remover) const override;


      private:
        std::string getDCPCode(DataSetPtr dataset) const;
        std::string getCodePropertyName(DataSetPtr dataset) const;
        std::string getStaticDataProperties(DataSetPtr dataset) const;
        std::string getDataMask(DataSetPtr dataset) const;
        std::string getStationTypeId(DataSetPtr dataset) const;
        std::string getUf(DataSetPtr dataset) const;

        virtual std::string dataSourceType() const override;
        virtual void retrieveDataCallback(const DataRetrieverPtr dataRetriever,
                                          DataSetPtr dataset,
                                          const Filter& filter,
                                          std::shared_ptr<FileRemover> remover,
                                          std::function<void(const std::string& /*uri*/)> processFile) const override;

        std::string getFolderMask(DataSetPtr dataSet) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_JSON_CEMADEN_HPP__
