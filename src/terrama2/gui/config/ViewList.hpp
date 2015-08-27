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
  \file terrama2/gui/config/ViewList.hpp

  \brief Definition of methods in class ViewList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Frederic Dartayre
*/

#ifndef VIEWLIST_H
#define VIEWLIST_H

#include <QList>
#include <QObject>

#include "View.hpp"

class Services;
class ServPlanos;

/*! \brief Classe responsável por manter e gerenciar a lista de vistas
 *         disponíveis
*/
class ViewList : public QObject, public QList<View*>
{
	Q_OBJECT

public:
  ViewList(Services* manager, ServPlanos* service);
  ~ViewList();

  bool  loadData();
	int   findViewIndex(int viewId) const;
  View* findView(int viewId) const;

private:
  ViewList(const ViewList&);             //!< Remove construtor de cópia default
  ViewList& operator=(const ViewList&);  //!< Remove operador de atrib. default

  Services*   _manager; //! Gerenciador de serviços
  ServPlanos* _service; //! Referência para acesso ao serviço remoto de Planos
};

#endif // VIEWLIST_H
