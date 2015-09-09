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
  \file terrama2/ws/collector/server/DataProcessor.hpp

  \brief Implementation of a Data Processor.

  \author Jano Simas
*/


#ifndef __TERRAMA2_WS_COLLECTOR_SERVER_DATAPROCESSOR_HPP__
#define __TERRAMA2_WS_COLLECTOR_SERVER_DATAPROCESSOR_HPP__

//Std
#include <memory>
#include <cstdint>
#include <string>

#include <QObject>

namespace terrama2
{
  namespace core {
    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;
  }

  namespace ws
  {
    namespace collector
    {
      namespace server
      {
        class DataProcessor : public QObject
        {
            Q_OBJECT

          public:
            DataProcessor(QObject* parent = nullptr);
            ~DataProcessor(){}

            void import(std::string uri); //should run in thread //JANO: implementar import

        };

        typedef std::shared_ptr<DataProcessor> DataProcessorPtr;
      }
    }
  }
}


#endif //__TERRAMA2_WS_COLLECTOR_SERVER_DATAPROCESSOR_HPP__
