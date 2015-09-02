/************************************************************************************
TerraMA² is a development platform of Monitoring, Analysis and Alert for
geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
tailings basins, epidemics and others.Copyright © 2012 INPE.

This code is part of the TerraMA² framework.
This framework is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The library provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements,
or modifications.

In no event shall INPE or K2 SISTEMAS be held liable to any party for direct, indirect,
special, incidental, or consequential damages arising out of the use of this
library and its documentation.
*************************************************************************************/

/*!
 * \file BulletinList.hpp
 *
 * \brief Definicao da classe BulletinList
 *
 * \author Raphael Meloni
 * \date september, 2011 
 * \version $Id$
 *
 */

#ifndef _BULLETIN_LIST_H_
#define _BULLETIN_LIST_H_

// TerraMA2
#include "Bulletin.hpp"

// QT
#include <QList>


class Services;
class ModNotificacao;

/*! \brief Classe responsável por manter e gerenciar a lista de boletins disponíveis
*/           
class BulletinList : public QList<Bulletin*>
{
//	Q_OBJECT

public:
  BulletinList(Services* manager, ModNotificacao* service);
  ~BulletinList();
  
  bool loadData();

  bool addNewBulletin(const Bulletin* bu);
  bool updateBulletin(const Bulletin* bu); 
  bool deleteBulletin(QString name);
  bool deleteBulletinIndex(int index);

  int      findNameIndex(QString name) const;
  Bulletin* findName(QString name) const;

private:
  BulletinList(const BulletinList&);             //!< Remove construtor de cópia default
  BulletinList& operator=(const BulletinList&);  //!< Remove operador de atrib. default
  bool deleteBulletinNameIndex(QString login, int index);

  Services*   _manager; //! Gerenciador de serviços
  ModNotificacao* _service; //! Referência para acesso ao serviço remoto de Notificação
};


#endif

