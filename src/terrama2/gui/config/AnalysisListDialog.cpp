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
 * \file AnalysisListDialog.cpp
 *
 * \brief Definicao dos metodos declarados na classe AnalysisListDialog
 *
 * \author Celso Luiz Ramos Cruz
 * \date february, 2008 
 * \version $Id
 *
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
	  //preenche os combobox
	  fillWarningComboBox(emailCmb);
	  fillWarningComboBox(cellphoneCmb);
      fillReportComboBox(reportCmb);
  }  
}

//! Destrutor
AnalysisListDialog::~AnalysisListDialog()
{
}

//! Preenche valores da WidgetList conforme lista de analises selecionadas
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

//! Procura analise na lista de analises já selecionadas
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

//! Preenche lista de analises selecionadas conforme WidgetList
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

//!< Preenche lista de combobox com os possíveis níveis de alerta e com seu icones
void AnalysisListDialog::fillWarningComboBox( QComboBox* combo, int currentIndex )
{
	//limpa o combo
	combo->clear();
	
	//adiciona os alertas
	combo->addItem(Utils::warningLevelColor(1), Utils::warningLevelToString(1));
	combo->addItem(Utils::warningLevelColor(2), Utils::warningLevelToString(2));
	combo->addItem(Utils::warningLevelColor(3), Utils::warningLevelToString(3));
	combo->addItem(Utils::warningLevelColor(4), Utils::warningLevelToString(4));
	combo->addItem(Utils::warningLevelColor(5), Utils::warningLevelToString(5));

	//define o alerta padrao
	combo->setCurrentIndex(currentIndex);
}

//!< Preenche lista de combobox com os possíveis níveis de relatório
void AnalysisListDialog::fillReportComboBox( QComboBox* combo, int currentIndex )
{
    //limpa o combo
    combo->clear();

    //adiciona os tipos
		combo->addItem(Utils::reportTypeToString(0));
		combo->addItem(Utils::reportTypeToString(1));

    //define o tipo de relatório padrao
    combo->setCurrentIndex(currentIndex);
}

//!< Get email and cell phone levels from dialog
void AnalysisListDialog::getLevels( int& email, int& cellphone, int& report )
{
	email = emailCmb->currentIndex() + 1;
	cellphone = cellphoneCmb->currentIndex() + 1;
    report = reportCmb->currentIndex();
}
