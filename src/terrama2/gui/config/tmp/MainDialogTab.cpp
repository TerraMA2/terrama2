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

Além da definição da interface, inclui ainda alguns métodos auxiliares
comuns a todas as fichas.
*/

// STL
#include <assert.h>

// QT
#include <QMessageBox>

// TerraMA2
#include "MainDialogTab.hpp"

/*! \brief Salva dados correntes, se válidos  
  
  Verifica se os dados são válidos.  Se não forem, mostra mensagem
  de erro e retorna false.  Se forem, tenta salvar.  Em caso de 
  erro, mostra mensagem e retorna false. Se a operação de salvar
  foi bem sucedida, retorna true.
*/
bool MainDialogTab::validateAndSaveChanges()
{
  QString err = "";
  
  // Valida dados
  if(!validate(err))
  {
    // Dados invalidos. Mostra mensagem de erro (a não ser que err esteja vazio)
    if(!err.isEmpty())
      QMessageBox::warning(_parent, tr("Erro validando dados..."), err);
    return false;
  }
  
  // Tenta salvar...
  if(!save())
  {
    // Se não conseguiu salvar, mostra mensagem de erro
    QMessageBox::warning(_parent, tr("Erro salvando dados..."), 
                         tr("Erro salvando dados.  Operação cancelada."));
    return false;
  }
  return true;
}

/*! \brief Verifica se os dados atuais foram alterados e 
           viabiliza (ou não) troca de acordo com opção do usuário
           
Se os dados estiverem atualizados, retorna true.  Se não estiverem, 
pergunta ao usuário se deseja salvar as alterações, descartar as 
alterações ou cancelar a operação.

Processa a decisão do usuário.  Se uma tentativa de salvar os dados
for falha, cancela a operação.

\param restore Indica se os dados na tela devem ou não ser restaurados 
               se o usuário descartar as alterações.
\return O retorno da função indica se a operação de troca dos dados visualizados 
        deve (retorno true) ou não prosseguir (retorno false). 
*/
bool MainDialogTab::verifyAndEnableChange(bool restore)
{
  // Se os dados atuais na tela não foram modificados, nada a fazer
  if(!dataChanged())
    return true;

  // Dados modificados.  Pergunta ao usuário se deseja salvar 
  // alterações, abandonar alterações ou cancelar a seleção de 
  // nova regra
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(_parent, tr("Modificações não salvas!"),
                                 verifyAndEnableChangeMsg(),
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                 QMessageBox::Save);

  // Se a operação foi cancelada, basta retornar false
  if(answer == QMessageBox::Cancel)
    return false;
  
  // Se os dados foram descartados pelo usuário, chama 
  // discardChanges() e retorna true
  if(answer == QMessageBox::Discard)
  {
    discardChanges(restore);
    return true;
  }
 
  // Usuário pediu para tentarmos salvar os dados
  // Se operação Ok, retorna true, senão retorna false cancelando a operação
  return validateAndSaveChanges();
}

//! Mensagem padrão emitida pela função verifyAndEnableChange() quando os dados foram alterados
QString MainDialogTab::verifyAndEnableChangeMsg()
{
  return tr("As alterações efetuadas na tela atual ainda\n"
            "não foram salvas.  Deseja salvar as alterações?");
}            

//! Slot chamado quando o usuário pressiona o botão de salvar alterações
void MainDialogTab::saveRequested()
{
  if(dataChanged())
    validateAndSaveChanges();
}

//! Slot chamado quando o usuário pressiona o botão de cancelar alterações
void MainDialogTab::cancelRequested()
{
  discardChanges(true);
}

