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
  \file terrama2/gui/config/UserAnalysis.cpp

  \brief Definition of Class UserAnalysis.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Celso Luiz Ramos Cruz
*/

/*!
\class UserAnalysis

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de notificação
*/

// STL
#include <assert.h>

// TerraMA2
#include "UserAnalysis.hpp"


//! Construtor para novos usuários, ainda não salvos na base de dados
UserAnalysis::UserAnalysis()
{
  _data.login    = "";
  _data.password = "";
  _data.fullName = "";
  _data.email    = "";
  _data.cellular = "";

  _data.canvasSize = WS_CANVASSIZE_MEDIUM;
}

//! Constroi um novo usuário inicializado com dados recebidos do módulo de análises
UserAnalysis::UserAnalysis(const wsUser& data)
{
  assert(!data.login.empty());
  _data = data;
}

//! Destrutor
UserAnalysis::~UserAnalysis()
{
}

std::vector<wsUserView>& UserAnalysis::listUserViewsbyUser()
{
	std::vector<wsUserView>& userViews = _data.userViews;
	return userViews;
}
