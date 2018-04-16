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
  \file src/unittest/services_integration_test/OccTs.hpp
  \brief test services_integration OccTs
  \author Bianca Maciel
*/


#ifndef __TERRAMA2_UNITTEST_COLLECTOR_OCC_HPP__
#define __TERRAMA2_UNITTEST_COLLECTOR_OCC_HPP__

#include <QtTest>
#include "Utils.hpp"


class OccurenceTs: public QObject
{
    Q_OBJECT

  private:

    void collectFile();
    void analysisZonal();
    void collectFTP();
    void collectIntersectionFile();
    std::shared_ptr<terrama2::services::analysis::core::Analysis> addAnalysisBase(std::shared_ptr<terrama2::services::analysis::core::DataManager> dataManagerAnalysis, std::string scriptAnalysis);

  private slots:

    void initTestCase(){ } // Run before all tests
    void cleanupTestCase(){  } // Run after all tests

    void init(){ utilsTS::database::createDB(); } //run before each test
    void cleanup(){ utilsTS::database::deleteDB(); } //run before each test

    //******Test functions********


     void collectTocantinsFTP(){ collectFTP(); }

     void collectTocantinsIntersectionFile(){ collectIntersectionFile(); }

      void collectAndAnalysisZonal() { collectFile(); /*analysisZonal();*/}


};

#endif //__TERRAMA2_UNITTEST_COLLECTOR_OCC_HPP__

