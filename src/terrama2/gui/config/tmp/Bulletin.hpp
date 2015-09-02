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
 * \file Bulletin.hpp
 *
 * \brief Definicao da classe Bulletin
 *
 * \author Raphael Meloni
 * \date september, 2011 
 * \version $Id$
 *
 */

#ifndef _BULLETIN_H_
#define _BULLETIN_H_

// TerraMA2
#include "soapModNotificacaoProxy.h"

// QT
#include <QString>

//! Classe responsável por representar um boletins e as analises e usuários associados
class Bulletin
{
public:
  Bulletin();
  Bulletin(const wsBulletin& data);
  ~Bulletin();

  //! Retorna o identificador do boletim
  int identifier() const { return _data.BulletinId; }

  //! Atribui um identificador para o boletim
  void setIdentifier(int BulletinId) {  _data.BulletinId = BulletinId;  }

  //! Retorna o nome do boletim
  QString name() const { return QString::fromStdString(_data.BulletinName); }

  //! Atribui um nome para o boletim
  void setName(QString nameBulletin) {  _data.BulletinName = nameBulletin.toStdString();  }

  //! Retorna a descricao do boletim
  QString description() const { return QString::fromStdString(_data.BulletinDesc); }

  //! Atribui uma descrição para o boletim
  void setDescription(QString BulletinDesc) {  _data.BulletinDesc = BulletinDesc.toStdString();  }

  //! Retorna a lista dos analises
  std::vector<struct wsAnalysisRule>& listAnalysisRulebyBulletin();

  //! Altera a lista de users
  void setAnalysisRuleByBulletin(const std::vector<struct wsAnalysisRule>& analysisRule) { _data.analysisRules.clear(); _data.analysisRules = analysisRule; }

  //! Retorna a lista dos usuários
  std::vector<struct wsUser>& listUserbyBulletin();

  //! Altera a lista de users
  void setUserByBulletin(const std::vector<struct wsUser>& users) { _data.users.clear(); _data.users = users; }
  
  //! Retorna a estrutura que empacota os dados do boletim
  const wsBulletin& data() const { return _data; }

private:
  wsBulletin _data;  //! Dados do mapa empacotados na estrutura lida / recebida do módulo de notificação
};


#endif

