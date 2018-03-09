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
 * \file interpolator/core/DataManager.hpp
 *
 * \brief This file contains a class that manages the interpolation parameters.
 *
 * \author Frederico Augusto Bedê
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__

#include "../../../core/data-model/DataManager.hpp"

#include "Typedef.hpp"

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \class DataManager
         *
         * \brief This class is used to manage the interpolation parameters created and used by TerraMA2.
         *
         * \ingroup interpolator
         */
        class DataManager : public terrama2::core::DataManager
        {
            Q_OBJECT

          public:


            /** @name Initializer Methods
            *  Methods related to instantiation and destruction.
            */
            //@{

            /*!
             * \brief Default constructor.
             */
            DataManager();

            /*!
             * \brief Destructor.
             */
            ~DataManager();
            //@}


            /** @name JSon methods
             *  Methods used to manipulate .json based objects.
             */
            //@{

            /*!
             * \brief Adds the parameters to the manager. Decode the json object and transforms it into a InterpolatorParams object.
             *
             * \param obj The object to be converted and added.
             */
            void addJSon(const QJsonObject& obj);

            /*!
             * \brief Removes the InterpolatorParams object based on json object \a obj.
             *
             * \param obj The object to be removed.
             */
            void removeJSon(const QJsonObject& obj);
            //@}


            /** @name Copy Methods
            *  Methods related to copy. Copy NOT allowed.
            */
            //@{

            /*!
             * \brief Copy constructor.
             */
            DataManager(const DataManager&) = delete;

            /*!
             * \brief Copy constructor. (Using pointer as argument)
             */
            DataManager(DataManager&&) = delete;

            /*!
             * \brief Copy operator.
             * \return No matters.
             */
            DataManager& operator=(const DataManager&) = delete;

            /*!
             * \brief Copy operator. (Using pointer as argument)
             * \return No matters.
             */
            DataManager& operator=(DataManager&&) = delete;
            //@}

            using terrama2::core::DataManager::add;
            using terrama2::core::DataManager::update;

            /** @name Manipulating Parameters Methods
            *  Methods related to the manipulation of the parameters.
            */
            //@{

            /*!
             * \brief Adds the parameters tho the manage.
             *
             * \param params The parameters to be inserted.
             *
             * \exception terrama2::InvalidArgumentException If the identifier of the given parameters was invalid an exception will be raised.
             */
            void add(InterpolatorParamsPtr params);

            /*!
             * \brief Updates the parameters int the manager.
             *
             * \param params The parameters to be updated.
             *
             * \exception terrama2::InvalidArgumentException If the identifier of the given parameters does not exists an exception will be raised.
             */
            void update(InterpolatorParamsPtr params);

            /*!
             * \brief Removes the parameters identified by \a id, from the manager.
             *
             * \param id Identifier of the parameters to be removed.
             *
             * \exception terrama2::InvalidArgumentException If the identifier of the given parameters does not exists an exception will be raised.
             */
            void removeInterpolator(InterpolatorId id);

            /*!
             * \brief Finds the identified by \a id.
             *
             * \param id Identifer of the parameters of interest.
             *
             * \return The parameters found.
             *
             * \exception terrama2::InvalidArgumentException If the identifier of the given parameters does not exists an exception will be raised.
             */
            InterpolatorParamsPtr findInterpolatorParams(InterpolatorId id) const;

            /*!
             * \brief Searches for the parameters indentified by \a id.
             *
             * \param id Identifier of the parameters to be serched.
             *
             * \return \a True if the parameters were found and \a false otherwise.
             */
            bool hasInterpolator(InterpolatorId id) const;
            //@}

          signals:

            /** @name Signals
            *  Signals emmited by the object to warn of the changings occurring in it.
            */
            //@{

            /*!
             * \brief Parameters added.
             */
            void interpolatorAdded(InterpolatorParamsPtr);

            /*!
             * \brief Parameters updated
             */
            void interpolatorUpdated(InterpolatorParamsPtr);

            /*!
             * \brief Parameters removed
             */
            void interpolatorRemoved(InterpolatorId);
            //@}

          protected:
            std::map<InterpolatorId, InterpolatorParamsPtr> interpolatorsParams_; //!< A map from InterpolatorId to Interpolator.
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_DATAMANAGER_HPP__
