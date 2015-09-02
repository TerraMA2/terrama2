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
  \file terrama2/gui/config/ColRuleDialog.cpp

  \brief Definition of methods in class ColRuleDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Pedro Luchini de Moraes
*/

// STL
#include <assert.h>

// QT  
#include <QAction>
#include <QMenu>
#include <QMessageBox>

// TerraMA2
#include "ColRuleDialog.hpp"
#include "LuaSyntaxHighlighter.hpp"
#include "LuaUtils.hpp"
#include "QtUtils.hpp"

//! Construtor
ColRuleDialog::ColRuleDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _ignoreChangeEvents = false;
  _collectionRuleChanged  = false;

  okBtn->setEnabled(false);

  // Connect
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  // Conecta sinais para detectar dados alterados
  connect(txtLuaScript,   SIGNAL(textChanged()), SLOT(setColRuleChanged()));
  
  new LuaSyntaxHighlighter(txtLuaScript);

  // Habilita botões de ajuda  
  fillFunctionsMenu();
  fillCommandsMenu();
  
  // Conecta sinal do botão de verificação ed sintaxe
  connect(syntaxBtn, SIGNAL(clicked()), SLOT(syntaxCheckRequested()));
}

//! Destrutor
ColRuleDialog::~ColRuleDialog()
{
}

//! Preenche valores da interface conforme collectionRule
void ColRuleDialog::setFields(const QString& colRule)
{
  _ignoreChangeEvents = true;

  txtLuaScript->setText(colRule);

  _ignoreChangeEvents = false;
}

//! Preenche collectionRule conforme valores da interface
void ColRuleDialog::getFields(QString* colRule, bool& changed)
{
  *colRule = txtLuaScript->toPlainText();
  
  changed = _collectionRuleChanged;
}

//! Marca flag indicando que houve alteração em algum campo
void ColRuleDialog::setColRuleChanged()
{
  if (_ignoreChangeEvents)
	  return;

  okBtn->setEnabled(true);
  _collectionRuleChanged = true;
}

