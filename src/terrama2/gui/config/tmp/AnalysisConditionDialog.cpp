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
  \file terrama2/gui/config/AnalysisConditionDialog.cpp

  \brief Definition of methods in class AnalysisConditionDialog.cpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "AnalysisConditionDialog.hpp"
#include "analysisList.h"
#include "riskMapList.h"
#include "luaSyntaxHighlighter.h"

// STL
#include <assert.h>

// QT
#include <QMessageBox>
#include <QAction>
#include <QMenu>

#include "luaUtils.h"
#include "qtUtils.h"
#include "utils.h"

/*! Constructor

\param anaList Available analyses list
\param currAnaIndex  Current analyse index in list. (1 if the analyse is brand new)
\param currAnaName   Current analyse name
\param condAnaId     Analylse identifier being in base (-1 if the value is unset)
\param condAnaScript Condition script
*/
AnalysisConditionDialog::AnalysisConditionDialog(const AnalysisList* anaList, const RiskMapList* riskList,
                                           int currAnaIndex, QString currAnaName, 
                                           int condAnaId, QString condAnaScript,
                                           QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _anaId    = condAnaId;
  _rule     = condAnaScript;
  _anaList  = anaList;
  _riskList = riskList;

  condAnalysisName->setText(currAnaName);

  condAnalysisList->clear();
  int currIndex = -1;
  for(int i=0, size=anaList->count(); i<size; i++)
  {
    Analysis* ana = anaList->at(i);
  wsAnalysisType anaType = ana->getAnalysisType();

    if(i == currAnaIndex || anaType == WS_ANALYSISTYPE_TERRAME) continue;

    QIcon ico(anaType == WS_ANALYSISTYPE_RISK ? ":/data/icons/analise_plano de risco.png" :
                        ":/data/icons/analizemodel.png");
    QListWidgetItem* item = new QListWidgetItem(ico, ana->name());
    item->setData(Qt::UserRole, i);
    condAnalysisList->addItem(item);

    if(ana->id() == condAnaId)
      currIndex = condAnalysisList->count()-1;
  }
  condAnalysisList->setCurrentRow(currIndex);
  analysisChanged(currIndex);

  new LuaSyntaxHighlighter(condScriptTed);

  condScriptTed->setPlainText(condAnaScript.trimmed());
  condScriptTed->setFocus();

  connect(okBtn,     SIGNAL(clicked()), SLOT(okPressed()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(condAnalysisList, SIGNAL(currentRowChanged(int)), SLOT(analysisChanged(int)));

  fillZoneOperatorMenu();
  fillResultsMenu();
  fillFunctionsMenu();
  fillCommandsMenu();

  connect(condSyntaxBtn, SIGNAL(clicked()), SLOT(syntaxCheckRequested()));
}

//! Destructor
AnalysisConditionDialog::~AnalysisConditionDialog()
{
}

//! Slot triggered when the current analyse is modified
void AnalysisConditionDialog::analysisChanged(int row)
{
  if(row == -1)
  {
    condAtributesBtn->setVisible(false);
    condZonaisBtn->setVisible(false);
    condResultsBtn->setVisible(false);
    return;
  }

  // Find out analyse type
  int index = condAnalysisList->item(row)->data(Qt::UserRole).toInt();
  Analysis* ana = _anaList->at(index);

  bool isRiskType = (ana->getAnalysisType() == WS_ANALYSISTYPE_RISK);

  condAtributesBtn->setVisible(isRiskType);
  condResultsBtn->setVisible  (isRiskType);
  condZonaisBtn->setVisible   (!isRiskType);
  
  // If type is risk, re-build menu of attributes according with selected analyse
  if(isRiskType)
    fillAttributesMenu(ana->riskMapId());
}

//! Slot triggered when ok button is clicked
void AnalysisConditionDialog::okPressed()
{
  // 1) Role
  _rule = condScriptTed->toPlainText().trimmed();
  if(_rule.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro validando dados..."), tr("Regra não foi preenchida."));
    return;
  }

  // 2) Analyse
  _anaId = -1;
  int pos = condAnalysisList->currentRow();
  if(pos == -1)
  {
    QMessageBox::warning(this, tr("Erro validando dados..."), tr("Análise condicionante não foi selecionada."));
    return;
  }

  int index = condAnalysisList->item(pos)->data(Qt::UserRole).toInt();
  _anaId = _anaList->at(index)->id();
  accept();
}

//! Fill the zone operators of menu
void AnalysisConditionDialog::fillZoneOperatorMenu()
{
  QMenu* zoneOpMenu = new QMenu(tr("Operadores Zonais"), this);
  zoneOpMenu->addAction(tr("Op. zonal p/ grades: Mínimo"            ))->setProperty("added_text", "minimo()");
  zoneOpMenu->addAction(tr("Op. zonal p/ grades: Máximo"            ))->setProperty("added_text", "maximo()");
  zoneOpMenu->addAction(tr("Op. zonal p/ grades: Média"             ))->setProperty("added_text", "media()");  
  zoneOpMenu->addAction(tr("Op. zonal p/ grades: Número de amostras"))->setProperty("added_text", "conta_amostras()"); 
  
  connect(zoneOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  condZonaisBtn->setMenu(zoneOpMenu);
  condZonaisBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Fill standard alert result of menu
void AnalysisConditionDialog::fillResultsMenu()
{
  QPixmap pixmap(10, 10);
  QMenu*  levelsMenu = new QMenu(tr("Níveis de Alerta"), this);
  levelsMenu->addAction(tr("0: Normal"))->setProperty("added_text", "0");
  pixmap.fill(QColor(0, 0, 255));
  levelsMenu->addAction(QIcon(pixmap), tr("1: Observação"))->setProperty("added_text", "1");
  pixmap.fill(QColor(255, 255, 0));
  levelsMenu->addAction(QIcon(pixmap), tr("2: Atenção"))->setProperty("added_text", "2");
  pixmap.fill(QColor(255, 127, 0));
  levelsMenu->addAction(QIcon(pixmap), tr("3: Alerta"))->setProperty("added_text", "3");
  pixmap.fill(QColor(255, 0, 0));
  levelsMenu->addAction(QIcon(pixmap), tr("4: Alerta máximo"))->setProperty("added_text", "4");
  
  connect(levelsMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  condResultsBtn->setMenu(levelsMenu);
  condResultsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Fill the menu with standards functions put on the enviroment
void AnalysisConditionDialog::fillFunctionsMenu()
{
  QMenu* funcOpMenu = new QMenu(tr("Funções padrão"), this);
  funcOpMenu->addAction(tr("print"))->setProperty("added_text", "print(_valores_)");
  funcOpMenu->addAction(tr("tonumber"))->setProperty("added_text", "tonumber(_valor_)");
  funcOpMenu->addAction(tr("tostring"))->setProperty("added_text", "tostring(_valor_)");
  funcOpMenu->addAction(tr("type"))->setProperty("added_text", "type(_valor_)");
  funcOpMenu->addSeparator();
  connect(funcOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

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

  condFunctionsBtn->setMenu(funcOpMenu);
  condFunctionsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Fill the menu with default lua commands
void AnalysisConditionDialog::fillCommandsMenu()
{
  QMenu* cmdOpMenu = new QMenu(tr("Comandos"), this);
  cmdOpMenu->addAction(tr("if then end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addAction(tr("if then elseif then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelseif _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  cmdOpMenu->addSeparator();
  cmdOpMenu->addAction(tr("and"))->setProperty("added_text", "(_condicao_ and _condicao_)");
  cmdOpMenu->addAction(tr("or"))->setProperty("added_text", "(_condicao_ or _condicao_)");
    
  connect(cmdOpMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));

  condCommandsBtn->setMenu(cmdOpMenu);
  condCommandsBtn->setPopupMode(QToolButton::InstantPopup);
}

//! Fill the menu with attributes associated to an monitored object
void AnalysisConditionDialog::fillAttributesMenu(int riskMapId)
{
  QMenu* attributesMenu;
  if(condAtributesBtn->menu())
  {
    attributesMenu = condAtributesBtn->menu();
    attributesMenu->clear();
  }
  else
  {
    attributesMenu = new QMenu(tr("Atributos"), this);
    condAtributesBtn->setMenu(attributesMenu);
    condAtributesBtn->setPopupMode(QToolButton::InstantPopup);
    connect(attributesMenu, SIGNAL(triggered(QAction*)), SLOT(addTextToScript(QAction*)));
  }

  const RiskMap* map = NULL;
  for(int i=0,size=_riskList->count(); i<size; i++)
  {
    if(_riskList->at(i)->id() == riskMapId)
    {
      map = _riskList->at(i);
      break;
    }
  }
  if(!map)
    return;

  // Fill plan attribute of menu
  for(int i=0, num=map->numAttributes(); i<num; i++)
  {
    QString name = map->attributeName(i);
    QString item_name = name + "  (" + Utils::columnTypeToString(map->attributeType(i)) + ")";
    attributesMenu->addAction(item_name)->setProperty("added_text", name);
  }
}

/*! \brief Slot triggered when the user selects an input through context menu

Add the text associated to the menu item selected in script.
In order to do that, it querys property name "added_text" of menu item.

*/
void AnalysisConditionDialog::addTextToScript(QAction* act)
{
  QString value = act->property("added_text").toString();

  condScriptTed->insertPlainText(value); 
  condScriptTed->setFocus(); 
}

//! Slot triggered when the click in syntax valitation button
void AnalysisConditionDialog::syntaxCheckRequested()
{
  QString script = condScriptTed->toPlainText().trimmed();
  QString err;
  int lin;
  
  if(!LuaUtils::checkSyntax(script.toLatin1().data(), err, lin))
  {
    if(lin)
      QtUtils::selectLine(condScriptTed, lin);
    QMessageBox::warning(this, tr("validação de sintaxe..."), err);
  }  
  else
    QMessageBox::warning(this, tr("validação de sintaxe..."), tr("Validação Ok"));  
}
