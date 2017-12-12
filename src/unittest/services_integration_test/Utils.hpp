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
  \file src/unittest/services_integration_test/Utils.cpp
  \brief test services_integration Utils
  \author Bianca Maciel
*/

#ifndef __TERRAMA2_UNITTEST_UTILS_HPP__
#define __TERRAMA2_UNITTEST_UTILS_HPP__


// STL
#include <iostream>

#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/core/utility/ServiceManager.hpp>
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/Raii.hpp>


//Collector

#include <terrama2/services/collector/core/Service.hpp>
#include <terrama2/services/collector/core/DataManager.hpp>
#include <terrama2/services/collector/core/Collector.hpp>
#include <terrama2/services/collector/mock/MockCollectorLogger.hpp>


//Analysis
#include <terrama2/services/analysis/core/python/PythonInterpreter.hpp>
#include <terrama2/services/analysis/core/utility/PythonInterpreterInit.hpp>
#include <terrama2/services/analysis/core/DataManager.hpp>
#include <terrama2/services/analysis/core/Service.hpp>
#include <terrama2/services/analysis/core/Analysis.hpp>
#include <terrama2/services/analysis/mock/MockAnalysisLogger.hpp>

#include <QString>
#include <QCoreApplication>
#include <QTest>

namespace utilsTS {

  namespace typecollectoranalysis
  {
     const std::string dcp = "dcp";
     const std::string occ = "occ";
     const std::string dcp_history = "dcp_history";
     const std::string operator_dcp = "operator_dcp";
     const std::string operator_history_interval = "operator_history_interval";
     const std::string estados = "estados";

  }

  namespace database
  {

        void interpreterScriptPy(std::string path);


        void restoreDB(std::string typeAnalysis);

        int compareCollector(std::string type);

        int compareAnalysis(std::string typeAnalysis);

        void deleteDB();

        void createDB();

        void compareCollectAndAnalysis(std::string typeAnalysis);

        void tryCatchCollector(std::string  typeAnalysis, int numTablesExpected);

        void tryCatchAnalysis(std::string  typeAnalysis, int numTablesExpected);
  }



    namespace collector
    {
        std::unique_ptr<terrama2::services::collector::core::Service> gmockAndServicesCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector);

        void addInputCollect(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector, std::string typeCollector);

        void addResultCollector(std::shared_ptr<terrama2::services::collector::core::DataManager> dataManagerCollector, std::string typeCollector);
    }

    namespace analysis
    {
        terrama2::core::DataSeriesPtr addInputDataSeriesAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string typeAnalysis);

        terrama2::core::DataSeriesPtr addInputDataSeriesStatic(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis);

        terrama2::core::DataSeriesPtr addResultAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, terrama2::core::DataSeriesPtr dataSeries, std::string typeAnalysis);

        std::unique_ptr<terrama2::services::analysis::core::Service> gmockAndServicesAnalysis(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis);
    }

    void timerCollectorAndAnalysis();




}
#endif //__TERRAMA2_UNITTEST_UTILS_HPP__
