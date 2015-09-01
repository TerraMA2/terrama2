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
  \file terrama2/gui/config/MainDialogTab.hpp

  \brief Definition of Class MainDialogTab.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _MAINDLG_TAB_H_
#define _MAINDLG_TAB_H_

// QT
#include <QObject>

// TerraMA2  
#include "MainDialog.hpp"

class QMainWindow;
class Services;

/*!
\brief Interface para defini��o das opera��es comuns a 
       todas as classes que tratam dados das fichas da janela 
       principal do sistema.
*/      
class MainDialogTab : public QObject
{
Q_OBJECT

public:
  /*! \brief Construtor

    \param  main_dialog  Di�logo principal do sistema que cont�m os tabs
    \param  manager Gerenciador de servi�os, utilzado para acesso �s listas de dados remotos
  */
  MainDialogTab(MainDialog* main_dialog, Services* manager)
    : QObject(main_dialog), _ui(main_dialog->ui()), 
      _parent(main_dialog), _manager(manager) {}

  //! Destrutor
  virtual ~MainDialogTab() {};

  /*! \brief Carrega dados lidos dos servi�os para a interface.  
         
    Chamada sempre que a configura��o ativa for alterada.
  */
  virtual void load() = 0;
  
  /*! \brief Indica se os dados foram alterados pelo usu�rio
  
    Retorna true se os dados foram alterados, false caso contr�rio
  */
  virtual bool dataChanged() = 0;

  /*! \brief Verifica se os dados atuais s�o validos e podem ser salvos
  
    Fun��o chamada por validateAndSaveChanges para validar valores na tela.
  
    \param err String que deve ser preenchida com a mensagem de erro caso
               os dados sejam inv�lidos.  Se err for preenchida com a string
               vazia, mensagem n�o ser� mostrada por validateAndSaveChanges()
    \return Retorna true se os dados s�o v�lidos, false caso contr�rio               
  */
  virtual bool validate(QString& err) = 0;

  /*! \brief Salva dados correntes

    Tenta salvar os dados correntes.  Retorna true ou false indicando 
    o sucesso da opera��o.  
  */
  virtual bool save() = 0;

  /*! \brief Cancela edi��es feitas nos dados correntes 
  
    \param restore_data Se true, indica que os dados originais devem 
                        ser restaurados na tela.  Se false, indica que 
                        os campos n�o precisam ser alterados.
  */
  virtual void discardChanges(bool restore_data) = 0;

  virtual bool validateAndSaveChanges();
  virtual bool verifyAndEnableChange(bool restore);

public slots:
  virtual void saveRequested();
  virtual void cancelRequested();
  
protected:
  virtual QString verifyAndEnableChangeMsg();

  Ui::MainDlg* _ui;      //!< Widgets do di�logo principal
  QMainWindow* _parent;  //!< Janela principal
  Services*    _manager; //!< Gerenciador de servi�os
};


#endif

