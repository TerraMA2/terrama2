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
  \file terrama2/gui/config/Analysis.hpp

  \brief Definition of methods in class Analysis.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

// TerraMA2
#include "soapModAnaliseProxy.h"

// QT
#include <QString>
#include <QList>

class RiskMap;
class CellularSpace;
class WeatherGrid;
class View;
class AlertIcon;

//! Class resnposible for demonstrate an analyse role
class Analysis
{
public:
  Analysis();
  Analysis(const wsAnalysisRule& data);
  ~Analysis();

  //! Return analyse role identifier
  int id() const { return _data.id; }

  void setId(int id);  

  //! Return true if role is brand new that no longer to be storaged on database.
  Retorna true se a regra for uma regra nova ainda não salva na base de dados
  bool isNew() const { return _data.id == -1; }

  //! Return the analyse role type
  wsAnalysisType getAnalysisType() const { return _data.type; }

  //! Change the analyse role type
  void setAnalysisType(wsAnalysisType anaType) {_data.type = anaType;}

  //! Return the analyse role name
  QString name() const { return QString::fromStdString(_data.name); }

  //! Change the analyse role name
  void setName(QString name) { _data.name = name.toStdString(); }

  //! Return author name of analyse role
  QString author() const { return QString::fromStdString(_data.author); }

  //! Change the autor name of analyse role
  void setAuthor(QString author) { _data.author = author.toStdString(); }

  //! Return the institution name that belong to an author
  QString institution() const { return QString::fromStdString(_data.institution); }

  //! Change the institution name that belong to an author
  void setInstitution(QString name) { _data.institution = name.toStdString(); }

  //! Return description of analyse role
  QString description() const { return QString::fromStdString(_data.details); }

  //! Change the description of analyse role
  void setDescription(QString description) { _data.details = description.toStdString(); }

  //! Return the analyse role script
  QString script() const { return QString::fromStdString(_data.luaScript); }

  //! Change the analyse role script
  void setScript(QString script) { _data.luaScript = script.toStdString(); }

  //! Return the generated output plan name (monitored object without role)
  QString outputName() const { return QString::fromStdString(_data.gridOutputName); }

  //! Change the generated output plan name (monitored object without role)
  void setOutputName(QString name) { _data.gridOutputName = name.toStdString(); }

  //! Return the monitored object identifier associated with an analyse role
  int riskMapId() const { return _data.riskMap.id; }

  void setRiskMap(const RiskMap& map);

  //! Return the PCD identifier associated with an analyse role
  int pcdId() const { return _data.pcd.id; }

  void setPCD(const WeatherGrid& pcd);

  //! Return the sight identifier associated with an analyse role
  int viewId() const { return _data.pcdView.id; }

  void setView(const View &view);

  //! Return the alert icons associated with an analyse role
  wsAlertIcons alertIcons() const { return _data.alertIcons; }

  //! Change the alert icon configuration
  void setAlertIcons(wsAlertIcons config) { _data.alertIcons = config; }

  //! Return the "cell space" identifier associated with an analyse role
  int cellularSpaceId() const {return _data.cellularSpace.id;}

  void setCellularSpace(const CellularSpace& cs);

  //! Return whather number grids associated with an analyse role
  int numWeatherGrids() const { return _data.inputWeatherDataSources.size(); }

  //! Return index identifier of grid
  int weatherGridId(int index) const { return _data.inputWeatherDataSources.at(index).id; }

  //! Return grid name from index
  QString weatherGridName(int index) const { return QString::fromStdString(_data.inputWeatherDataSources.at(index).name); }
  
  //! Return the geometry grid type
  wsWeatherDataSourceGeometry weatherGeometry(int index) const { return _data.inputWeatherDataSources.at(index).geometry; }

  //! Return data format
  wsWeatherDataSourceFileFormat weatherFormat(int index) const { return _data.inputWeatherDataSources.at(index).format; }

  QList<wsWeatherDataSource*> getWeatherGrids();

  void setWeatherGrids(const QList<WeatherGrid*>& gridlist);

  //! Return ids list of additional maps selected
  std::vector<struct wsAddMapDisplayConfig> listAdditionalMapId() const { return _data.additionalMaps; }

  //! Change ids list of additional maps selected
  void setListAdditionalMapId(std::vector<struct wsAddMapDisplayConfig> listAdditionalMaps) { _data.additionalMaps = listAdditionalMaps; }

  //! Return analyse module instance.
  int instance() const { return _data.instance; }

  //! Change analyse module instance.
  void setInstance(int instance) { _data.instance = instance; }

  //! Return analyse condition (Active, Inactive, Condition)
  enum wsAnalysisCondition conditionType() const { return _data.condition; }

  //! Change analyse execution condition (Active, Inactive, Condition)
  void setConditionType(enum wsAnalysisCondition type) { _data.condition = type; }

  //! Return analyse id which it depends from condition type is "condicionada"
  int conditionAnalysisId() const { return _data.conditionAnalysisID; }

  //! Change analyse id which it depends from condition type is "condicionada"
  void setConditionAnalysisId(int id) { _data.conditionAnalysisID = id; }

  //! Return script for condition analyses
  QString conditionScript() const { return QString::fromStdString(_data.conditionScript); }

  //! Change script for condition analyses
  void setConditionScript(QString script) { _data.conditionScript = script.toStdString(); }

  //! Return true if the analyse might to generate a jpg file after runtime
  bool generateImage() const { return _data.generateImage; }

  //! Change the field if the analyse might to generate a jpg file after runtime
  void setGenerateImage(bool generate) { _data.generateImage = generate; }

  //! Return the cell fill operations that ough to be applied each analyse input plan
  std::vector<struct wsFillOperation> getFillOperations() const {return _data.fillOperations;}

  //! Change the list of operations of cell filling
  void setFillOperations(std::vector<struct wsFillOperation> fillOps) {_data.fillOperations = fillOps;}

  //! Return a struct that wraps analyse data
  const wsAnalysisRule& data() const { return _data; }

  //! Return output grid configuration
  wsGridOutputConfig gridOutputConfig() const { return _data.gridOutputConfig; }

  //! Change the output grid configuration
  void setGridOutputConfig(wsGridOutputConfig config) { _data.gridOutputConfig = config; }

private:
  wsAnalysisRule _data;  //! Wrapped analyse data
};


#endif

