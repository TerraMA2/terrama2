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
  \file terrama2/gui/config/RiskMapList.hpp

  \brief Definition of methods in class RiskMapList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class RiskMapList

É a classe responsável por efetuar a comunicação com o
módulo de análises.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de mapas devem ser feitas através da 
API própria para garantir a sincronização com o servidor.
*/

// STL
#include <assert.h>

// TerraMA2
#include "RiskMapList.hpp"
#include "Services.hpp"

//! Construtor
RiskMapList::RiskMapList(Services* manager, ServPlanos* service)
{
  assert(manager && service);
  _manager = manager;
  _service = service;
}

//! Destrutor
RiskMapList::~RiskMapList()
{
}


#ifdef OFFLINE_TEST
  static wsDBColumn attrTestDB1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  static wsDBColumn attrTestDB2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};

  static wsProjectionParams projPar1 =	{ WS_PROJNAME_LatLong, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =	{ WS_PROJNAME_UTM, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar3 =	{ WS_PROJNAME_CylindricalEquidistant, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar4 =	{ WS_PROJNAME_LatLong, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  std::vector<wsDBColumn> vet1(attrTestDB1, attrTestDB1 + sizeof(attrTestDB1)/sizeof(wsDBColumn));
  std::vector<wsDBColumn> vet2(attrTestDB2, attrTestDB2 + sizeof(attrTestDB2)/sizeof(wsDBColumn));

  static wsRiskMapTheme baseRMT[] =	{ { 0, "RiskMapTheme 1", projPar1, vet1},
                                      { 1, "RiskMapTheme 2", projPar2, vet2},
                                      { 2, "RiskMapTheme 3", projPar3, vet2},
                                      { 3, "RiskMapTheme 4", projPar4, vet1},
                                      { 4, "RiskMapTheme 5", projPar1, vet1}
                                    };
#endif


/*! \brief Carrega lista remota de temas possiveis para objetos monitorados do módulo de análise

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool RiskMapList::loadRiskMapThemeData()
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  std::vector<wsRiskMapTheme> result(baseRMT, baseRMT + sizeof(baseRMT)/sizeof(wsRiskMapTheme));
#else
  std::vector<struct wsRiskMapTheme> result;
  if (_service->layer__getEligibleThemesForNewRiskMap(result) != SOAP_OK)
      return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de temas disponíveis \npara objetos monitorados."));
#endif

  _riskMapThemesEligible = result;

  return true;
}

/*! \brief Carrega lista remota de objetos monitorados do módulo de análise

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool RiskMapList::loadData()
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  static wsRiskMap testData[] = {
    {0, baseRMT[0], "Mapa 1", "Autor 1", "Inst 1", 1999, 1, 11, 2000, 1, 21, "Detalhes 1"},
    {1, baseRMT[1], "Mapa 2", "Autor 2", "Inst 2", 2000, 2, 12, 2001, 2, 22, "Detalhes 2"},
    {2, baseRMT[2], "Mapa 3", "Autor 3", "Inst 3", 2001, 3, 13, 2002, 3, 23, "Detalhes 3"},
    {3, baseRMT[3], "Mapa 4", "Autor 4", "Inst 4", 2002, 4, 14, 2003, 4, 24, "Detalhes 4"},
    {4, baseRMT[4], "Mapa 5", "Autor 5", "Inst 5", 2003, 5, 15, 2004, 5, 25, "Detalhes 5"}
  };
  std::vector<wsRiskMap> result(testData, testData + sizeof(testData)/sizeof(wsRiskMap));
#else
  std::vector<struct wsRiskMap> result;
  if(_service->layer__getRiskMapList(result) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível carregar a lista de objetos monitorados do módulo de análise."));
#endif

  // Cria objetos do tipo riskMap para cada mapa retornado
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    RiskMap* map = new RiskMap(result[i]);
    append(map);
  }
  return true;
}

/*! \brief Adiciona um novo objeto monitorado ao conjunto remoto de mapas

Retorna true se conseguiu adicionar o novo objeto monitorado.  Caso contrário,
MOSTRA mensagem de erro e retorna false. 

Se a inserção remota foi bem sucedida, inclui na lista uma CÓPIA do objeto monitorado
recebido, com seu identificador do servidor atualizado para refletir o 
identificador remoto
*/
bool RiskMapList::addNewRiskMap(const RiskMap* rm)
{
	int newId;

#ifdef OFFLINE_TEST
  static int id_generator = 1000;
  newId = id_generator++;
#else
//	int themeId; // Ver de onde tirar...
  const wsRiskMap& data = rm->data();
  //if(_service->analysis__newRiskMap(data, newId) != SOAP_OK)
  if (_service->layer__newRiskMap(data.baseRiskMapTheme.id,
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
									 data.attrProperties,
									 data.nameAttr,
	                                 newId) != SOAP_OK)
      return _manager->showModServPlanosError(QObject::tr("Não foi possível adicionar o objeto monitorado."));
#endif

  // objeto monitorado salvo remotamente com sucesso.  Atualiza lista de objetos monitorados
  RiskMap* newRm = new RiskMap;
  *newRm = *rm;
  newRm->setId(newId);

  append(newRm);

  emit afterInsertRiskMapList(count()-1, rm->data());

  return true;
}

/*! \brief Atualiza os dados remotos de um objeto monitorado, identificado por seu
           índice na lista

Retorna true se conseguiu alterar o objeto monitorado.  Caso contrário,
MOSTRA mensagem de erro e retorna false. Se a atualização remota
foi bem sucedida, copia dados salvos para a lista. 

\param id identificador do objeto monitorado
\param rm   Dados a serem salvos
*/
bool RiskMapList::updateRiskMap(int id, const RiskMap* rm)
{
  RiskMap* oldRm = findMap(id);
  int index = findMapIndex(id);
  assert(oldRm->id() == rm->id());
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  const wsRiskMap& data = rm->data();
  if (_service->layer__editRiskMap(  data.id,
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
									 data.attrProperties,
									 data.nameAttr,
	                                 dummy) != SOAP_OK)
      return _manager->showModServPlanosError(QObject::tr("Não foi possível atualizar os dados do objeto monitorado."));
#endif

  // objeto monitorado salvo remotamente com sucesso.  Atualiza lista de objetos monitorados
  *oldRm = *rm;

  emit afterUpdateRiskMapList(index, rm->data());

  return true;
}

/*void RiskMapList::beforeDeleteRiskMapList(int id, int index)
{
}*/

/*! \brief Remove um objeto monitorado, identificado por sua
           chave (id) no banco

Retorna true se conseguiu remover o objeto monitorado.  Caso contrário,
MOSTRA mensagem de erro e retorna false. 
*/
bool RiskMapList::deleteRiskMap(int id)
{
  RiskMap* rm = findMap(id);
  int index = findMapIndex(id);
  
#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  if(_service->layer__deleteRiskMap(id, dummy) != SOAP_OK)
    return _manager->showModServPlanosError(QObject::tr("Não foi possível remover o objeto monitorado."));
#endif

  emit beforeDeleteRiskMapList(index, rm->data());

  // Conseguimos apagar o objeto monitorado do servidor remoto.  Remove da lista
  removeAt(index);
  delete rm;


  return true;
}

/*! \brief Retorna o índice do objeto monitorado associado com um identificador

Retorna -1 se o identificador não tiver sido encontrado
*/
int RiskMapList::findMapIndex(int mapId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == mapId)
      return i;
  }
  return -1;
}

/*! \brief Retorna o objeto monitorado associado com um identificador

Retorna NULL se o identificador não tiver sido encontrado
*/
RiskMap* RiskMapList::findMap(int mapId) const
{
  int index = findMapIndex(mapId);
  return (index != -1) ? at(index) : NULL;
}

