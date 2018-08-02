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
  \file terrama2/core/data-access/SynchronizedDataSet.hpp

  \brief Thread-safe access to a interpolator.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_CORE_SYNCHRONIZED_INTERPOLATOR_HPP__
#define __TERRAMA2_CORE_SYNCHRONIZED_INTERPOLATOR_HPP__

// TerraMa2
#include "../Config.hpp"

// TerraLib
#include <terralib/raster/Interpolator.h>

// STL
#include <memory>
#include <mutex>

namespace terrama2
{
  namespace core
  {
    /**
     * @brief Provides sycronized access to a [te::rst::Interpolator](http://www.dpi.inpe.br/terralib5/doxygendoc/d2/da4/classte_1_1rst_1_1Interpolator.html).
     * 
     * 
     */ 
    class TMCOREEXPORT SynchronizedInterpolator : protected te::rst::Interpolator
    {
      public:
        SynchronizedInterpolator(te::rst::Raster const* r, int m);

        /**
         * @brief Check documentation on base class [te::rst::Interpolator](http://www.dpi.inpe.br/terralib5/doxygendoc/d2/da4/classte_1_1rst_1_1Interpolator.html).
         * 
         */
        virtual void getValue(const double& c, const double& r, std::complex<double>& v, const std::size_t& b);

        /**
         * @brief Check documentation on base class [te::rst::Interpolator](http://www.dpi.inpe.br/terralib5/doxygendoc/d2/da4/classte_1_1rst_1_1Interpolator.html).
         * 
         */
        virtual void getValues(const double& c, const double& r, std::vector<std::complex<double> >& values);

      protected:
        mutable std::mutex mutex_; //!< Mutex to synchronize the interpolator access.

    };
  } // end namespace core
}   // end namespace terrama2

#endif //__TERRAMA2_CORE_SYNCHRONIZED_INTERPOLATOR_HPP__
