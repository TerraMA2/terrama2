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
  \file terrama2/analysis/core/Analysis.hpp

  \brief Model class for the analysis configuration.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
#define __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__

#include "../../../core/data-model/DataSeries.hpp"

// STL
#include <string>
#include <vector>

namespace terrama2
{
  namespace analysis
  {
    namespace core
    {
			class Analysis
      {

        public:

          enum InfluenceType
          {
            RADIUS_TOUCHES,
            RADIUS_CENTER,
            REGION
          };

          struct Influence
          {
            InfluenceType type;
            DataSeriesId dataSeriesId;
            std::string attribute;
            double radius;
          };

          enum Type
          {
            PCD_TYPE,
            MONITORED_OBJECT_TYPE,
            GRID_TYPE,
            TERRAME_TYPE
          };

          enum ScriptLanguage
          {
            PYTHON,
            LUA
          };

          void setId(uint64_t id);
					uint64_t id() const;

          void setAdditionalDataList(const std::vector<terrama2::core::DataSeries>& additionalDataList);
          std::vector<terrama2::core::DataSeries> additionalDataList() const;

          void setMonitoredObject(const terrama2::core::DataSeries& monitoredObject);
          terrama2::core::DataSeries monitoredObject() const;


          void setDCP(const terrama2::core::DataSeries& DCP, Influence influence);
          terrama2::core::DataSeries DCP() const;

					void setScriptLanguage(const ScriptLanguage scriptLanguage);
					ScriptLanguage scriptLanguage() const;

					void setScript(const std::string& script);
					std::string script() const;

					void setDescription(const std::string& description);
					std::string description() const;

					void setType(const Type type);
          Type type() const;

          Influence influence(DataSeriesId dataSeriesId) const;
          void setInfluence(DataSeriesId dataSeriesId, Influence influence);

				private:
					uint64_t id_;
          std::vector<terrama2::core::DataSeries> additionalDataList_;
					terrama2::core::DataSeries monitoredObject_;
          terrama2::core::DataSeries pcd_;
					ScriptLanguage scriptLanguage_;
					std::string script_;
					std::string description_;
          Type type_;
          std::map<DataSeriesId, Influence> mapInfluence_;

			};
		}
  }
}

#endif //__TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
