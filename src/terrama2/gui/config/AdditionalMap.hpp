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
  \file terrama2/gui/config/AdditionalMap.hpp

  \brief Definition of methods in class AdditionalMap.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _ADDITIONAL_H_
#define _ADDITIONAL_H_


// TerraMA2
#include "soapServPlanosProxy.h"

// QT
#include <QString>
#include <QDate>

//! Class responsible to represent an additional map
class AdditionalMap
{
public:
  AdditionalMap();
  AdditionalMap(const wsAdditionalMap& data);
  ~AdditionalMap();

  //! Change the theme
  void setThemeStructure(wsTheme theme) { _data.baseTheme = theme; }

  //! Retrive the additional map identifier
  int id() const { return _data.id; }

  void setId(int id);  

  //! Return "true" if the additional map is brand new and it is not stored on database yet.
  Retorna true se o mapa adicional for novo e ainda não salvo na base de dados
  bool isNew() const { return _data.id == -1; }
  
  //! Retrieve the additional map name
  QString name() const { return QString::fromStdString(_data.name); }

  //! Change the additional map name
  void setName(QString name) { _data.name = name.toStdString(); }

  //! Retrieve the additional map author name
  QString author() const { return QString::fromStdString(_data.author); }

  //! Change the additional map author name
  void setAuthor(QString author) { _data.author = author.toStdString(); }

  //! Retrieve institution name whose author is associated
  QString institution() const { return QString::fromStdString(_data.institution); }

  //! Change the institution name whose author is associated
  void setInstitution(QString name) { _data.institution = name.toStdString(); }

  //! Retrieve a description of additional map
  QString description() const { return QString::fromStdString(_data.details); }

  //! Change the description of additional map
  void setDescription(QString description) { _data.details = description.toStdString(); }

  //! Retrieve the creation date of additional map
  QDate creationDate() const { return QDate(_data.creationYear, _data.creationMonth, _data.creationDay); }

  void setCreationDate(QDate date);

  //! Retrieve the expiration date of additional map
  QDate expirationDate() const { return QDate(_data. expirationYear, _data. expirationMonth, _data. expirationDay); }

  void setExpirationDate(QDate date);

  //! Retrieve the attributes number theme
  int numAttributes() const { return _data.baseTheme.attributes.size(); }

  //! Retrieve the attribute name from index
  QString attributeName(int index) const { return QString::fromStdString(_data.baseTheme.attributes.at(index).name); }

  //! Retrieve the attribute type from index
  wsColumnType attributeType(int index) const { return _data.baseTheme.attributes.at(index).type; }

  //! Retrieve the struct that wraps the additional map data
  const wsAdditionalMap& data() const { return _data; }

  //! Retrieve the struct that wraps the additional map theme
  const wsTheme& theme() const { return _data.baseTheme; }

  //! Change the theme parameters of rater type
  void setThemeRasterParameters(int nLines, int nColumns, double resX, double resY);

  //! Change the theme id
  void setThemeId(int id)     {	_data.baseTheme.id   = id; }

  //! Change the theme name
  void setThemeName(QString name) { _data.baseTheme.name = name.toStdString(); }

  //! Change the theme name
  void setThemeName(std::string name) { _data.baseTheme.name = name; }

  //! Change the geometry of theme
  void setThemeGeometry(enum wsLayerGeometry geometry) { _data.baseTheme.geometry = geometry; }

  //! Change the projection parameters of theme
  void setThemeProjection(const struct wsProjectionParams& projection) { _data.baseTheme.projection = projection; }

  //! Retrieve the id associated in Data Collection
  int dataCollectionId() {return _data.dataCollectionId;}

  //! Change the Data Collection ID associated
  void setDataCollectionId(int id) { _data.dataCollectionId = id; }

  //! Retrieve a set of attribute properties of additional map
  std::vector<wsRiskMapProperty> attrProperties() const { return _data.attrProperties;}

  //! Change the attribuite properties of additional map
  void setAttrProperties(std::vector<wsRiskMapProperty> attrProperties) { _data.attrProperties = attrProperties; }


private:
  wsAdditionalMap _data;
};


#endif

