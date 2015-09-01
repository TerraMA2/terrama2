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
  \file terrama2/gui/admin/PreviewCustom.cpp

  \brief Definition of ServiceManager Class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>
#include <memory>

// TerraMA2
#include "ServiceManager.hpp"

// SOAP
#include "ModAnalise.nsmap" // Apenas um dos nsmap's deve ser incluido...  
#include "soapModAnaliseProxy.h"
#include "soapModColetaProxy.h"
#include "soapModNotificacaoProxy.h"
#include "soapServPlanosProxy.h"
#include "soapServAnimacaoProxy.h"


/*! Efetua um "ping" do serviço especificado em type.

\param config Parâmetros de configuração com endereços dos serviços a serem pingados
\param type   Tipo do serviço a ser pingado.  
              Valores (seguindo ordem na tabela na interface de servicesDlg):
              0 - Coleta
              1 - Planos
              2 - Notificacao
			  3 - Animacao
              4 - Analise (principal)
              5..n - Analises auxiliares
*/                
bool ServiceManager::ping(const ConfigData& config, int type)
{
  QString     addr;
  QByteArray  addrstr;
  std::string answer;
  
  if(type == 0) // Coleta
  {
    std::auto_ptr<ModColeta> mod(new ModColeta);
    addr = config.collection()._address + ":" + QString::number(config.collection()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->collection__ping(answer) != SOAP_OK)
      return false;
  }
  else if(type == 1) // Planos
  {
    std::auto_ptr<ServPlanos> mod(new ServPlanos);
    addr = config.plans()._address + ":" + QString::number(config.plans()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->layer__ping(answer) != SOAP_OK)
      return false;
  }
  else if(type == 2) // Notificacao
  {
    std::auto_ptr<ModNotificacao> mod(new ModNotificacao);
    addr = config.notification()._address + ":" + QString::number(config.notification()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->notification__ping(answer) != SOAP_OK)
      return false;
  }
  else if(type == 3) //Animacao
  {
	  std::auto_ptr<ServAnimacao> mod(new ServAnimacao);
	  addr = config.animation()._address + ":" + QString::number(config.animation()._servicePort);
	  mod->endpoint = (addrstr = addr.toLatin1()).data();
	  if(mod->animacao__ping(answer) != SOAP_OK)
		  return false;
  }
  else if(type == 4) //Analise (principal)
  {
    std::auto_ptr<ModAnalise> mod(new ModAnalise);
    addr = config.analysis()._address + ":" + QString::number(config.analysis()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->analysis__ping(answer) != SOAP_OK)
      return false;
  }
  else // Analises auxiliares
  {
    std::auto_ptr<ModAnalise> mod(new ModAnalise);
    addr = config.analysis()._instanceData.at(type-5)._address + ":" + QString::number(config.analysis()._instanceData.at(type-5)._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->analysis__ping(answer) != SOAP_OK)
      return false;
  }
  
  if(answer != "Sismaden pong")
    return false;  
  return true;
}

/*! Efetua o envio de solicitação de finalização do serviço especificado em type.

\param config Parâmetros de configuração com endereços dos serviços a serem finalizados
\param type   Tipo do serviço a ser finalizado.  
              Valores (seguindo ordem na tabela na interface de servicesDlg):
              0 - Coleta
              1 - Planos
              2 - Notificacao
              3 - Analise (principal)
              4..n - Analises auxiliares
*/                
bool ServiceManager::close(const ConfigData& config, int type)
{
  QString     addr;
  QByteArray  addrstr;
  
  if(type == 0) // Coleta
  {
    std::auto_ptr<ModColeta> mod(new ModColeta);
    addr = config.collection()._address + ":" + QString::number(config.collection()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->collection__shutdownCollectionModule() != SOAP_OK)
      return false;
  }
  else if(type == 1) // Planos
  {
    std::auto_ptr<ServPlanos> mod(new ServPlanos);
    addr = config.plans()._address + ":" + QString::number(config.plans()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->layer__shutdownLayerService() != SOAP_OK)
      return false;
  }
  else if(type == 2) // Notificacao
  {
    std::auto_ptr<ModNotificacao> mod(new ModNotificacao);
    addr = config.notification()._address + ":" + QString::number(config.notification()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->notification__shutdownNotificationModule() != SOAP_OK)
      return false;
  }
  else if(type == 3) // Animcacao
  {
	  std::auto_ptr<ServAnimacao> mod(new ServAnimacao);
	  addr = config.animation()._address + ":" + QString::number(config.animation()._servicePort);
	  mod->endpoint = (addrstr = addr.toLatin1()).data();
	  if(mod->animacao__shutdownAnimacaoService() != SOAP_OK)
		  return false;
  }
  else if(type == 4) //Analise (principal)
  {
    std::auto_ptr<ModAnalise> mod(new ModAnalise);
    addr = config.analysis()._address + ":" + QString::number(config.analysis()._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->analysis__shutdownAnalysisModule() != SOAP_OK)
      return false;
  }
  else // Analises auxiliares
  {
    std::auto_ptr<ModAnalise> mod(new ModAnalise);
    addr = config.analysis()._instanceData.at(type-5)._address + ":" + QString::number(config.analysis()._instanceData.at(type-5)._servicePort);
    mod->endpoint = (addrstr = addr.toLatin1()).data();
    if(mod->analysis__shutdownAnalysisModule() != SOAP_OK)
      return false;
  }
  return true;
}
