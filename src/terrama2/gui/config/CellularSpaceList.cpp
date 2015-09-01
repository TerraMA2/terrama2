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

� a classe respons�vel por efetuar a comunica��o com o m�dulo de an�lises.

Herda de QList para facilitar a consulta aos dados, por�m 
adi��o e remo��o de espa�os celulares devem ser feitas atrav�s 
da API pr�pria para garantir a sincroniza��o com o servidor.
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

/*! \brief Carrega lista remota de temas possiveis para espa�os celulares do m�dulo de an�lise  

Retorna true se conseguiu carregar a lista. Caso contr�rio, 
MOSTRA mensagem de erro e retorna false. 

*/
bool CellularSpaceList::loadCellThemeData()
{
	// Efetua chamada remota
	std::vector<struct wsTheme> result;
	if (_service->layer__getEligibleThemesForNewCellularSpace(result) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("N�o foi poss�vel carregar a lista de temas dispon�veis \npara espa�os celulares."));

	_cellThemesEligible = result;

	return true;
}

/*! \brief Carrega lista remota de espa�os celulares  

Retorna true se conseguiu carregar a lista. Caso contr�rio, 
MOSTRA mensagem de erro e retorna false. 

*/
bool CellularSpaceList::loadData()
{
	// Efetua chamada remota
	std::vector<struct wsCellularSpace> result;
	if(_service->layer__getCellularSpaceList(result) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("N�o foi poss�vel carregar a lista de espa�os celulares do m�dulo de an�lise."));

	// Cria objetos do tipo CellularSpace
	for(int i=0, count=(int)result.size(); i<count; i++)
	{
		CellularSpace* map = new CellularSpace(result[i]);
		append(map);
	}
	return true;
}

/*! \brief Adiciona um novo espa�o celular ao conjunto remoto

Retorna true se conseguiu adicionar o novo espa�o celular. Caso contr�rio, 
MOSTRA mensagem de erro e retorna false. 

Se a inser��o remota foi bem sucedida, inclui na lista uma C�PIA do espa�o celular
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
		return _manager->showModServPlanosError(QObject::tr("N�o foi poss�vel adicionar o Espa�o Celular."));

	// Espa�o celular salvo remotamente com sucesso.  Atualiza lista de espa�os celulares
	CellularSpace* newCs = new CellularSpace;
	*newCs = *cs;
	newCs->setId(newId);

	append(newCs);

	emit afterInsertCellularSpaceList(count()-1, cs->data());

	return true;
}

/*! \brief Atualiza os dados remotos de um espa�o celular, identificado por seu �ndice na lista

Retorna true se conseguiu alterar o espa�o celular. Caso contr�rio, 
MOSTRA mensagem de erro e retorna false. Se a atualiza��o remota
foi bem sucedida, copia dados salvos para a lista. 

\param id identificador do espa�o celular
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
		return _manager->showModServPlanosError(QObject::tr("N�o foi poss�vel atualizar os dados do espa�o celular."));

	// Espa�o celular salvo remotamente com sucesso.  Atualiza lista de espa�os celulares
	*oldCs = *cs;

	emit afterUpdateCellularSpaceList(index, cs->data());

	return true;
}

/*! \brief Remove um espa�o celular, identificado por sua chave (id) no banco

Retorna true se conseguiu remover o espa�o celular.  Caso contr�rio, 
MOSTRA mensagem de erro e retorna false. 
*/
bool CellularSpaceList::deleteCellularSpace(int id)
{
	CellularSpace* cs = findCellularSpace(id);
	int index = findCellularSpaceIndex(id);

	int dummy = 0;
	if(_service->layer__deleteCellularSpace(id, dummy) != SOAP_OK)
		return _manager->showModServPlanosError(QObject::tr("N�o foi poss�vel remover o espa�o celular."));

	emit beforeDeleteCellularSpaceList(index);

	// Conseguimos apagar o espa�o celular do servidor remoto.  Remove da lista
	removeAt(index);
	delete cs;

	return true;
}

/*! \brief Retorna o �ndice do espa�o celular associado com um identificador

Retorna -1 se o identificador n�o tiver sido encontrado
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

/*! \brief Retorna o espa�o celular associado com um identificador

Retorna NULL se o identificador n�o tiver sido encontrado
*/
CellularSpace* CellularSpaceList::findCellularSpace(int csId) const
{
	int index = findCellularSpaceIndex(csId);
	return (index != -1) ? at(index) : NULL;
}

