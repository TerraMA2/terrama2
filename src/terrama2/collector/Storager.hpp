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
  \file terrama2/collector/Storager.hpp

  \brief Store a temporary terralib DataSet into the permanent storage area.

  \author Jano Simas
*/

#ifndef __TERRAMA2_COLLECTOR_STORAGER_HPP__
#define __TERRAMA2_COLLECTOR_STORAGER_HPP__

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace collector
  {

    /*!
         * \brief The Storager class store the data in the final storage area and format.
         *
         * The Storager is responsible for creating the final storaging area and
         * converting the data (terralib) to the appropriate format.
         *
         */
    class Storager : public boost::noncopyable
    {
      public:
        Storager(const std::map<std::string, std::string>& storageMetadata);

        /*!
          \brief Store a temporary data set in it's final storage area and format.
          \param Code name for the storage dataset, may not be used if defined in the storage metadata

          \return Pointer to a te::da::DataSet of the final storage.


          \exception TODO: Storager::store exception...
         */
        virtual void store(const std::string& standardDataSetName,
                           const std::vector<std::shared_ptr<te::da::DataSet> > &datasetVec,
                           const std::shared_ptr<te::da::DataSetType> &dataSetType) = 0;

      protected:
        std::map<std::string, std::string> storageMetadata_;

    };

    typedef std::shared_ptr<Storager> StoragerPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_STORAGER_HPP__
