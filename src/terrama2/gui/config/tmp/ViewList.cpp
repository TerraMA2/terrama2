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
  \file terrama2/gui/config/ViewList.cpp

  \brief Definition of methods in class ViewList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Frederic Dartayre
*/

/*!
\class ViewList

É a classe responsável por efetuar a comunicação com o
módulo de planos.

Herda de QList para facilitar a consulta aos dados, porém
adição e remoção de mapas devem ser feitas através da
API própria para garantir a sincronização com o servidor.
*/

// QT
#include <assert.h>

// TerraMA2
#include "ViewList.hpp"
#include "Services.hpp"

ViewList::ViewList(Services* manager, ServPlanos* service)
{
	assert(manager && service);
  _manager = manager;
  _service = service;
}

//! Destrutor
ViewList::~ViewList()
{
}

/*! \brief Carrega lista remota de vistas do módulo de planos

Retorna true se conseguiu carregar a lista.  Caso contrário,
MOSTRA mensagem de erro e retorna false.

*/
bool ViewList::loadData()
{

  std::vector<struct wsView> result;
  if(_service->layer__getViewList(result) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de vistas do módulo de planos."));

  // Cria objetos do tipo riskMap para cada mapa retornado
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    View* view = new View(result[i]);
    append(view);
  }
  return true;
}

/*! \brief Retorna o índice da visa associada com um identificador

Retorna -1 se o identificador não tiver sido encontrado
*/
int ViewList::findViewIndex(int viewId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == viewId)
      return i;
  }
  return -1;
}

/*! \brief Retorna a vista associada com um identificador

Retorna NULL se o identificador não tiver sido encontrado
*/
View* ViewList::findView(int viewId) const
{
  int index = findViewIndex(viewId);
  return (index != -1) ? at(index) : NULL;
}
