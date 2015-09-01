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
  \file terrama2/gui/config/MainDialogTab.cpp

  \brief Definition of methods in class MainDialogTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class MainDialogTab

Al�m da defini��o da interface, inclui ainda alguns m�todos auxiliares
comuns a todas as fichas.
*/

// STL
#include <assert.h>

// QT
#include <QMessageBox>

// TerraMA2
#include "MainDialogTab.hpp"

/*! \brief Salva dados correntes, se v�lidos  
  
  Verifica se os dados s�o v�lidos.  Se n�o forem, mostra mensagem
  de erro e retorna false.  Se forem, tenta salvar.  Em caso de 
  erro, mostra mensagem e retorna false. Se a opera��o de salvar
  foi bem sucedida, retorna true.
*/
bool MainDialogTab::validateAndSaveChanges()
{
  QString err = "";
  
  // Valida dados
  if(!validate(err))
  {
    // Dados invalidos. Mostra mensagem de erro (a n�o ser que err esteja vazio)
    if(!err.isEmpty())
      QMessageBox::warning(_parent, tr("Erro validando dados..."), err);
    return false;
  }
  
  // Tenta salvar...
  if(!save())
  {
    // Se n�o conseguiu salvar, mostra mensagem de erro
    QMessageBox::warning(_parent, tr("Erro salvando dados..."), 
                         tr("Erro salvando dados.  Opera��o cancelada."));
    return false;
  }
  return true;
}

/*! \brief Verifica se os dados atuais foram alterados e 
           viabiliza (ou n�o) troca de acordo com op��o do usu�rio
           
Se os dados estiverem atualizados, retorna true.  Se n�o estiverem, 
pergunta ao usu�rio se deseja salvar as altera��es, descartar as 
altera��es ou cancelar a opera��o.

Processa a decis�o do usu�rio.  Se uma tentativa de salvar os dados
for falha, cancela a opera��o.

\param restore Indica se os dados na tela devem ou n�o ser restaurados 
               se o usu�rio descartar as altera��es.
\return O retorno da fun��o indica se a opera��o de troca dos dados visualizados 
        deve (retorno true) ou n�o prosseguir (retorno false). 
*/
bool MainDialogTab::verifyAndEnableChange(bool restore)
{
  // Se os dados atuais na tela n�o foram modificados, nada a fazer
  if(!dataChanged())
    return true;

  // Dados modificados.  Pergunta ao usu�rio se deseja salvar 
  // altera��es, abandonar altera��es ou cancelar a sele��o de 
  // nova regra
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(_parent, tr("Modifica��es n�o salvas!"),
                                 verifyAndEnableChangeMsg(),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                 QMessageBox::Save);

  // Se a opera��o foi cancelada, basta retornar false
  if(answer == QMessageBox::Cancel)
    return false;
  
  // Se os dados foram descartados pelo usu�rio, chama 
  // discardChanges() e retorna true
  if(answer == QMessageBox::Discard)
  {
    discardChanges(restore);
    return true;
  }
 
  // Usu�rio pediu para tentarmos salvar os dados
  // Se opera��o Ok, retorna true, sen�o retorna false cancelando a opera��o
  return validateAndSaveChanges();
}

//! Mensagem padr�o emitida pela fun��o verifyAndEnableChange() quando os dados foram alterados
QString MainDialogTab::verifyAndEnableChangeMsg()
{
  return tr("As altera��es efetuadas na tela atual ainda\n"
            "n�o foram salvas.  Deseja salvar as altera��es?");
}            

//! Slot chamado quando o usu�rio pressiona o bot�o de salvar altera��es
void MainDialogTab::saveRequested()
{
  if(dataChanged())
    validateAndSaveChanges();
}

//! Slot chamado quando o usu�rio pressiona o bot�o de cancelar altera��es
void MainDialogTab::cancelRequested()
{
  discardChanges(true);
}

