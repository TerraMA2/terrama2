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
  \file terrama2/gui/config/CellularSpaceList.hpp

  \brief Definition of methods in class CellularSpaceList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Gustavo Sampaio
*/

#ifndef _CELLULARSPACE_LIST_H_
#define _CELLULARSPACE_LIST_H_

// QT
#include <QList>
#include <QObject>


// TerraMA2
#include "CellularSpace.hpp"

class Services;
class ServPlanos;

/*! \brief Classe responsável por manter e gerenciar a lista de espaços celulares disponíveis
*/           
class CellularSpaceList : public QObject, public QList<CellularSpace*>
{
	Q_OBJECT

public:
	CellularSpaceList(Services* manager, ServPlanos* service);
	~CellularSpaceList();

	bool loadData();
	bool loadCellThemeData();
	bool addNewCellularSpace(const CellularSpace* cs);
	bool updateCellularSpace(int id, const CellularSpace* cs); 
	bool deleteCellularSpace(int id);

	int      findCellularSpaceIndex(int csId) const;
	CellularSpace* findCellularSpace(int csId) const;

	//! Retorna a lista de temas para espaços celulares
	const std::vector<struct wsTheme>& getCellThemes() const { return _cellThemesEligible; }

signals:
	void beforeDeleteCellularSpaceList(const int& index);
	void afterUpdateCellularSpaceList(const int& index, const wsCellularSpace& data);
	void afterInsertCellularSpaceList(const int& index, const wsCellularSpace& data);

private:
	CellularSpaceList(const CellularSpaceList&);             //!< Remove construtor de cópia default
	CellularSpaceList& operator=(const CellularSpaceList&);  //!< Remove operador de atrib. default

	Services*   _manager; //! Gerenciador de serviços
	ServPlanos* _service; //! Referência para acesso ao serviço remoto de Planos

	std::vector<struct wsTheme> _cellThemesEligible;
};


#endif

