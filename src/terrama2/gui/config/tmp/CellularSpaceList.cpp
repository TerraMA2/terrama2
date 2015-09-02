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
  \file terrama2/gui/config/CellularSpaceList.cpp

  \brief Definition of methods in class CellularSpaceList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Gustavo Sampaio
*/

/*!
\class CellularSpaceList

É a classe responsável por efetuar a comunicação com o módulo de análises.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de espaços celulares devem ser feitas através 
da API própria para garantir a sincronização com o servidor.
*/

// STL
#include <assert.h>

// TerraMA2
#include "CellularSpaceList.hpp"
#include "Services.hpp"

//! Construtor
CellularSpaceList::CellularSpaceList(Services* manager, ServPlanos* service)
{
	assert(manager && service);
	_manager = manager;
	_service = service;
}

//! Destrutor
CellularSpaceList::~CellularSpaceList()
{
}

/*! \brief Carrega lista remota de temas possiveis para espaços celulares do módulo de análise  

Retorna true se conseguiu carregar a lista. Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool CellularSpaceList::loadCellThemeData()
{
	// Efetua chamada remota
	std::vector<struct wsTheme> result;
	if (_service->layer__getEligibleThemesForNewCellularSpace(result) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de temas disponíveis \npara espaços celulares."));

	_cellThemesEligible = result;

	return true;
}

/*! \brief Carrega lista remota de espaços celulares  

Retorna true se conseguiu carregar a lista. Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool CellularSpaceList::loadData()
{
	// Efetua chamada remota
	std::vector<struct wsCellularSpace> result;
	if(_service->layer__getCellularSpaceList(result) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de espaços celulares do módulo de análise."));

	// Cria objetos do tipo CellularSpace
	for(int i=0, count=(int)result.size(); i<count; i++)
	{
		CellularSpace* map = new CellularSpace(result[i]);
		append(map);
	}
	return true;
}

/*! \brief Adiciona um novo espaço celular ao conjunto remoto

Retorna true se conseguiu adicionar o novo espaço celular. Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

Se a inserção remota foi bem sucedida, inclui na lista uma CÓPIA do espaço celular
recebido, com seu identificador do servidor atualizado para refletir o 
identificador remoto
*/
bool CellularSpaceList::addNewCellularSpace(const CellularSpace* cs)
{
	int newId;

	const wsCellularSpace& data = cs->data();
	if (_service->layer__newCellularSpace(data.baseTheme.id,
										  data.name,
										  data.author,
										  data.institution,
										  data.creationYear,
										  data.creationMonth,
										  data.creationDay,
										  data.expirationYear,
										  data.expirationMonth,
										  data.expirationDay,
										  data.details,
										  newId) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("Não foi possível adicionar o Espaço Celular."));

	// Espaço celular salvo remotamente com sucesso.  Atualiza lista de espaços celulares
	CellularSpace* newCs = new CellularSpace;
	*newCs = *cs;
	newCs->setId(newId);

	append(newCs);

	emit afterInsertCellularSpaceList(count()-1, cs->data());

	return true;
}

/*! \brief Atualiza os dados remotos de um espaço celular, identificado por seu índice na lista

Retorna true se conseguiu alterar o espaço celular. Caso contrário, 
MOSTRA mensagem de erro e retorna false. Se a atualização remota
foi bem sucedida, copia dados salvos para a lista. 

\param id identificador do espaço celular
\param cs Dados a serem salvos
*/
bool CellularSpaceList::updateCellularSpace(int id, const CellularSpace* cs)
{
	CellularSpace* oldCs = findCellularSpace(id);
	int index = findCellularSpaceIndex(id);
	assert(oldCs->id() == cs->id());

	int dummy = 0;
	const wsCellularSpace& data = cs->data();
	if (_service->layer__editCellularSpace( data.id,
											data.name,
											data.author,
											data.institution,
											data.creationYear,
											data.creationMonth,
											data.creationDay,
											data.expirationYear,
											data.expirationMonth,
											data.expirationDay,
											data.details,
											dummy) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("Não foi possível atualizar os dados do espaço celular."));

	// Espaço celular salvo remotamente com sucesso.  Atualiza lista de espaços celulares
	*oldCs = *cs;

	emit afterUpdateCellularSpaceList(index, cs->data());

	return true;
}

/*! \brief Remove um espaço celular, identificado por sua chave (id) no banco

Retorna true se conseguiu remover o espaço celular.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool CellularSpaceList::deleteCellularSpace(int id)
{
	CellularSpace* cs = findCellularSpace(id);
	int index = findCellularSpaceIndex(id);

	int dummy = 0;
	if(_service->layer__deleteCellularSpace(id, dummy) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("Não foi possível remover o espaço celular."));

	emit beforeDeleteCellularSpaceList(index);

	// Conseguimos apagar o espaço celular do servidor remoto.  Remove da lista
	removeAt(index);
	delete cs;

	return true;
}

/*! \brief Retorna o índice do espaço celular associado com um identificador

Retorna -1 se o identificador não tiver sido encontrado
*/
int CellularSpaceList::findCellularSpaceIndex(int csId) const
{
	for(int i=0, num=(int)count(); i<num; i++)
	{
		if(at(i)->id() == csId)
			return i;
	}
	return -1;
}

/*! \brief Retorna o espaço celular associado com um identificador

Retorna NULL se o identificador não tiver sido encontrado
*/
CellularSpace* CellularSpaceList::findCellularSpace(int csId) const
{
	int index = findCellularSpaceIndex(csId);
	return (index != -1) ? at(index) : NULL;
}

