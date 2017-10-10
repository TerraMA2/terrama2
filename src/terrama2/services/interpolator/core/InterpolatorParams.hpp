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
          NEARESTNEIGHBOR,     //!< Nearest-neighbor aproach.
          AVGDIST,             //!< Simple average of the neighbors aproach.
          SQRAVGDIST           //!< Weight average of the neighbors aproach.
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
            srid_(0),
            active_(false)
          {
            filter_.lastValues = std::make_shared<long unsigned int>(1);
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
            filter_ = other.filter_;
            series_ = other.series_;
            outSeries_ = other.outSeries_;
            srid_ = other.srid_;
            attributeName_ = other.attributeName_;
            id_ = other.id_;
            serviceInstanceId_ = other.serviceInstanceId_;
            dataManager_ = other.dataManager_;
            numNeighbors_ = other.numNeighbors_;
            projectId_ = other.projectId_;
            active_ = other.active_;
          }

          /*!
           * \brief Destructor.
           */
          virtual ~InterpolatorParams()
          {

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
            filter_ = other.filter_;
            series_ = other.series_;
            outSeries_ = other.outSeries_;
            srid_ = other.srid_;
            attributeName_ = other.attributeName_;
            id_ = other.id_;
            serviceInstanceId_ = other.serviceInstanceId_;
            dataManager_ = other.dataManager_;
            numNeighbors_ = other.numNeighbors_;
            projectId_ = other.projectId_;
            active_ = other.active_;

            return *this;
          }

          double resolutionX_;                                  //!< Resolution in x-asis of the output (unit of the output srid).
          double resolutionY_;                                  //!< Resolution in y-asis of the output (unit of the output srid).
          InterpolatorType interpolationType_;                  //!< The interpolation algorithm to be used.
          mutable te::gm::Envelope bRect_;                      //!< The bounding rect of the output raster.
          terrama2::core::Filter filter_;                       //!< Information on how input data should be filtered before storage.
          DataSeriesId series_;                                 //!< The indentifier of the data to be used by the interpolator.
          DataSeriesId outSeries_;                              //!< The indentifier of the output data.
          int srid_;                                            //!< SRID for the output.
          std::string attributeName_;                           //!< Name of the attribute to be used by the interpolator.
          InterpolatorId id_;                                   //!< Identifier of the interpolator being used.
          ServiceInstanceId serviceInstanceId_;                 //!< Identifier of the service.
          DataManager* dataManager_;                            //!< Pointer to the data manager.
          size_t numNeighbors_;                                 //!< Number of neighbors to be used in computations.
          ProjectId projectId_;                                 //!< Identifier of the project.
          bool active_;                                         //!< True if the service is active, false otherwise.
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
            InterpolatorParams::numNeighbors_ = 1;
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
           * \brief Destructor.
           */
          ~NNInterpolatorParams()
          {

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
         * \struct AvgDistInterpolatorParams
         *
         * \brief Parameters for a simple average of neighbors strategy of interpolation.
         *
         * \ingroup interpolator
         */
        struct AvgDistInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          AvgDistInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType_ = AVGDIST;
            InterpolatorParams::numNeighbors_ = 4;
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          AvgDistInterpolatorParams(const AvgDistInterpolatorParams& other) :
            InterpolatorParams(other)
          {
          }

          /*!
           * \brief Destructor.
           */
          ~AvgDistInterpolatorParams()
          {

          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          AvgDistInterpolatorParams& operator=(const AvgDistInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);

            return *this;
          }
        };

        /*!
         * \struct SqrAvgDistInterpolatorParams
         *
         * \brief Parameters for a weight distance of neighbors strategy of interpolation.
         *
         * \ingroup interpolator
         */
        struct SqrAvgDistInterpolatorParams : public InterpolatorParams
        {
          /*!
           * \brief Default constructor.
           */
          SqrAvgDistInterpolatorParams() :
            InterpolatorParams()
          {
            InterpolatorParams::interpolationType_ = SQRAVGDIST;
            InterpolatorParams::numNeighbors_ = 4;
            pow_ = 2;
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The parameters to e copied.
           */
          SqrAvgDistInterpolatorParams(const SqrAvgDistInterpolatorParams& other) :
            InterpolatorParams(other)
          {
            pow_ = other.pow_;
          }

          /*!
           * \brief Destructor.
           */
          ~SqrAvgDistInterpolatorParams()
          {

          }

          /*!
           * \brief Copy operator.
           *
           * \param other The parameters to be copied.
           *
           * \return A pointer to the object itself.
           */
          SqrAvgDistInterpolatorParams& operator=(const SqrAvgDistInterpolatorParams& other)
          {
            InterpolatorParams::operator =(other);
            pow_ = other.pow_;

            return *this;
          }

          int pow_;                            //!< The expoent of to use with distance.
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_INTERPOLATORPARAMS_HPP__
