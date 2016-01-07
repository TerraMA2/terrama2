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
  \file terrama2/core/Intersection.hpp

  \brief Intersection information of a DataSet.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_INTERSECTION_HPP__
#define __TERRAMA2_CORE_INTERSECTION_HPP__


// STL
#include <map>
#include <string>
#include <vector>

namespace terrama2
{
  namespace core
  {

    /*!
      \class Intersection

      \brief Intersection information of a DataSet.
     */
    class Intersection
    {

      public:

        /*!
          \brief Constructor.

          \param item The associated DataSet.
        */
        Intersection(uint64_t dataSetId = 0);

        /*! \brief Destructor. */
        ~Intersection();

        /*! \brief Returns the identifier to the associated DataSet. */
        uint64_t dataset() const;

        /*! \brief Sets the DataSet identifier. */
        void setDataSet(uint64_t id);

        /*! \brief Returns the attribute map. */
        std::map<std::string, std::vector<std::string> > attributeMap() const;

        /*! \brief Sets the attribute map. */
        void setAttributeMap(std::map<std::string, std::vector<std::string> >& attributeMap);

        /*! \brief Returns the band map. */
        std::map<uint64_t, std::string> bandMap() const;

        /*! \brief Sets the band map. */
        void setBandMap(std::map<uint64_t, std::string >& bandMap);


      private:

        uint64_t dataset_; //!< Associates the intersection to a given DataSet.
        std::map<std::string, std::vector<std::string> > attributeMap_; //!< Map with the table name and the columns to be added to the collected data.
        std::map<uint64_t, std::string> bandMap_; //!< Map with the grid DataSet identifier and the bands that should be added to the collected data.

    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_INTERSECTION_HPP__

