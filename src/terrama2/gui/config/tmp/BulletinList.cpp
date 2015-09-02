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
 * \file BulletinList.cpp
 *
 * \brief Definicao dos metodos declarados na classe BulletinList
 * \author Raphael Meloni
 * \date september, 2011 
 * \version $Id
 *
 */

/*!
\class BulletinList

É a classe responsável por efetuar a comunicação com o
módulo de notificações.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de mapas devem ser feitas através da 
API própria para garantir a sincronização com o servidor.
*/

// TerraMA2
#include "BulletinList.hpp"
#include "soapModNotificacaoProxy.h"
#include "Services.hpp"

// STL
#include <assert.h>


//! Construtor
BulletinList::BulletinList(Services* manager, ModNotificacao* service)
{
  assert(manager && service);
  _manager = manager;
  _service = service;
}

//! Destrutor
BulletinList::~BulletinList()
{
}


#ifdef OFFLINE_TEST
/*RM
  static wsDBColumn attrTestData1[] = {{"Id", WS_COLTYPE_INT},
                                       {"ClasseSuc", WS_COLTYPE_REAL},
                                       {"Limiar_CP", WS_COLTYPE_REAL},
                                       {"Limiar_CA", WS_COLTYPE_REAL}};
  std::vector<wsDBColumn> vetDBCol1(attrTestData1, attrTestData1 + sizeof(attrTestData1)/sizeof(wsDBColumn));
  static wsDBColumn attrTestData2[] = {{"Id", WS_COLTYPE_INT},
                                       {"attr1", WS_COLTYPE_REAL},
                                       {"attr2", WS_COLTYPE_REAL},
                                       {"attr3", WS_COLTYPE_STRING},
                                       {"attr4", WS_COLTYPE_STRING},
                                       {"attr5", WS_COLTYPE_DATETIME}};
  std::vector<wsDBColumn> vetDBCol2(attrTestData2, attrTestData2 + sizeof(attrTestData2)/sizeof(wsDBColumn));

  static wsProjectionParams projPar1 =	{ WS_PROJNAME_Mercator, WS_DATUM_CorregoAlegre, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units1", 0.0, WS_HEMISPHERE_SOUTH};
  static wsProjectionParams projPar2 =	{ WS_PROJNAME_Sinusoidal, WS_DATUM_SAD69, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "Units2", 0.0, WS_HEMISPHERE_SOUTH};

  static wsRiskMapTheme baseRMT1 =	{ 0, "RiskMapTheme 1", projPar1, vetDBCol1};
  static wsRiskMapTheme baseRMT2 =	{ 0, "RiskMapTheme 2", projPar2, vetDBCol2};

  static wsRiskMap riskMap1 = {0, baseRMT1, "Mapa 1", "Autor 1", "Inst 1", 1999, 1, 11, 2000, 1, 21, "Detalhes 1"};
  static wsRiskMap riskMap2 = {1, baseRMT2, "Mapa 2", "Autor 2", "Inst 2", 2000, 2, 12, 2001, 2, 22, "Detalhes 2"};
  static wsRiskMap riskMap3 = {2, baseRMT1, "Mapa 3", "Autor 3", "Inst 3", 2001, 3, 13, 2002, 3, 23, "Detalhes 3"};

  static wsPCD pcd_colPts[] = { { "filename1.txt", -47.1021, -23.1231  },
                                { "filename2.txt", -46.7021, -21.2411  },
                                { "filename3.txt", -45.1351, -24.3251  },
                                { "filename4.txt", -47.2441, -23.2337  } };
  std::vector<wsPCD> vectpcd_colPts(pcd_colPts, pcd_colPts + sizeof(pcd_colPts)/sizeof(wsPCD));

  static wsWeatherDataSource testGridData1[] = {
    {0, 1, WS_WDSGEOM_RASTER, "Grid Clima 1", WS_WDSTYPE_PREDICTED_RAIN,   {}, 10.0, 1.0, "Details 1", "Path 1", "Mask 1", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav01.nav", "cltfile01.ctl", WS_GRADSDATATYPE_INTEGER16, 1},
    {1, 1, WS_WDSGEOM_POINTS, "Grid Clima 2", WS_WDSTYPE_ACCUMULATED_RAIN, {}, 11.0, 2.0, "Details 2", "Path 2", "Mask 2", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script02.lua", "nav02.nav", "cltfile02.ctl", WS_GRADSDATATYPE_FLOAT32,   2},
    {2, 0, WS_WDSGEOM_RASTER, "Grid Clima 3", WS_WDSTYPE_OTHER,            {}, 12.0, 3.0, "Details 3", "Path 3", "Mask 3", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script03.lua", "nav03.nav", "cltfile03.ctl", WS_GRADSDATATYPE_INTEGER16, 3},
    {3, 0, WS_WDSGEOM_POINTS, "Grid Clima 4", WS_WDSTYPE_PREDICTED_RAIN,   {}, 13.0, 4.0, "Details 4", "Path 4", "Mask 4", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script04.lua", "nav04.nav", "cltfile04.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
  };
  std::vector<wsWeatherDataSource> vet1a(testGridData1, testGridData1 + sizeof(testGridData1)/sizeof(wsWeatherDataSource));

  static wsWeatherDataSource testGridData2[] = {
    {4, 1, WS_WDSGEOM_RASTER, "Grid Clima 5", WS_WDSTYPE_PREDICTED_RAIN,   {}, 10.0, 1.0, "Details 1", "Path 1", "Mask 1", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script05.lua", "nav05.nav", "cltfile05.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
    {5, 1, WS_WDSGEOM_RASTER, "Grid Clima 6", WS_WDSTYPE_ACCUMULATED_RAIN, {}, 11.0, 2.0, "Details 2", "Path 2", "Mask 2", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script06.lua", "nav06.nav", "cltfile06.ctl", WS_GRADSDATATYPE_INTEGER16, 2},
    {6, 0, WS_WDSGEOM_POINTS, "Grid Clima 7", WS_WDSTYPE_OTHER,            {}, 12.0, 3.0, "Details 3", "Path 3", "Mask 3", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script07.lua", "nav07.nav", "cltfile07.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
    {7, 0, WS_WDSGEOM_POINTS, "Grid Clima 8", WS_WDSTYPE_PREDICTED_RAIN,   {}, 13.0, 4.0, "Details 4", "Path 4", "Mask 4", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script08.lua", "nav08.nav", "cltfile08.ctl", WS_GRADSDATATYPE_INTEGER16, 2},
  };
  std::vector<wsWeatherDataSource> vet1b(testGridData2, testGridData2 + sizeof(testGridData2)/sizeof(wsWeatherDataSource));

  static wsTheme baseTheme1 = { 0, "BaseTheme 1", projPar1, vetDBCol2, 	WS_LAYERGEOM_RASTER, 3, 3, 0.0, 0.0 };
  static wsTheme baseTheme2 = { 1, "BaseTheme 2", projPar2, vetDBCol1, 	WS_LAYERGEOM_POLYGONS, 3, 3, 0.0, 0.0 };

  static wsAdditionalMap adtMap1 = { 1, baseTheme1, "AdtMap 1", "Author 1", "Inst 1", 0,0,0, 0,0,0, "Dets 1"};
  static wsAdditionalMap adtMap2 = { 2, baseTheme2, "AdtMap 2", "Author 2", "Inst 2", 0,0,0, 0,0,0, "Dets 2"};
  static wsAdditionalMap adtMap3 = { 3, baseTheme1, "AdtMap 3", "Author 3", "Inst 3", 0,0,0, 0,0,0, "Dets 3"};
  static wsAdditionalMap adtMap4 = { 4, baseTheme2, "AdtMap 4", "Author 4", "Inst 4", 0,0,0, 0,0,0, "Dets 4"};

  struct wsAddMapDisplayConfig admdcMap1[] = 
                                   { { adtMap1, WS_STATUS_VISIBLE_IF_WARNING}, { adtMap2, WS_STATUS_NOT_VISIBLE}, 
	                                 { adtMap3, WS_STATUS_VISIBLE_IF_WARNING}, { adtMap4, WS_STATUS_NOT_VISIBLE},
                                   };
  std::vector<wsAddMapDisplayConfig> vet2(admdcMap1, admdcMap1 + sizeof(admdcMap1)/sizeof(wsAddMapDisplayConfig));

  static wsAnalysisRule testData[] = {
	{0, WS_ANALYSISTYPE_RISK,  "Teste 1", "Autor 1", "Inst 1", "Detalhes 1", riskMap2, vet2, vet1a, "111", "Script1"},
    {1, WS_ANALYSISTYPE_RISK,  "Teste 2", "Autor 2", "Inst 2", "Detalhes 2", riskMap2, vet2, vet1b, "222", "Script2"},
    {2, WS_ANALYSISTYPE_MODEL,  "Teste 3", "Autor 3", "Inst 3", "Detalhes 3", riskMap3, vet2, vet1a, "Output3", "Script3"}
  };
  std::vector<wsAnalysisRule> analysisRules(testData, testData + sizeof(testData)/sizeof(wsAnalysisRule));

  struct wsUser usersData[] = { { "fulano", "qquma", "Fulano da Silva", "fulanodasilva@dominio.com", "99991111", WS_CANVASSIZE_SMALL, analysisRules },
                                { "beltrano", "qquma", "Beltrano da Silva", "beltranodasilva@dominio.com", "99992222", WS_CANVASSIZE_MEDIUM, analysisRules },
                                { "ciclano", "qquma", "Ciclano da Silva", "ciclanodasilva@dominio.com", "88882222", WS_CANVASSIZE_LARGE, analysisRules }
                              };
*/
#endif


