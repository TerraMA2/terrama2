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
 * \file wizardDlg.h
 *
 * \brief Definicao da classe WizardDlg
 *
 * \author Carlos Augusto Teixeira Mendes
 * \date february, 2008 
 * \version $Id$
 *
 */

#ifndef _WIZARDDLG_H_
#define _WIZARDDLG_H_

#include <QList>

#include "ui_wizardDlg.h"

class RiskMap;
class WeatherGrid;

/*! \brief Classe que implementa as açoes do diálogo do assistente
           para criação de regras de análise
*/      
class WizardDlg : public QDialog, private Ui::WizardDlg
{
Q_OBJECT

public:
  WizardDlg(const RiskMap* riskmap, const QList<WeatherGrid*>& sellist, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~WizardDlg();

  QString script();

private slots:
  void okRequested();
  void addRuleRequested();
  void removeRuleRequested();
  void cpcGridChanged(int index);
  void cpcPCDChanged(int index);

private:
  QString _script;  //!< Script gerado
  QList<WeatherGrid*> _sellist; //!< Lista de fontes de dados selecionadas

  void generateGenericScript();
  bool generateCPCScript();
};


#endif

