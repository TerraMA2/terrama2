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
 * \file Bulletin.cpp
 *
 * \brief Definicao dos metodos declarados na classe Bulletin
 * \author Raphael Meloni
 * \date september, 2011 
 * \version $Id
 *
 */

/*!
\class Bulletin

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de notificação

*/

// TerraMA2
#include "Bulletin.hpp"

// STL
#include <assert.h>


//! Construtor para novos boletins, ainda não salvos na base de dados
Bulletin::Bulletin()
{
  _data.BulletinId		= -1;
  _data.BulletinName	= "";
  _data.BulletinDesc	= "";  
}

//! Constroi um novo boletim inicializado com dados recebidos do módulo de notificacao
Bulletin::Bulletin(const wsBulletin& data)
{
  assert(!data.BulletinName.empty());
  _data = data;
}

//! Destrutor
Bulletin::~Bulletin()
{
}

std::vector<wsUser>& Bulletin::listUserbyBulletin()
{
	std::vector<wsUser>& users = _data.users;
	return users;
}

std::vector<wsAnalysisRule>& Bulletin::listAnalysisRulebyBulletin()
{
	std::vector<wsAnalysisRule>& analysisRules = _data.analysisRules;
	return analysisRules;
}