/*! \brief Carrega lista remota de boletins 

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool BulletinList::loadData()
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  //RM std::vector<wsUser> result(usersData, usersData + sizeof(usersData)/sizeof(wsUser));
#else
  std::vector<struct wsBulletin> result;
  if(_service->notification__getBulletinList(result) != SOAP_OK)
    return _manager->showModNotificacaoError(QObject::tr("Não foi possível carregar a lista de boletins."));
#endif

  // Cria objetos do tipo Bulletin para cada mapa retornado
  for(int i=0; i < (int)result.size(); i++)
  {
    Bulletin* bu = new Bulletin(result[i]);
    append(bu);
  }
  return true;
}

/*! \brief Adiciona um novo boletim ao conjunto remoto de boletins

Retorna true se conseguiu adicionar o novo Boletin.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

Se a inserção remota foi bem sucedida, inclui na lista uma CÓPIA do boletim
recebido
*/

bool BulletinList::addNewBulletin(const Bulletin* bu)
{
	#ifdef OFFLINE_TEST
	static int id_generator = 1000;
	static int newId = id_generator++;
	#else
	#ifndef OFFLINE_TEST
	int newId;
	const wsBulletin& data = bu->data();

	if (_service->notification__newBulletin(data.BulletinName,
											data.BulletinDesc,
											data.users,
											data.analysisRules,
											newId) != SOAP_OK)
		return _manager->showModNotificacaoError(QObject::tr("Não foi possível adicionar o boletim."));
	#endif
	#endif

  // Boletim salvo remotamente com sucesso.  Atualiza lista de boletins
  Bulletin* newBu = new Bulletin;
  *newBu = *bu;

  newBu->setIdentifier(newId);
  append(newBu);

  return true;
}

