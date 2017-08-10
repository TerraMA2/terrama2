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
  \file interpolator/core/Typedef.hpp

  \brief Contains definitions of types used by the interpolation mechanism.

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__

#include "../../../core/data-model/Process.hpp"
//#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/Typedef.hpp"

// TerraLib
#include <terralib/geometry/Envelope.h>

//! Unique identifier of a Interpolator.
typedef ProcessId InterpolatorId;

namespace te
{
  namespace rst
  {
    // Forward declarations
    class Raster;
  }
}

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \enum
         *
         * \brief The InterpolatorTypes enum
         */
        enum InterpolatorType
        {
          NEARESTNEIGHBOR,
          BILINEAR,
          BICUBIC
        };

        /*!
         * \struct InterpolatorParams
         *
         * \brief Generic params for interpolation algorithms.
         */
        struct InterpolatorParams
        {
          int resolutionX;
          int resolutionY;
          InterpolatorType interpolationType;
          te::gm::Envelope bRect;
          std::string fileName;
          terrama2::core::Filter filter;      //!< Information on how input data should be filtered before storage.
        };

        /*!
         * \struct NNIterpolatorParams
         *
         * \brief Params for a nearest neigbor strategy of interpolation.
         */
        struct NNIterpolatorParams : public InterpolatorParams
        {
          NNIterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = NEARESTNEIGHBOR;
          }

          unsigned int nnCR;
          unsigned int nnRR;
          double nnLastRow;                               //!< Last row available for nearest Neighbor interpolation.
          double nnLastCol;                               //!< Last column available for nearest Neighbor interpolation.

        };

        /*!
         * \struct BLInterpolatorParams
         *
         * \brief Params for a bilinear strategy of interpolation.
         */
        struct BLInterpolatorParams : public InterpolatorParams
        {
          BLInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = BILINEAR;
          }

          double bilRowMin;                                //!< Minimum row for bilinear interpolation.
          double bilRowMax;                                //!< Maximum row for bilinear interpolation.
          double bilColMin;                                //!< Minimum column for bilinear interpolation.
          double bilColMax;                                //!< Maximum column for bilinear interpolation.
          double bilRowDifMin;                             //!< Minimum difference between rows (min/max).
          double bilRowDifMax;                             //!< Maximum difference between rows (min/max).
          double bilColDifMin;                             //!< Minimum difference between columns (min/max).
          double bilColDifMax;                             //!< Maximum difference between columns (min/max).
          double bilDistances[4];                          //!< Bilinear distances.
          double bilWeights[4];                            //!< Bilinear weights;
          std::vector<std::complex<double> > bilValues;    //!< Bilinear values;
          double bilLastRow;                               //!< Last row available for bilinear interpolation.
          double bilLastCol;                               //!< Last column available for bilinear interpolation.
        };

        /*!
         * \struct BCInterpolatorParams
         *
         * \brief Params for  a bicubic strategy of interpolation.
         */
        struct BCInterpolatorParams : public InterpolatorParams
        {
          BCInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = BICUBIC;
          }

          unsigned bicGridRow;
          unsigned bicGridCol;
          unsigned bicBufRow;
          unsigned bicBufCol;
          double bicReadRealValue;
          double bicReadImagValue;
          double bicBbufferReal[4][4];
          double bicBbufferImag[4][4];
          double bicOffsetX;
          double bicOffsetY;
          double bicKernel;
          double bicHWeights[4];
          double bicVWeights[4];
          double bicHSum;
          double bicVSum;
          double bicRowAccumReal;
          double bicRowAccumImag;
          double bicRowsValuesReal[4];
          double bicRowsValuesImag[4];
          double bicRowBound;                              //!< Last row available for bicubic interpolation.
          double bicColBound;                              //!< Last column available for bicubic interpolation.
        };
      }
    }
  }
}



#endif // __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__