//! Preenche menu com funções padrão colocadas no ambiente
void ColRuleDialog::fillFunctionsMenu()
{
  // Menu com operações padrão
  QMenu* funcOpMenu = new QMenu(tr("Funções padrão"), this);
  funcOpMenu->addAction(tr("print"))->setProperty("added_text", "print(_valores_)");
  funcOpMenu->addAction(tr("tonumber"))->setProperty("added_text", "tonumber(_valor_)");
  funcOpMenu->addAction(tr("tostring"))->setProperty("added_text", "tostring(_valor_)");
  funcOpMenu->addAction(tr("type"))->setProperty("added_text", "type(_valor_)");
  funcOpMenu->addSeparator();
  connect(funcOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
  
  // Menu com operações matemáticas
  QMenu* mathMenu = new QMenu(tr("Func. Matemáticas"), this);
  mathMenu->addAction(tr("abs"  ))->setProperty("added_text", "math.abs(_numero_)");
  mathMenu->addAction(tr("ceil" ))->setProperty("added_text", "math.ceil(_numero_)");
  mathMenu->addAction(tr("exp"  ))->setProperty("added_text", "math.exp(_numero_)");
  mathMenu->addAction(tr("floor"))->setProperty("added_text", "math.floor(_numero_)");
  mathMenu->addAction(tr("log"  ))->setProperty("added_text", "math.log(_numero_)");
  mathMenu->addAction(tr("log10"))->setProperty("added_text", "math.log10(_numero_)");
  mathMenu->addAction(tr("max"  ))->setProperty("added_text", "math.max(_lista_de_numeros_)");
  mathMenu->addAction(tr("min"  ))->setProperty("added_text", "math.min(_lista_de_numeros_)");
  mathMenu->addAction(tr("modf" ))->setProperty("added_text", "math.modf(_numero_)");
  mathMenu->addAction(tr("pow"  ))->setProperty("added_text", "math.pow(_numero_, _numero_)");
  mathMenu->addAction(tr("sqrt "))->setProperty("added_text", "math.sqrt(_numero_)");
  mathMenu->addSeparator();

  QMenu* trigMenu = new QMenu(tr("Trigonométricas"), this);
  trigMenu->addAction(tr("acos" ))->setProperty("added_text", "math.acos(_numero_)");
  trigMenu->addAction(tr("asin" ))->setProperty("added_text", "math.asin(_numero_)");
  trigMenu->addAction(tr("atan" ))->setProperty("added_text", "math.atan(_numero_)");
  trigMenu->addAction(tr("atan2"))->setProperty("added_text", "math.atan2(_numero_, _numero_)");
  trigMenu->addAction(tr("cos"  ))->setProperty("added_text", "math.cos(_numero_em_radianos_)");
  trigMenu->addAction(tr("deg"  ))->setProperty("added_text", "math.deg(_numero_em_radianos_)");
  trigMenu->addAction(tr("pi"   ))->setProperty("added_text", "math.pi");
  trigMenu->addAction(tr("rad"  ))->setProperty("added_text", "math.rad(_numero_em_graus_)");
  trigMenu->addAction(tr("sin"  ))->setProperty("added_text", "math.sin(_numero_em_radianos_)");
  trigMenu->addAction(tr("tan"  ))->setProperty("added_text", "math.tan(_numero_em_radianos_)");
  QMenu* hiperMenu = new QMenu(tr("Hiperbólicas"), this);
  hiperMenu->addAction(tr("cosh" ))->setProperty("added_text", "math.cosh(_numero_)");
  hiperMenu->addAction(tr("sinh" ))->setProperty("added_text", "math.sinh(_numero_)");
  hiperMenu->addAction(tr("tanh" ))->setProperty("added_text", "math.tanh(_numero_)");

  mathMenu->addMenu(trigMenu);
  mathMenu->addMenu(hiperMenu);
  funcOpMenu->addMenu(mathMenu);
  
  // Associa menu com o botão
  functionsBtn->setMenu(funcOpMenu);
  functionsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Preenche menu com comandos padrão em Lua
void ColRuleDialog::fillCommandsMenu()
{
  QMenu* cmdOpMenu = new QMenu(tr("Comandos"), this);
  cmdOpMenu->addAction(tr("if then end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then elseif then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelseif _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addSeparator();
  cmdOpMenu->addAction(tr("and"))->setProperty("added_text", "(_condicao_ and _condicao_)");
  cmdOpMenu->addAction(tr("or"))->setProperty("added_text", "(_condicao_ or _condicao_)");
    
  connect(cmdOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  commandsBtn->setMenu(cmdOpMenu);
  commandsBtn->setPopupMode(QToolButton::InstantPopup);
}

/*! \brief Slot chamado quando o usuário seleciona uma entrada via menu de contexto
           
Adiciona o texto associado ao item de menu selecionado no script.
Para isso, consulta a propriedade de nome "added_text" do item de menu
*/
void ColRuleDialog::addTextToScript(QAction* act)
{
  QString value = act->property("added_text").toString();

  txtLuaScript->insertPlainText(value); 
  txtLuaScript->setFocus(); 
}

//! Slot chamado quando o usuário pressiona o botão de validação de sintaxe do script
void ColRuleDialog::syntaxCheckRequested()
{
  QString script = txtLuaScript->toPlainText().trimmed();
  QString err;
  int lin;
  
  if(!LuaUtils::checkSyntax(script.toLatin1().data(), err, lin))
  {
    if(lin)
      QtUtils::selectLine(txtLuaScript, lin);
    QMessageBox::warning(this, tr("validação de sintaxe..."), err);
  }  
  else
    QMessageBox::warning(this, tr("validação de sintaxe..."), tr("Validação Ok"));  
}
