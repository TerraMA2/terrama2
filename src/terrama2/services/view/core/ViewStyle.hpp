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
  \file terrama2/services/view/core/ViewStyle.hpp

  \brief View style.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_VIEWSTYLE_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_VIEWSTYLE_HPP__

// TerraMA2
#include "MemoryDataSetLayer.hpp"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {

        class ViewStyle
        {
        public:

          ViewStyle() = default;

          // copy constructor
          ViewStyle(const ViewStyle& rhs);

          ~ViewStyle() = default;

          void setPolygonSymbolizer(te::se::Symbolizer* symbolizer) noexcept;

          void setLineSymbolizer(te::se::Symbolizer* symbolizer) noexcept;

          void setPointSymbolizer(te::se::Symbolizer* symbolizer) noexcept;

          te::se::Style* CreateFeatureTypeStyle(const te::gm::GeomType& geomType) const noexcept;

          te::se::Symbolizer* getSymbolizer(const te::gm::GeomType& geomType) const noexcept;

        private:

          // Assingment operator
          ViewStyle& operator=(const ViewStyle&);

        private:
          std::unique_ptr<te::se::Symbolizer> polygonSymbolizer_;
          std::unique_ptr<te::se::Symbolizer> lineSymbolizer_;
          std::unique_ptr<te::se::Symbolizer> pointSymbolizer_;
        };

        void MONO_0_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer);

        void RGB_012_RGB_Contrast_Style(std::shared_ptr<te::map::MemoryDataSetLayer> layer);

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_VIEWTYLE_HPP__