/*! \brief Atualiza os dados remotos de um boletim, identificado por seu 
           índice na lista

Retorna true se conseguiu alterar o boletim.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. Se a atualização remota
foi bem sucedida, copia dados salvos para a lista. 

\param bua   Dados a serem salvos
*/
bool BulletinList::updateBulletin(const Bulletin* bu)
{
  Bulletin* oldBu = findName(bu->name());
  assert(oldBu->name() == bu->name());
  
#ifdef OFFLINE_TEST
#else
  int dummy;
  const wsBulletin& data = bu->data();

  if (_service->notification__editBulletin( data.BulletinId,
											data.BulletinName,
											data.BulletinDesc,
											data.users,
											data.analysisRules,
											dummy) != SOAP_OK)
      return _manager->showModNotificacaoError(QObject::tr("Não foi possível atualizar os dados do boletim."));
#endif

//   Boletim salvo remotamente com sucesso.  Atualiza lista de boletins
  *oldBu = *bu;

  return true;
}

/*! \brief Remove um boletim, identificado por seu nome no banco

Retorna true se conseguiu remover o boletim.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool BulletinList::deleteBulletin(QString name)
{
  int index = findNameIndex(name);
   
  return deleteBulletinNameIndex(name, index);
}

/*! \brief Remove um boletim, identificado por seu indice na lista

Retorna true se conseguiu remover o boletim.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool BulletinList::deleteBulletinIndex(int index)
{
  QString name = at(index)->name();

  return deleteBulletinNameIndex(name, index);
}

/*! \brief Remove um boletim, identificado por seu indice na lista e nome no banco. 
           Esta rotina é chamada pela deleteBulletin e deleteBulletinIndex.

Retorna true se conseguiu remover o boletim.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 
*/
bool BulletinList::deleteBulletinNameIndex(QString name, int index)
{
  Bulletin* bu = findName(name);

#ifdef OFFLINE_TEST
#else
  int dummy = 0;
  if(_service->notification__deleteBulletin(name.toStdString(), dummy) != SOAP_OK)
    return _manager->showModNotificacaoError(QObject::tr("Não foi possível remover o boletim."));
#endif

  // Conseguimos apagar o boletim do servidor remoto.  Remove da lista
  removeAt(index);
  delete bu;

  return true;
}

/*! \brief Retorna o índice do boletim associado com um identificador

Retorna -1 se o identificador não tiver sido encontrado
*/
int BulletinList::findNameIndex(QString name) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->name() == name)
      return i;
  }
  return -1;
}

/*! \brief Retorna o boletim associado com um identificador

Retorna NULL se o identificador não tiver sido encontrado
*/
Bulletin* BulletinList::findName(QString name) const
{
  int index = findNameIndex(name);
  return (index != -1) ? at(index) : NULL;
}

