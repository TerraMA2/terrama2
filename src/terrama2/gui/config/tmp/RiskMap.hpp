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
  \file terrama2/gui/config/RiskMap.hpp

  \brief Definition of methods in class RiskMap.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _RISKMAP_H_
#define _RISKMAP_H_

#include <QString>
#include <QDate>

#include "soapServPlanosProxy.h"

//! Classe responsável por representar um objeto monitorado
class RiskMap
{
public:
  RiskMap();
  RiskMap(const wsRiskMap& data);
  ~RiskMap();

  //! Retorna o identificador do objeto monitorado
  int id() const { return _data.id; }

  void setId(int id);  

  //! Retorna true se o mapa for novo e ainda não salvo na base de dados
  bool isNew() const { return _data.id == -1; }
  
  //! Retorna o nome do mapa
  QString name() const { return QString::fromStdString(_data.name); }

  //! Altera o nome do mapa
  void setName(QString name) { _data.name = name.toStdString(); }

  //! Retorna o nome do autor do mapa
  QString author() const { return QString::fromStdString(_data.author); }

  //! Altera o nome do autor do mapa
  void setAuthor(QString author) { _data.author = author.toStdString(); }

  //! Retorna o nome da instituição a que o autor está associado
  QString institution() const { return QString::fromStdString(_data.institution); }

  //! Altera o nome da instituição a que o autor está associado
  void setInstitution(QString name) { _data.institution = name.toStdString(); }

  //! Retorna uma descrição do mapa
  QString description() const { return QString::fromStdString(_data.details); }

  //! Altera a descrição do mapa
  void setDescription(QString description) { _data.details = description.toStdString(); }

  //! Retorna a data de criação do mapa
  QDate creationDate() const { return QDate(_data.creationYear, _data.creationMonth, _data.creationDay); }

  void setCreationDate(QDate date);

  //! Retorna a data de expiração do mapa
  QDate expirationDate() const { return QDate(_data. expirationYear, _data. expirationMonth, _data. expirationDay); }

  void setExpirationDate(QDate date);

  //! Retorna o número de atributos do objeto monitorado
  int numAttributes() const { return _data.baseRiskMapTheme.attributes.size(); }

  //! Retorna o nome do atributo de indice index
  QString attributeName(int index) const { return QString::fromStdString(_data.baseRiskMapTheme.attributes.at(index).name); }

  //! Retorna o nome do atributo de indice index
  wsColumnType attributeType(int index) const { return _data.baseRiskMapTheme.attributes.at(index).type; }

  //! Retorna a estrutura que empacota os dados do objeto monitorado
  const wsRiskMap& data() const { return _data; }

  //! Altera a estrutura com o tema do objeto monitorado
  void setRiskMapTheme(wsRiskMapTheme& riskMapTheme) { _data.baseRiskMapTheme = riskMapTheme; }

  //! Retorna a estrutura que empacota o tema do objeto monitorado
  const wsRiskMapTheme& riskMapTheme() const { return _data.baseRiskMapTheme; }

  //! Pega o nome do atributo de identificação
  const QString nameAttr() const { return QString::fromStdString(_data.nameAttr); }

  //! Define o nome do atributo de identificação
  void setNameAttr(QString nameAttr) { _data.nameAttr = nameAttr.toStdString();}

  //! Retorna as propriedades dos atributos do objeto monitorado
  std::vector<wsRiskMapProperty> attrProperties() const { return _data.attrProperties;}

  //! Altera as propriedades dos atributos do objeto monitorado
  void setAttrProperties(std::vector<wsRiskMapProperty> attrProperties) { _data.attrProperties = attrProperties; }
 
private:
  wsRiskMap _data;  //! Dados do mapa empacotados na estrutura lida / recebida do módulo de análises
};


#endif

