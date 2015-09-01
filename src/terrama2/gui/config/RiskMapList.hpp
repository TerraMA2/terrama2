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
  \file terrama2/gui/config/RiskMapList.hpp

  \brief Definition of methods in class RiskMapList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _RISKMAP_LIST_H_
#define _RISKMAP_LIST_H_

// QT
#include <QList>
#include <QObject>

// TerraMA2
#include "RiskMap.hpp"

class Services;
class ServPlanos;

/*! \brief Classe responsável por manter e gerenciar a lista de objetos
           monitorados disponíveis
*/           
class RiskMapList : public QObject, public QList<RiskMap*>
{
	Q_OBJECT

public:
  RiskMapList(Services* manager, ServPlanos* service);
  ~RiskMapList();
  
  bool loadData();
  bool loadRiskMapThemeData();
  bool addNewRiskMap(const RiskMap* rm);
  bool updateRiskMap(int id, const RiskMap* rm); 
  bool deleteRiskMap(int id);

  int      findMapIndex(int mapId) const;
  RiskMap* findMap(int mapId) const;

  //! Retorna a lista de temas para objetos monitorados
  const std::vector<struct wsRiskMapTheme>& getRiskMapThemes() const { return _riskMapThemesEligible; }
  
signals:
    void beforeDeleteRiskMapList(const int& index, const wsRiskMap& data);
    void afterUpdateRiskMapList(const int& index, const wsRiskMap& data);
    void afterInsertRiskMapList(const int& index, const wsRiskMap& data);


private:
  RiskMapList(const RiskMapList&);             //!< Remove construtor de cópia default
  RiskMapList& operator=(const RiskMapList&);  //!< Remove operador de atrib. default

  Services*   _manager; //! Gerenciador de serviços
  ServPlanos* _service; //! Referência para acesso ao serviço remoto de Planos

  std::vector<struct wsRiskMapTheme> _riskMapThemesEligible;
};


#endif

