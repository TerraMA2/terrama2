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
  \file terrama2/gui/config/AnalysisList.hpp

  \brief Definition of methods in class AnalysisList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _ANALYSIS_LIST_H_
#define _ANALYSIS_LIST_H_

// TerraMA2
#include "Analysis.hpp"

// QT
#include <QList>
#include <QObject>


class Services;
class ModAnalise;

/*! \brief Classe responsável por manter e gerenciar a lista de regras 
           de análises disponíveis
*/           
class AnalysisList : public QObject, public QList<Analysis*>
{
	Q_OBJECT

public:
  AnalysisList(Services* manager, ModAnalise* service);
  ~AnalysisList();
  
  bool loadData();
  bool addNewAnalysis(const Analysis* ana);
  bool updateAnalysis(int index, const Analysis* ana); 
  bool deleteAnalysis(int index, bool deleteLayers);

  int  numInstances(void) const { return _numInstances; }

  int       findAnalysisIndex(int analysisId) const;
  Analysis* findAnalysis(int analysisId) const;

signals:
  void afterUpdateAnalysis(int id, QString newName);

private:
  AnalysisList(const AnalysisList&);             //!< Remove the copy-constructor
  AnalysisList& operator=(const AnalysisList&);  //!< Remove the attrib operator

  Services*   _manager;
  ModAnalise* _service;
  int _numInstances;
};


#endif

