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
  \file InterpolatorParams.hpp

  \brief

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__

#include "../../../core/data-model/Filter.hpp"

// TerraLib
#include <terralib/geometry/Envelope.h>


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
          NEARESTNEIGHBOR,  //!<
          BILINEAR,         //!<
          BICUBIC           //!<
        };

        /*!
         * \struct InterpolatorParams
         *
         * \brief Generic params for interpolation algorithms.
         */
        struct InterpolatorParams
        {
          InterpolatorParams()
          {

          }

          InterpolatorParams(const InterpolatorParams& other)
          {
            resolutionX = other.resolutionX;
            resolutionY = other.resolutionY;
            interpolationType = other.interpolationType;
            bRect = other.bRect;
            fileName = other.fileName;
            filter = other.filter;
            series = other.series;
          }

          virtual InterpolatorParams& operator=(const InterpolatorParams& other)
          {
            resolutionX = other.resolutionX;
            resolutionY = other.resolutionY;
            interpolationType = other.interpolationType;
            bRect = other.bRect;
            fileName = other.fileName;
            filter = other.filter;
            series = other.series;

            return *this;
          }

          int resolutionX;                    //!<
          int resolutionY;                    //!<
          InterpolatorType interpolationType; //!<
          te::gm::Envelope bRect;             //!<
          std::string fileName;               //!<
          terrama2::core::Filter filter;      //!< Information on how input data should be filtered before storage.
          DataSeriesId series;                //!<
        };

        /*!
         * \struct NNIterpolatorParams
         *
         * \brief Params for a nearest neigbor strategy of interpolation.
         */
        struct NNInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief NNIterpolatorParams
           */
          NNInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = NEARESTNEIGHBOR;
          }

          NNInterpolatorParams(const NNInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            nnCR = other.nnCR;
            nnRR = other.nnRR;
            nnLastRow = other.nnLastRow;
            nnLastCol = other.nnLastCol;
          }

          NNInterpolatorParams& operator=(const NNInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            nnCR = other.nnCR;
            nnRR = other.nnRR;
            nnLastRow = other.nnLastRow;
            nnLastCol = other.nnLastCol;

            return *this;
          }


          unsigned int nnCR;                              //!<
          unsigned int nnRR;                              //!<
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
          /*!
           * \brief BLInterpolatorParams
           */
          BLInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = BILINEAR;
          }

          BLInterpolatorParams(const BLInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            bilRowMin = other.bilRowMin;
            bilRowMax = other.bilRowMax;
            bilColMin = other.bilColMin;
            bilColMax = other.bilColMax;
            bilRowDifMin = other.bilRowDifMin;
            bilRowDifMax = other.bilRowDifMax;
            bilColDifMin = other.bilColDifMin;
            bilColDifMax = other.bilColDifMax;
            std::copy(other.bilDistances, other.bilDistances + 4, bilDistances);
            std::copy(other.bilWeights, other.bilWeights + 4, bilWeights);
            bilValues = other.bilValues;
            bilLastRow = other.bilLastRow;
            bilLastCol = other.bilLastCol;
          }

          BLInterpolatorParams& operator=(const BLInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            bilRowMin = other.bilRowMin;
            bilRowMax = other.bilRowMax;
            bilColMin = other.bilColMin;
            bilColMax = other.bilColMax;
            bilRowDifMin = other.bilRowDifMin;
            bilRowDifMax = other.bilRowDifMax;
            bilColDifMin = other.bilColDifMin;
            bilColDifMax = other.bilColDifMax;
            std::copy(other.bilDistances, other.bilDistances + 4, bilDistances);
            std::copy(other.bilWeights, other.bilWeights + 4, bilWeights);
            bilValues = other.bilValues;
            bilLastRow = other.bilLastRow;
            bilLastCol = other.bilLastCol;

            return *this;
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
          /*!
           * \brief BCInterpolatorParams
           */
          BCInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType = BICUBIC;
          }

          BCInterpolatorParams(const BCInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            bicGridRow = other.bicGridRow;
            bicGridCol = other.bicGridCol;
            bicBufRow = other.bicBufRow;
            bicBufCol = other.bicBufCol;
            bicReadRealValue = other.bicReadRealValue;
            bicReadImagValue = other.bicReadImagValue;

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferReal[i][j] = other.bicBbufferReal[i][j];

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferImag[i][j] = other.bicBbufferImag[i][j];

            bicOffsetX = other.bicOffsetX;
            bicOffsetY = other.bicOffsetY;
            bicKernel = other.bicKernel;
            std::copy(other.bicHWeights, other.bicHWeights + 4, bicHWeights);
            std::copy(other.bicVWeights, other.bicVWeights + 4, bicVWeights);
            bicHSum = other.bicHSum;
            bicVSum = other.bicVSum;
            bicRowAccumReal = other.bicRowAccumReal;
            bicRowAccumImag = other.bicRowAccumImag;
            std::copy(other.bicRowsValuesReal, other.bicRowsValuesReal + 4, bicRowsValuesReal);
            std::copy(other.bicRowsValuesImag, other.bicRowsValuesImag + 4, bicRowsValuesImag);
            bicRowBound = other.bicRowBound;
            bicColBound = other.bicColBound;
          }

          BCInterpolatorParams& operator=(const BCInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            bicGridRow = other.bicGridRow;
            bicGridCol = other.bicGridCol;
            bicBufRow = other.bicBufRow;
            bicBufCol = other.bicBufCol;
            bicReadRealValue = other.bicReadRealValue;
            bicReadImagValue = other.bicReadImagValue;

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferReal[i][j] = other.bicBbufferReal[i][j];

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferImag[i][j] = other.bicBbufferImag[i][j];

            bicOffsetX = other.bicOffsetX;
            bicOffsetY = other.bicOffsetY;
            bicKernel = other.bicKernel;
            std::copy(other.bicHWeights, other.bicHWeights + 4, bicHWeights);
            std::copy(other.bicVWeights, other.bicVWeights + 4, bicVWeights);
            bicHSum = other.bicHSum;
            bicVSum = other.bicVSum;
            bicRowAccumReal = other.bicRowAccumReal;
            bicRowAccumImag = other.bicRowAccumImag;
            std::copy(other.bicRowsValuesReal, other.bicRowsValuesReal + 4, bicRowsValuesReal);
            std::copy(other.bicRowsValuesImag, other.bicRowsValuesImag + 4, bicRowsValuesImag);
            bicRowBound = other.bicRowBound;
            bicColBound = other.bicColBound;

            return *this;
          }

          unsigned bicGridRow;                            //!<
          unsigned bicGridCol;                            //!<
          unsigned bicBufRow;                             //!<
          unsigned bicBufCol;                             //!<
          double bicReadRealValue;                        //!<
          double bicReadImagValue;                        //!<
          double bicBbufferReal[4][4];                    //!<
          double bicBbufferImag[4][4];                    //!<
          double bicOffsetX;                              //!<
          double bicOffsetY;                              //!<
          double bicKernel;                               //!<
          double bicHWeights[4];                          //!<
          double bicVWeights[4];                          //!<
          double bicHSum;                                 //!<
          double bicVSum;                                 //!<
          double bicRowAccumReal;                         //!<
          double bicRowAccumImag;                         //!<
          double bicRowsValuesReal[4];                    //!<
          double bicRowsValuesImag[4];                    //!<
          double bicRowBound;                             //!< Last row available for bicubic interpolation.
          double bicColBound;                             //!< Last column available for bicubic interpolation.
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__
