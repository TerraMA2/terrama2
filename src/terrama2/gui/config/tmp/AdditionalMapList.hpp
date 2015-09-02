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
  \file terrama2/gui/config/AddicionalMapList.hpp

  \brief Definition of methods in class AdditionalMapList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _ADDITIONALMAP_LIST_H_
#define _ADDITIONALMAP_LIST_H_

// TerraMA2
#include "AdditionalMap.hpp"

// QT
#include <QList>
#include <QObject>

class Services;
class ServPlanos;

/*! \brief Class responsible to keep and to handle the list of risk maps available

*/
class AdditionalMapList : public QObject, public QList<AdditionalMap*>
{
	Q_OBJECT

public:
  AdditionalMapList(Services* manager, ServPlanos* service);
  ~AdditionalMapList();

  bool loadData();
  bool loadAdditionalMapThemeData();
  bool addNewAdditionalMap(const AdditionalMap* rm, bool isGrid);
  bool updateAdditionalMap(int id, const AdditionalMap* rm); 
  bool deleteAdditionalMap(int id);

  int  findAdditionalMapIndex(int mapId) const;
  AdditionalMap* findAdditionalMap(int mapId) const;

  //! Return the list of themes for additional maps
  const std::vector<struct wsTheme>& getAdditionalMapThemesGrid() const { return _additionalMapThemesEligibleGrid; }
  const std::vector<struct wsTheme>& getAdditionalMapThemesVector() const { return _additionalMapThemesEligibleVector; }

signals:
  void afterUpdateAdditionalMap(int id, QString newName);

private:
  AdditionalMapList(const AdditionalMapList&);             //!< Remove the copy-constructor
  AdditionalMapList& operator=(const AdditionalMapList&);  //!< Remove the attrib. operator

  Services*   _manager; //! Service Handler
  ServPlanos* _service; //! A reference for remote service access of plans

  std::vector<struct wsTheme> _additionalMapThemesEligibleGrid;
  std::vector<struct wsTheme> _additionalMapThemesEligibleVector;
};


#endif

