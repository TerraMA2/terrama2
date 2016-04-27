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
  \file src/terrama2/services/analysis/core/JSonUtils.hpp

  \brief Utility functions to encode and decode analysis objects to/from JSON.

  \author Paulo R. M. Oliveira
*/

#include "Analysis.hpp"

// Qt
#include <QJsonObject>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        Analysis fromAnalysisJson(const QJsonObject& json);

        QJsonObject toJson(const Analysis& analysis);

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2
