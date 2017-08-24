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
 * \file InterpolatorParams.hpp
 *
 * \brief This file contains definitions of parameters to be passed to the interpolators.
 *
 * \author Frederico Augusto Bedê
 *
 * \todo Comment the attributes of terrama2::services::interpolator::core::BCInterpolatorParams.
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__

#include "../../../core/data-model/Filter.hpp"
#include "../../../core/Typedef.hpp"

#include "Typedef.hpp"

// TerraLib
#include <terralib/geometry/Envelope.h>

//STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \enum InterpolatorType
         *
         * See the Interpolator documentation for more details about the algorithms available.
         *
         * \ingroup interpolator
         */
        enum InterpolatorType
        {
          NEARESTNEIGHBOR,  //!< Nearest-neighbor aproach.
          BILINEAR,         //!< Bilinear aproach.
          BICUBIC           //!< Bicubic aproach.
        };

        /*!
         * \struct InterpolatorParams
         *
         * \brief Generic parameters for interpolation algorithms.
         *
         * \ingroup interpolator
         */
        struct InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          InterpolatorParams() :
            resolutionX_(0),
            resolutionY_(0),
            interpolationType_(NEARESTNEIGHBOR),
            srid_(0)
          {
            filter_.reset(new terrama2::core::Filter);
            filter_->lastValues = std::make_shared<long unsigned int>(1);
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          InterpolatorParams(const InterpolatorParams& other)
          {
            resolutionX_ = other.resolutionX_;
            resolutionY_ = other.resolutionY_;
            interpolationType_ = other.interpolationType_;
            bRect_ = other.bRect_;
            fileName_ = other.fileName_;
            filter_.reset(new terrama2::core::Filter(*other.filter_.get()));
            series_ = other.series_;
            srid_ = other.srid_;
            attributeName_ = other.attributeName_;
            id_ = other.id_;
            serviceInstanceId_ = other.serviceInstanceId_;
            dataManager_ = other.dataManager_;
          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          virtual InterpolatorParams& operator=(const InterpolatorParams& other)
          {
            resolutionX_ = other.resolutionX_;
            resolutionY_ = other.resolutionY_;
            interpolationType_ = other.interpolationType_;
            bRect_ = other.bRect_;
            fileName_ = other.fileName_;
            filter_.reset(new terrama2::core::Filter(*other.filter_.get()));
            series_ = other.series_;
            srid_ = other.srid_;
            attributeName_ = other.attributeName_;
            id_ = other.id_;
            serviceInstanceId_ = other.serviceInstanceId_;
            dataManager_ = other.dataManager_;

            return *this;
          }

          unsigned int resolutionX_;                            //!< Number of columns of the output raster file.
          unsigned int resolutionY_;                            //!< Number of rows of the output raster file.
          InterpolatorType interpolationType_;                  //!< The interpolation algorithm to be used.
          te::gm::Envelope bRect_;                              //!< The bounding rect of the output raster.
          std::string fileName_;                                //!< The complete path for the output raster.
          std::unique_ptr<terrama2::core::Filter> filter_;      //!< Information on how input data should be filtered before storage.
          DataSeriesId series_;                                 //!< The indentifier of the data to be used by the interpolator.
          int srid_;                                            //!< SRID for the output.
          std::string attributeName_;                           //!< Name of the attribute to be used by the interpolator.
          InterpolatorId id_;                                   //!< Identifier of the interpolator being used.
          ServiceInstanceId serviceInstanceId_;                 //!< Identifier of the service.
          DataManagerPtr dataManager_;                          //!< Pointer to the data manager.
        };

        /*!
         * \struct NNInterpolatorParams
         *
         * \brief Parameters for a nearest-neigbor strategy of interpolation.
         *
         * \ingroup interpolator
         */
        struct NNInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          NNInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType_ = NEARESTNEIGHBOR;
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The object to be copied.
           */
          NNInterpolatorParams(const InterpolatorParams& other) :
            InterpolatorParams(other)
          {
            const InterpolatorParams* otherP = &other;

            const NNInterpolatorParams* nnPar = dynamic_cast<const NNInterpolatorParams*>(otherP);

            if(nnPar != 0)
              *this = NNInterpolatorParams::operator=(*nnPar);
            else
              *this = InterpolatorParams::operator=(other);
          }


          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          NNInterpolatorParams(const NNInterpolatorParams& other) :
            InterpolatorParams(other)
          {

          }

          /*!
           * \brief Copy operator.
           *
           * \param other The object to be copied.
           *
           * \return A pointer to the object itself.
           */
          InterpolatorParams& operator=(const InterpolatorParams& other)
          {
            const InterpolatorParams* otherP = &other;

            const NNInterpolatorParams* nnPar = dynamic_cast<const NNInterpolatorParams*>(otherP);

            if(nnPar != 0)
              return NNInterpolatorParams::operator=(*nnPar);
            else
              return InterpolatorParams::operator=(other);
          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          NNInterpolatorParams& operator=(const NNInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            return *this;
          }
        };

        /*!
         * \struct BLInterpolatorParams
         *
         * \brief Parameters for a bilinear strategy of interpolation.
         *
         * \ingroup interpolator
         */
        struct BLInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          BLInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType_ = BILINEAR;
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          BLInterpolatorParams(const BLInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            bilRowMin_ = other.bilRowMin_;
            bilRowMax_ = other.bilRowMax_;
            bilColMin_ = other.bilColMin_;
            bilColMax_ = other.bilColMax_;
            bilRowDifMin_ = other.bilRowDifMin_;
            bilRowDifMax_ = other.bilRowDifMax_;
            bilColDifMin_ = other.bilColDifMin_;
            bilColDifMax_ = other.bilColDifMax_;
            std::copy(other.bilDistances_, other.bilDistances_ + 4, bilDistances_);
            std::copy(other.bilWeights_, other.bilWeights_ + 4, bilWeights_);
            bilValues_ = other.bilValues_;
            bilLastRow_ = other.bilLastRow_;
            bilLastCol_ = other.bilLastCol_;
          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          BLInterpolatorParams& operator=(const BLInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            bilRowMin_ = other.bilRowMin_;
            bilRowMax_ = other.bilRowMax_;
            bilColMin_ = other.bilColMin_;
            bilColMax_ = other.bilColMax_;
            bilRowDifMin_ = other.bilRowDifMin_;
            bilRowDifMax_ = other.bilRowDifMax_;
            bilColDifMin_ = other.bilColDifMin_;
            bilColDifMax_ = other.bilColDifMax_;
            std::copy(other.bilDistances_, other.bilDistances_ + 4, bilDistances_);
            std::copy(other.bilWeights_, other.bilWeights_ + 4, bilWeights_);
            bilValues_ = other.bilValues_;
            bilLastRow_ = other.bilLastRow_;
            bilLastCol_ = other.bilLastCol_;

            return *this;
          }

          double bilRowMin_;                                //!< Minimum row for bilinear interpolation.
          double bilRowMax_;                                //!< Maximum row for bilinear interpolation.
          double bilColMin_;                                //!< Minimum column for bilinear interpolation.
          double bilColMax_;                                //!< Maximum column for bilinear interpolation.
          double bilRowDifMin_;                             //!< Minimum difference between rows (min/max).
          double bilRowDifMax_;                             //!< Maximum difference between rows (min/max).
          double bilColDifMin_;                             //!< Minimum difference between columns (min/max).
          double bilColDifMax_;                             //!< Maximum difference between columns (min/max).
          double bilDistances_[4];                          //!< Bilinear distances.
          double bilWeights_[4];                            //!< Bilinear weights;
          std::vector<std::complex<double> > bilValues_;    //!< Bilinear values;
          double bilLastRow_;                               //!< Last row available for bilinear interpolation.
          double bilLastCol_;                               //!< Last column available for bilinear interpolation.
        };

        /*!
         * \struct BCInterpolatorParams
         *
         * \brief Parameters for a bicubic strategy of interpolation.
         *
         * \ingroup interpolator
         */
        struct BCInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          BCInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType_ = BICUBIC;
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          BCInterpolatorParams(const BCInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            bicGridRow_ = other.bicGridRow_;
            bicGridCol_ = other.bicGridCol_;
            bicBufRow_ = other.bicBufRow_;
            bicBufCol_ = other.bicBufCol_;
            bicReadRealValue_ = other.bicReadRealValue_;
            bicReadImagValue_ = other.bicReadImagValue_;

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferReal_[i][j] = other.bicBbufferReal_[i][j];

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferImag_[i][j] = other.bicBbufferImag_[i][j];

            bicOffsetX_ = other.bicOffsetX_;
            bicOffsetY_ = other.bicOffsetY_;
            bicKernel_ = other.bicKernel_;
            std::copy(other.bicHWeights_, other.bicHWeights_ + 4, bicHWeights_);
            std::copy(other.bicVWeights_, other.bicVWeights_ + 4, bicVWeights_);
            bicHSum_ = other.bicHSum_;
            bicVSum_ = other.bicVSum_;
            bicRowAccumReal_ = other.bicRowAccumReal_;
            bicRowAccumImag_ = other.bicRowAccumImag_;
            std::copy(other.bicRowsValuesReal_, other.bicRowsValuesReal_ + 4, bicRowsValuesReal_);
            std::copy(other.bicRowsValuesImag_, other.bicRowsValuesImag_ + 4, bicRowsValuesImag_);
            bicRowBound_ = other.bicRowBound_;
            bicColBound_ = other.bicColBound_;
          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          BCInterpolatorParams& operator=(const BCInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            bicGridRow_ = other.bicGridRow_;
            bicGridCol_ = other.bicGridCol_;
            bicBufRow_ = other.bicBufRow_;
            bicBufCol_ = other.bicBufCol_;
            bicReadRealValue_ = other.bicReadRealValue_;
            bicReadImagValue_ = other.bicReadImagValue_;

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferReal_[i][j] = other.bicBbufferReal_[i][j];

            for(size_t i = 0; i < 4; i++)
              for(size_t j = 0; j < 4; j++)
                bicBbufferImag_[i][j] = other.bicBbufferImag_[i][j];

            bicOffsetX_ = other.bicOffsetX_;
            bicOffsetY_ = other.bicOffsetY_;
            bicKernel_ = other.bicKernel_;
            std::copy(other.bicHWeights_, other.bicHWeights_ + 4, bicHWeights_);
            std::copy(other.bicVWeights_, other.bicVWeights_ + 4, bicVWeights_);
            bicHSum_ = other.bicHSum_;
            bicVSum_ = other.bicVSum_;
            bicRowAccumReal_ = other.bicRowAccumReal_;
            bicRowAccumImag_ = other.bicRowAccumImag_;
            std::copy(other.bicRowsValuesReal_, other.bicRowsValuesReal_ + 4, bicRowsValuesReal_);
            std::copy(other.bicRowsValuesImag_, other.bicRowsValuesImag_ + 4, bicRowsValuesImag_);
            bicRowBound_ = other.bicRowBound_;
            bicColBound_ = other.bicColBound_;

            return *this;
          }

          unsigned bicGridRow_;                            //!<
          unsigned bicGridCol_;                            //!<
          unsigned bicBufRow_;                             //!<
          unsigned bicBufCol_;                             //!<
          double bicReadRealValue_;                        //!<
          double bicReadImagValue_;                        //!<
          double bicBbufferReal_[4][4];                    //!<
          double bicBbufferImag_[4][4];                    //!<
          double bicOffsetX_;                              //!<
          double bicOffsetY_;                              //!<
          double bicKernel_;                               //!<
          double bicHWeights_[4];                          //!<
          double bicVWeights_[4];                          //!<
          double bicHSum_;                                 //!<
          double bicVSum_;                                 //!<
          double bicRowAccumReal_;                         //!<
          double bicRowAccumImag_;                         //!<
          double bicRowsValuesReal_[4];                    //!<
          double bicRowsValuesImag_[4];                    //!<
          double bicRowBound_;                             //!< Last row available for bicubic interpolation.
          double bicColBound_;                             //!< Last column available for bicubic interpolation.
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__
