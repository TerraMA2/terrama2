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
  \file terrama2/gui/config/View.cpp

  \brief Definition of Class View

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Frederic Dartayre
*/

/*!
\class View

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados
retornada pelo modulo de planos

*/

#include <assert.h>

#include "viewList.h"

#include "services.h"

View::View(const wsView& data)
{
	assert(data.id >= 0);
  _data = data;
}

//! Destrutor
View::~View()
{
}
