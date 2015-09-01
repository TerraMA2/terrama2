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
  \file terrama2/gui/config/AnalysisListDialog.cpp

  \brief Definition of methods in AnalysisListDialog.cpp class

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/

/*!
\class AnalysisListDialog

*/

// TerraMA2
#include "AnalysisListDialog.hpp"
#include "utils.h"

// STL
#include <assert.h>

//! Construtor
AnalysisListDialog::AnalysisListDialog(bool hideWarningCombo, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  // Connect
  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  if (hideWarningCombo)
  {
    warningGbox->hide();
  }
  else
  {
    fillWarningComboBox(emailCmb);
    fillWarningComboBox(cellphoneCmb);
    fillReportComboBox(reportCmb);
  }  
}

//! Destructor
AnalysisListDialog::~AnalysisListDialog()
{
}

//! Fill up the WidgetList values according to selected analysis list
bool AnalysisListDialog::setFields(const std::vector<struct wsUserView>& userViews, AnalysisList* anaList)
{
  QListWidgetItem* item;
  wsAnalysisRule anaRule;
  bool ret = false;

  listWidgetAnalysis->clear();

  for (int i=0; i<anaList->size(); i++)
  {
    anaRule = anaList->at(i)->data();

  if (!findAnalysisRule(userViews, anaRule))
  {
    if(anaList->at(i)->getAnalysisType() == WS_ANALYSISTYPE_RISK)
    {
         QString ico = ":/data/icons/analise_plano de risco.png";
     item = new QListWidgetItem(QIcon(ico), QString::fromStdString(anaRule.name));
     item->setData(Qt::UserRole, anaRule.id);

     listWidgetAnalysis->addItem(item);
     ret = true;
    }
    else if(anaList->at(i)->getAnalysisType() == WS_ANALYSISTYPE_PCD)
    {
     QString ico = ":/data/icons/dado_pontual.png";
     item = new QListWidgetItem(QIcon(ico), QString::fromStdString(anaRule.name));
     item->setData(Qt::UserRole, anaRule.id);

     listWidgetAnalysis->addItem(item);
     ret = true;
    }
  }
  }

  return ret;
}

bool AnalysisListDialog::setFields( const std::vector<struct wsAnalysisRule>& analysisRules, AnalysisList* anaList )
{
  QListWidgetItem* item;
  wsAnalysisRule anaRule;
  bool ret = false;

  listWidgetAnalysis->clear();

  for (int i=0; i<anaList->size(); i++)
  {
    anaRule = anaList->at(i)->data();

    if (!findAnalysisRule(analysisRules, anaRule))
    {
      if(anaList->at(i)->getAnalysisType() == WS_ANALYSISTYPE_RISK)
      {
                QString ico = ":/data/icons/analise_plano de risco.png";
        item = new QListWidgetItem(QIcon(ico), QString::fromStdString(anaRule.name));
        item->setData(Qt::UserRole, anaRule.id);

        listWidgetAnalysis->addItem(item);
        ret = true;
      }

      if(anaList->at(i)->getAnalysisType() == WS_ANALYSISTYPE_PCD)
      {
        QString ico = ":/data/icons/dado_pontual.png";
        item = new QListWidgetItem(QIcon(ico), QString::fromStdString(anaRule.name));
        item->setData(Qt::UserRole, anaRule.id);

        listWidgetAnalysis->addItem(item);
        ret = true;
      }
    }
  }

  return ret;
}

//! Find by analyse at selected list
bool AnalysisListDialog::findAnalysisRule(const std::vector<struct wsUserView>& userViews, const wsAnalysisRule& anaRule)
{
  wsUserView userView;
  bool ret = false;

  for (unsigned i=0; i<userViews.size() && !ret; i++)
  {
    userView = userViews.at(i);
    ret = (userView.analysisID == anaRule.id);
  }

  return ret;
}

bool AnalysisListDialog::findAnalysisRule( const std::vector<struct wsAnalysisRule>& analysisRules, const wsAnalysisRule& anaRule )
{
  wsAnalysisRule analysisRule;
  bool ret = false;

  for (unsigned i=0; i<analysisRules.size() && !ret; i++)
  {
    analysisRule = analysisRules.at(i);
    if(analysisRule.id == anaRule.id)
    {
      ret = true;
      break;
    }
  }

  return ret;
}

//! Fill up the selected analysis list according WidgetList
void AnalysisListDialog::getFields(QList<int>& newIDSelectedAnalyses, bool& changed)
{
  QListWidgetItem* item;

  newIDSelectedAnalyses.clear();

  changed = (listWidgetAnalysis->selectedItems().size()>0);

  for (int i=0; i<listWidgetAnalysis->selectedItems().size(); i++)
  {
    item = listWidgetAnalysis->selectedItems().at(i);
    newIDSelectedAnalyses.push_back(item->data(Qt::UserRole).toInt());
  }
}

//! Fill up the combobox list with available alert levels
void AnalysisListDialog::fillWarningComboBox( QComboBox* combo, int currentIndex )
{
  combo->clear();

  combo->addItem(Utils::warningLevelColor(1), Utils::warningLevelToString(1));
  combo->addItem(Utils::warningLevelColor(2), Utils::warningLevelToString(2));
  combo->addItem(Utils::warningLevelColor(3), Utils::warningLevelToString(3));
  combo->addItem(Utils::warningLevelColor(4), Utils::warningLevelToString(4));
  combo->addItem(Utils::warningLevelColor(5), Utils::warningLevelToString(5));

  combo->setCurrentIndex(currentIndex);
}

//! Fill up the combobox list with available report levels
void AnalysisListDialog::fillReportComboBox( QComboBox* combo, int currentIndex )
{
    combo->clear();

    combo->addItem(Utils::reportTypeToString(0));
    combo->addItem(Utils::reportTypeToString(1));

    combo->setCurrentIndex(currentIndex);
}

//! Get email and cell phone levels from dialog
void AnalysisListDialog::getLevels( int& email, int& cellphone, int& report )
{
  email = emailCmb->currentIndex() + 1;
  cellphone = cellphoneCmb->currentIndex() + 1;
    report = reportCmb->currentIndex();
}
