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
  \file terrama2/gui/config/AlertIconList.cpp

  \brief Definition of methods in class AlertIconList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred
*/

/*!
\class AlertIconList

This is the class responsible for make the communication with the analysis module.

*/

// TerraMA2
#include "AlertIconList.hpp"
#include "Services.hpp"

// STL
#include <assert.h>

AlertIconList::AlertIconList(Services* manager, ServPlanos* service)
{
  assert(manager && service);
  _manager      = manager;
  _service      = service;
}


/*! \brief Load the remote imagery set

Return true if it managed to load. Otherwise, display error message and then return false

*/
bool AlertIconList::loadData()
{
  std::vector<struct wsImage> result;
  if(_service->layer__getImages(result) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível recuperar o conjunto de imagens."));

  // Create objects of weatherGrid for each grid retrieved
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    AlertIcon* icon = new AlertIcon(result[i]);
    append(icon);
  }

  return true;
}

/*! \brief Add an image to the remote imagery set

Return true if it managed to do. Otherwise, display error message and return false

*/
bool AlertIconList::addIcon(uchar *cdata, int size, const QPixmap & pixmap, int & newImageID)
{
  wsImage data;

  data.id = -1;
  data.data.reserve(size);
  for(int i = 0; i < size; i++)
    data.data.push_back(cdata[i]);

  if(_service->layer__addImage(data.data, data.id) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível inserir a nova imagem."));

  assert(data.id != -1);

  AlertIcon *icon = new AlertIcon(data, pixmap);
  append(icon);

  newImageID = data.id;

  return true;
}


bool AlertIconList::deleteIcon(int index)
{
  AlertIcon* alertIcon = at(index);

  int dummy = 0;
  if(_service->layer__deleteImage(alertIcon->id(), dummy) != SOAP_OK)
    return _manager->showModAnaliseError(QObject::tr("Não foi possível remover a imagem."));

  removeAt(index);
  delete alertIcon;

  return true;
}

/*! \brief Return the icon index associated with an identifier.

Return -1 if the identifier has not been found

*/
int AlertIconList::findIconIndex(int iconId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == iconId)
      return i;
  }
  return -1;
}

/*! \brief Return the icon associated with an identifier

Return NULL if the identifier has not been found

*/
AlertIcon* AlertIconList::findIcon(int iconId) const
{
  int index = findIconIndex(iconId);
  return (index != -1) ? at(index) : NULL;
}
