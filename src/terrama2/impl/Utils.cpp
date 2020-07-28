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
  \file terrama2/core/impl/Utils.hpp

  \brief Utility funtions for impl classes.

  \author Paulo R. M. Oliveira
 */

#include "Utils.hpp"

#include "DataAccessorDcpInpe.hpp"
#include "DataAccessorDcpToa5.hpp"
#include "DataAccessorDcpPostGIS.hpp"
#include "DataAccessorDCPSingleTable.hpp"
#include "DataAccessorGDAL.hpp"
#include "DataAccessorGrib.hpp"
#include "DataAccessorGrADS.hpp"
#include "DataAccessorNetCDF.hpp"
#include "DataAccessorOccurrenceWfp.hpp"
#include "DataAccessorOccurrenceLightning.hpp"
#include "DataAccessorOccurrencePostGIS.hpp"
#include "DataAccessorDCPCSV.hpp"
#include "DataAccessorOccurrenceCSV.hpp"
#include "DataAccessorGeometricObjectPostGIS.hpp"
#include "DataAccessorGeometricObjectOGR.hpp"
#include "DataAccessorWildFireEvent.hpp"
#include "DataAccessorJsonCemaden.hpp"

#include "DataAccessorStaticDataOGR.hpp"
#include "DataAccessorStaticDataPostGIS.hpp"
#include "DataAccessorStaticDataViewPostGIS.hpp"
#include "DataAccessorStaticGDAL.hpp"

#include "DataAccessorAnalysisPostGIS.hpp"

#include "DataStoragerPostGIS.hpp"
#include "DataStoragerTiff.hpp"
#include "DataStoragerCSV.hpp"
#include "DataStoragerDCPPostGIS.hpp"
#include "DataStoragerDCPSingleTable.hpp"

#include "DataRetrieverFTP.hpp"
#include "DataRetrieverHTTP.hpp"
#include "DataRetrieverHTTPS.hpp"
#include "DataRetrieverStaticHTTP.hpp"
#include "DataRetrieverWFS.hpp"
#include "DataAccessorWFS.hpp"

#include "../core/utility/DataAccessorFactory.hpp"
#include "../core/utility/DataStoragerFactory.hpp"
#include "../core/utility/DataRetrieverFactory.hpp"

#include "../core/interpreter/InterpreterFactory.hpp"
#include "../core/interpreter/PythonInterpreter.hpp"

void terrama2::core::registerFactories()
{
  // Data access
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorDcpInpe::dataAccessorType(), terrama2::core::DataAccessorDcpInpe::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorDcpToa5::dataAccessorType(), terrama2::core::DataAccessorDcpToa5::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorDcpPostGIS::dataAccessorType(), terrama2::core::DataAccessorDcpPostGIS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorGDAL::dataAccessorType(), terrama2::core::DataAccessorGDAL::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorGrib::dataAccessorType(), terrama2::core::DataAccessorGrib::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorGrADS::dataAccessorType(), terrama2::core::DataAccessorGrADS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorOccurrenceWfp::dataAccessorType(), terrama2::core::DataAccessorOccurrenceWfp::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorOccurrenceLightning::dataAccessorType(), terrama2::core::DataAccessorOccurrenceLightning::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorOccurrencePostGIS::dataAccessorType(), terrama2::core::DataAccessorOccurrencePostGIS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorDCPCSV::dataAccessorType(), terrama2::core::DataAccessorDCPCSV::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorOccurrenceCSV::dataAccessorType(), terrama2::core::DataAccessorOccurrenceCSV::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorGeometricObjectPostGIS::dataAccessorType(), terrama2::core::DataAccessorGeometricObjectPostGIS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorGeometricObjectOGR::dataAccessorType(), terrama2::core::DataAccessorGeometricObjectOGR::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorWildFireEvent::dataAccessorType(), terrama2::core::DataAccessorWildFireEvent::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorDcpSingleTable::dataAccessorType(), terrama2::core::DataAccessorDcpSingleTable::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorJsonCemaden::dataAccessorType(), terrama2::core::DataAccessorJsonCemaden::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorWFS::dataAccessorType(), terrama2::core::DataAccessorWFS::make);

  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorNetCDF::dataAccessorType(),
                                                         terrama2::core::DataAccessorNetCDF::make);

  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorStaticDataOGR::dataAccessorType(), terrama2::core::DataAccessorStaticDataOGR::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorStaticDataPostGIS::dataAccessorType(), terrama2::core::DataAccessorStaticDataPostGIS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorStaticDataViewPostGIS::dataAccessorType(), terrama2::core::DataAccessorStaticDataViewPostGIS::make);
  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorStaticGDAL::dataAccessorType(), terrama2::core::DataAccessorStaticGDAL::make);

  terrama2::core::DataAccessorFactory::getInstance().add(terrama2::core::DataAccessorAnalysisPostGIS::dataAccessorType(), terrama2::core::DataAccessorAnalysisPostGIS::make);
  // Data storager
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerOccurrencePostGIS::dataStoragerType(), terrama2::core::DataStoragerOccurrencePostGIS::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerAnalysisMonitoredObject::dataStoragerType(), terrama2::core::DataStoragerAnalysisMonitoredObject::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerVectorProcessingObject::dataStoragerType(), terrama2::core::DataStoragerVectorProcessingObject::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerDCPPostGIS::dataStoragerType(), terrama2::core::DataStoragerDCPPostGIS::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerDCPSingleTable::dataStoragerType(), terrama2::core::DataStoragerDCPSingleTable::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerTiff::dataStoragerType(), terrama2::core::DataStoragerTiff::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerInpeWfpCSV::dataStoragerType(), terrama2::core::DataStoragerInpeWfpCSV::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerOccurrenceCSV::dataStoragerType(), terrama2::core::DataStoragerOccurrenceCSV::make);
  terrama2::core::DataStoragerFactory::getInstance().add(terrama2::core::DataStoragerWildfireEvent::dataStoragerType(), terrama2::core::DataStoragerWildfireEvent::make);

  terrama2::core::DataRetrieverFactory::getInstance().add(terrama2::core::DataRetrieverFTP::dataRetrieverType(), terrama2::core::DataRetrieverFTP::make);
  terrama2::core::DataRetrieverFactory::getInstance().add(terrama2::core::DataRetrieverHTTP::dataRetrieverType(), terrama2::core::DataRetrieverHTTP::make);
  terrama2::core::DataRetrieverFactory::getInstance().add(terrama2::core::DataRetrieverHTTPS::dataRetrieverType(), terrama2::core::DataRetrieverHTTPS::make);
  terrama2::core::DataRetrieverFactory::getInstance().add(terrama2::core::DataRetrieverStaticHTTP::dataRetrieverType(), terrama2::core::DataRetrieverStaticHTTP::make);
  terrama2::core::DataRetrieverFactory::getInstance().add(terrama2::core::DataRetrieverWFS::dataRetrieverType(), terrama2::core::DataRetrieverWFS::make);

  terrama2::core::InterpreterFactory::getInstance().add(terrama2::core::PythonInterpreter::interpreterType(), terrama2::core::PythonInterpreter::make, terrama2::core::PythonInterpreter::createInitializer);
}
