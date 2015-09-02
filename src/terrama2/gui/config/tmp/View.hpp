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
  \file terrama2/gui/config/View.hpp

  \brief Definition of Class View

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Frederic Dartayre
*/

#ifndef VIEW_H
#define VIEW_H

// QT
#include <QString>

#include "soapServPlanosProxy.h"

class View
{
public:
	View(const wsView& data);
	~View();

	//! Retorna o identificador da vista
	int id() const { return _data.id; }

	//! Retorna o nome da vista
	QString name() const { return QString::fromStdString(_data.name); }

	//! Retorna a estrutura que empacota os dados da vista
	const wsView& data() const { return _data; }

private:
	wsView _data;  //! Dados do grid empacotados na estrutura lida / recebida do módulo de análises
};

#endif // VIEW_H
