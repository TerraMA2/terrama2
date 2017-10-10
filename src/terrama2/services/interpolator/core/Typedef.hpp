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

#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-model/Process.hpp"

// TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/sam/kdtree.h>


//! Unique identifier of a Interpolator
typedef ProcessId InterpolatorId;

namespace te
{
  namespace rst
  {
    // Forward declaration
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
        // Forward declarations
        class DataManager;
        struct Interpolator;
        struct InterpolatorParams;

        /*!
         * \struct InterpolatorData
         *
         * \brief Defines an object containing data, to be used by the KD-tree.
         *
         * \ingroup interpolator
         */
        struct InterpolatorData
        {
          /*!
           * \brief Default constructor.
           */
          InterpolatorData()
          {
            pt_ = new te::gm::Point(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
          }

          /*!
           * \brief Copy constructor.
           *
           * \param other The data to e copied.
           */
          InterpolatorData(const InterpolatorData& other)
          {
            pt_ = new te::gm::Point(*other.pt_);
            series_ = other.series_;
          }

          /*!
           * \brief Copy operator.
           *
           * \param other The data to be copied.
           *
           * \return A pointer to the object itself.
           */
          InterpolatorData& operator=(const InterpolatorData& other)
          {
            pt_ = new te::gm::Point(*other.pt_);
            series_ = other.series_;

            return *this;
          }


          /*!
           * \brief Returns the x-asis component of the coordinate.
           *
           * \return The x-asis component of the coordinate.
           */
          double getX() const
          {
            return pt_->getX();
          }

          /*!
           * \brief Returns the y-asis component of the coordinate.
           *
           * \return The y-asis component of the coordinate.
           */
          double getY() const
          {
            return pt_->getY();
          }

          /*!
           * \brief Returns \a true if both \a x and \a y are different of max double value, and \a false otherwise.
           *
           * \return \a True if coordinate is valid and \a false oherwise.
           */
          bool isValid() const
          {
            double max = std::numeric_limits<double>::max();

            return ((getX() != max) && (getY() != max));
          }

          te::gm::Point* pt_;                     //!< Coordinate of the sample.
          terrama2::core::DataSetSeries series_;  //!< Data of the sample.
        };


        /*!
         * \typedef DataSeriesVector
         *
         * \brief Defines a vector of InterpolatorData objects.
         *
         * \ingroup interpolator
         */
        typedef std::vector<InterpolatorData> DataSeriesVector;

        /*!
         * \typedef InterpolatorNode
         *
         * \brief Defines a specialized node to be used with kd-tree.
         *
         * \ingroup interpolator
         */
        typedef te::sam::kdtree::AdaptativeNode<te::gm::Point, DataSeriesVector, InterpolatorData> InterpolatorNode;

        /*!
         * \typedef InterpolatorTree
         *
         * \brief Defines a specialized kd-tree to be used by the interpolation mechanism.
         *
         * \ingroup interpolator
         */
        typedef te::sam::kdtree::AdaptativeIndex<InterpolatorNode> InterpolatorTree;

        /*!
         * \typedef InterpolatorParamsPtr
         *
         * \brief Defines a smart pointer for the InterpolatorParams struct.
         *
         * \ingroup interpolator
         */
        typedef std::shared_ptr<terrama2::services::interpolator::core::InterpolatorParams> InterpolatorParamsPtr;

        /*!
         * \typedef InterpolatorPtr
         *
         * \brief Defines a smart pointer for the Interpolator class.
         *
         * \ingroup interpolator
         */
        typedef std::shared_ptr<Interpolator> InterpolatorPtr;

        /*!
         * \typedef DataManagerPtr
         *
         * \brief Defines a smart pointer for the DataManager class.
         *
         * \ingroup interpolator
         */
        typedef std::shared_ptr<terrama2::services::interpolator::core::DataManager> DataManagerPtr;
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__
