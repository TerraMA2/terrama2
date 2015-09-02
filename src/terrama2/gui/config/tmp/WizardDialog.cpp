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
  \file terrama2/gui/config/WizardDialog.cpp

  \brief Definition of Class WizardDialog.cpp methods

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class WizardDialog

After the dialog closed, the script() function must be called to obtain the generated
script provided by user
*/

// STL
#include <assert.h>

// QT
#include <QCoreApplication>
#include <QMessageBox>
#include <QHeaderView>

// TerraMA2
#include "WizardDialog.hpp"
#include "RiskMap.hpp"
#include "WeatherGrid.hpp"

//! Amount of hours enought of acumulated rain for the CPC model
const unsigned int aciSpan = 84;

/*! \brief Construtor

\param riskmap  Monitored selected object
\param sellist  List of available grids
\param parent   Dialog parent Widget
\param f        Flags for build dialog
\
*/
WizardDialog::WizardDialog(const RiskMap* riskmap, const QList<WeatherGrid*>& sellist, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _sellist = QList<WeatherGrid*>(sellist);

  riskMapNameLbl->setText(riskmap->name());

  attributesListWidget->clear();
  cpcCmbBoxAttributes->clear();

  for(int i=0, num=riskmap->numAttributes(); i<num; i++)
  {
    wsColumnType type = riskmap->attributeType(i);
    if(type != WS_COLTYPE_REAL && type != WS_COLTYPE_INT && type != WS_COLTYPE_UNSIGNEDINT)
      continue;

    attributesListWidget->addItem(riskmap->attributeName(i));

    cpcCmbBoxAttributes->addItem(riskmap->attributeName(i));
  }

  if(cpcCmbBoxAttributes->count() == 0)
  {
    cpcCmbBoxAttributes->addItem("Nenhum", -1);
    cpcCmbBoxAttributes->setEnabled(false);
  }

  gridListWidget->clear();
  cpcCmbBoxPCD->clear();
  cpcCmbBoxGrid->clear();

  for(int i=0, num=(int)_sellist.count(); i<num; i++)
  {
    QString name = _sellist[i]->name();
    QListWidgetItem* item = new QListWidgetItem(name);
  
    QIcon icon;
    if(_sellist[i]->data().geometry == WS_WDSGEOM_POINTS) // Dados de PCDs obtidos por meio de servidores de arquivos ou servidores WFS
    {
      icon = QIcon(":/data/icons/dado_pontual.png");
      cpcCmbBoxPCD->addItem(icon, name, i);
    }
    else if(_sellist[i]->data().geometry == WS_WDSGEOM_RASTER) // Dados matriciais obtidos por meio de servidores de arquivos ou servidores WCS
    {
      icon = QIcon(":/data/icons/grid.png");
      cpcCmbBoxGrid->addItem(icon, name, i);
    }
    
    if(_sellist[i]->data().geometry == WS_WDSGEOM_RASTER)
    {
      item->setIcon(icon);
      gridListWidget->addItem(item);
    }
  }

  if(cpcCmbBoxGrid->count() == 0)
  {
    cpcCmbBoxGrid->addItem("Nenhum", -1);
    cpcCmbBoxGrid->setEnabled(false);
  }
  cpcGridChanged(0);

  if(cpcCmbBoxPCD->count() == 0)
  {
    cpcCmbBoxPCD->addItem("Nenhum", -1);
    cpcCmbBoxPCD->setEnabled(false);
  }
  cpcPCDChanged(0);

  // Fill up the zone operation for the generic assistant tab
  zoneOpCmb->clear();
  zoneOpCmb->addItem(tr("Mínimo"), "minimo");
  zoneOpCmb->addItem(tr("Máximo"), "maximo");
  zoneOpCmb->addItem(tr("Média"),  "media");  
  zoneOpCmb->addItem(tr("Número amostras"), "conta_amostras"); 
  zoneOpCmb->setCurrentIndex(0);

  // Fill up the zone operation for the generic CPC tab 
  cpcCmbBoxOperator->clear();
  cpcCmbBoxOperator->addItem(tr("Mínimo"), "minimo");
  cpcCmbBoxOperator->addItem(tr("Máximo"), "maximo");
  cpcCmbBoxOperator->addItem(tr("Média"),  "media");  
  cpcCmbBoxOperator->setCurrentIndex(0);

  // Prepare the role table
  rulesListWidget->clear();

  // Bind the window signals
  connect(okBtn,      SIGNAL(clicked()), SLOT(okRequested()));
  connect(cancelBtn,    SIGNAL(clicked()), SLOT(reject()));

  // Bind the tab signal of generic assistant
  connect(addRuleBtn,   SIGNAL(clicked()), SLOT(addRuleRequested()));
  connect(removeRuleBtn,  SIGNAL(clicked()), SLOT(removeRuleRequested()));

  // Bind the tab signal of CPC assistant
  connect(cpcCmbBoxGrid,  SIGNAL(currentIndexChanged(int)), SLOT(cpcGridChanged(int)));
  connect(cpcCmbBoxPCD,   SIGNAL(currentIndexChanged(int)), SLOT(cpcPCDChanged(int)));
}

//! Destructor
WizardDialog::~WizardDialog()
{
}

/*! \brief Return the generated script by selection made by user

Wheather the user calcels the operation, return empty string

*/
QString WizardDialog::script()
{
  return _script;
}

//! Slot triggered when user press the button to add new role
void WizardDialog::addRuleRequested()
{
  // Check if there is an attribute and there is a grid selected. Retrieve both
  int attr_row = attributesListWidget->currentRow();
  if(attr_row == -1)
  {
    QMessageBox::warning(this, tr("Erro..."), tr("Selecione o atributo do objeto monitorado a ser utilizado."));
    return;
  }
  QString attr_name = attributesListWidget->item(attr_row)->text();
  
  int grid_row = gridListWidget->currentRow();
  if(grid_row == -1)
  {
    QMessageBox::warning(this, tr("Erro..."), tr("Selecione a grade a ser utilizada."));
    return;
  }
  QString grid_name = gridListWidget->item(grid_row)->text();

  // Find out the relation operator among the cells
  QString op;
  if     (opGtRdb->isChecked())  op = ">";
  else if(opGteRdb->isChecked()) op = ">=";
  else if(opLtRdb->isChecked())  op = "<";
  else if(opLteRdb->isChecked()) op = "<=";
  else if(opEqRdb->isChecked())  op = "=";
  else if(opNeqRdb->isChecked()) op = "~=";
  else
    assert(0);

  // Find out the zone operator to be applied
  QString zonal = zoneOpCmb->itemData(zoneOpCmb->currentIndex()).toString();

  QString rule = zonal + "('" + grid_name + "') " + op + " " + attr_name;
  rulesListWidget->addItem(rule);
}

//! Slot chamado quando o usuário pressiona o botão para remover uma regra
void WizardDialog::removeRuleRequested()
{
  int row = rulesListWidget->currentRow();

  if(row == -1)
  {
    QMessageBox::warning(this, tr("Erro..."), tr("Selecione a regra a ser excluída."));
    return;
  }

  delete rulesListWidget->takeItem(row);
}

//! Slot triggered when the user press the ok button
void WizardDialog::okRequested()
{
  if(tabWidget->currentIndex() == 0)
  {
    generateGenericScript();
  }
  else
  {
    if(!generateCPCScript())
      return;
  }
  accept();
}

void WizardDialog::cpcGridChanged(int index)
{
  int wgIndex = cpcCmbBoxGrid->itemData(index).toInt();
  if(wgIndex >= 0 && _sellist[wgIndex]->data().geometry == WS_WDSGEOM_RASTER && _sellist[wgIndex]->data().grads_numBands > 1)
  {
    // The spinbox appers and handle the prediction hours amount 
    cpcLabelHoras->show();
    int maxHours = _sellist[wgIndex]->maxHours();
    cpcSpinBoxHoras->setRange(1, maxHours < aciSpan ? maxHours : aciSpan);
    cpcSpinBoxHoras->setValue(cpcSpinBoxHoras->maximum());
    cpcSpinBoxHoras->show();

    // We put the values of envelope of CPC prediction
    cpcAtencaoSpb->setValue(1.0);
    cpcAlertaSpb->setValue(1.4);
    cpcAlertaMaxSpb->setValue(2.1);
  }
  else
  {
    cpcLabelHoras->hide();
    cpcSpinBoxHoras->hide();

    cpcAtencaoSpb->setValue(0.5);
    cpcAlertaSpb->setValue(1.4);
    cpcAlertaMaxSpb->setValue(2.1);
  }
}

void WizardDialog::cpcPCDChanged(int index)
{
  if(cpcCmbBoxPCD->itemData(index).toInt() != -1)
  {
    cpcCmbBoxPCDAttr->clear();
    int numAttr = _sellist[cpcCmbBoxPCD->itemData(index).toInt()]->data().pcd_attributes.size();

    if(numAttr == 0)
    {
      cpcCmbBoxPCDAttr->addItem("Nenhum");
      cpcCmbBoxPCDAttr->setEnabled(false);
    }
    else
    {
      for (int i = 0; i < numAttr; i++)
      {
        cpcCmbBoxPCDAttr->addItem(QString::fromStdString(_sellist[cpcCmbBoxPCD->itemData(index).toInt()]->data().pcd_attributes.at(i)));
      }
      cpcCmbBoxPCDAttr->setEnabled(true);
    }
  }
  else
  {
    cpcCmbBoxPCDAttr->setEnabled(false);
  }
}

void WizardDialog::generateGenericScript()
{
  if(rulesListWidget->count() == 0)
  {
    QMessageBox::warning(this, tr("Erro..."), tr("Nenhuma regra foi adicionada."));
    return;
  }

  // Concat the whole roles with AND or OR operator according the user selection
  QString op = combineAndRdb->isChecked() ? "and" : "or";

  _script = "if(";
  for(int i=0, num=(int)rulesListWidget->count(); i<num; i++)
  {
    _script += "(" + rulesListWidget->item(i)->text() + ")";
    if(i < num-1)
      _script += " " + op + "\n   ";
    else
      _script += ")\n";
  }
  _script += "then\n";
  _script += "  return 4\n";
  _script += "end\n";
}

bool WizardDialog::generateCPCScript()
{
  WeatherGrid* grid;
  WeatherGrid* pcd;

  // Firstly, we validate the interface configurations and we did catch the data source relevant
  if(cpcCmbBoxGrid->isEnabled())
  {
    grid = _sellist[cpcCmbBoxGrid->itemData(cpcCmbBoxGrid->currentIndex()).toInt()];
  }
  else
  {
    QMessageBox::warning(this, tr("Erro..."), tr("É necessária uma fonte de dados de tipo Grade para "
                           "o cálculo da intensidade de precipitação horária!"));
    return false;
  }

  if(cpcCmbBoxPCD->isEnabled())
  {
    pcd = _sellist[cpcCmbBoxPCD->itemData(cpcCmbBoxPCD->currentIndex()).toInt()];

    if(!cpcCmbBoxPCDAttr->isEnabled())
    {
      QMessageBox::warning(this, tr("Erro..."), tr("É necessária pelo menos um atributo no PCD usado "
                             "no cálculo do CPC!"));
      return false;

    }
  }
  else
  {
    QMessageBox::warning(this, tr("Erro..."), tr("É necessária uma fonte de dados de tipo PCD para o "
                           "cálculo da quantidade de chuva acumulada!"));
    return false;
  }

  QString zonalOperator = cpcCmbBoxOperator->itemData(cpcCmbBoxOperator->currentIndex()).toString();

  bool isForecast = (grid->data().geometry == WS_WDSGEOM_RASTER && grid->data().grads_numBands > 1);  

  QString aci;
  QString li;

  if(!isForecast)
  {
    // Caso não seja um modelo de previsão, a variável aci é calculada apenas
    // usando-se a média das chuvas acumuladas dos pcds que possuem influência 
    // sobre este polígono:
    aci = "local pcds = influencia_pcd('" + cpcCmbBoxPCD->currentText() + "')\n"
        "for i, v in ipairs(pcds) do\n"
        "  aci = aci + ( soma_historico_pcd('" + cpcCmbBoxPCD->currentText() + "', "
                           "'" + cpcCmbBoxPCDAttr->currentText() + "', v, "
                         " " + QString::number(aciSpan) + ") ) / #pcds\n"
        "end";

    // A variável de intensidade de precipitação horária é calculada aplicando-se
    // o operador zonal especificado ao plano selecionado.
    li = zonalOperator + "('" + cpcCmbBoxGrid->currentText() + "') or 0";
  }
  else
  {
    int hours = cpcSpinBoxHoras->value();

    // Variáveis temporárias para cálculo das chuvas acumuladas
    QString variablePrev  = QString::fromStdString("prev" + QString::number(hours).toStdString() + "h");
    QString variablePrevAnt = QString::fromStdString("prev" + QString::number(hours-1).toStdString() + "h");
    aci = "local " + variablePrev + " = " + zonalOperator + "_pn('" + cpcCmbBoxGrid->currentText() + "', " + QString::number(hours) + ")\n";
    if (hours > 1)
      aci += "local " + variablePrevAnt + " = " + zonalOperator + "_pn('" + cpcCmbBoxGrid->currentText() + "', " + 
           QString::number(hours - 1) + ")\n";
    
    aci += "\n";

    // No caso do modelo de previsão, pegamos uma quantidade de horas
    // das bandas do arquivo ETA de acordo com o especificado pelo usuário
    // e complementamos com dados dos PCDs para que se somem uma quantidade
    // necessária de horas (constante aciSpan).
    aci += "local pcds = influencia_pcd('" + cpcCmbBoxPCD->currentText() + "')\n"
        "for i, v in ipairs(pcds) do\n"
        "  aci = aci + ( soma_historico_pcd('" + cpcCmbBoxPCD->currentText() + "', "
                                         "'" + cpcCmbBoxPCDAttr->currentText() + "', v,"
                         " " + QString::number(aciSpan - hours + 1) + ") ) / #pcds\n"
        "end\n";
    
    // Só consideramos a fonte de dados ETA no modelo caso o usuário
    // tenha escolhido mais de uma hora de previsão (neste caso, só
    // usamos a banda de índice 1 no cálculo do li).
    if(hours > 1)
    {
      aci += "aci = aci + " + variablePrevAnt;
    }

    // A intensidade de precipitação horária prevista é calculada utilizando
    // a quantidade de chuva acumulada entre a hora desejada pelo usuário e
    // a hora anterior.
    li = variablePrev;
    if(hours > 1)
      li += " - " + variablePrevAnt; //zonalOperator + "_eta('" + cpcCmbBoxGrid->currentText() + "', " + toString(hours-1) + ")";
    
    li += " or 0";
  }
  
  // Agora, com as variáveis com seus valores atribuídos, vamos escrever o script:
  _script = "\t -- Análise de Coeficiente de Precipitação Crítica\n"
        "\n"
        "-- Constante de Risco por Área (K): \n"
        "local k = " + cpcCmbBoxAttributes->currentText() + "\n"
        "\n"
        "-- Cálculo da variável de Acumulação de Chuvas (Aci): \n"
        "local aci = 0\n" 
        "" + aci + "\n"
        "\n"
        "-- Cálculo da variável de Intensidade de Precipitação Horária (Li): \n"
        "local li = " + li + "\n"
        "\n"
        "-- Cálculo do CPC: \n"
        "local CPC = li / ( k * ( aci ^-0.933 ) ) or 0\n"
        "\n"
        "if      CPC < " + QString::number(cpcAtencaoSpb->value()) +    " then return 1\n"
        "elseif  CPC < " + QString::number(cpcAlertaSpb->value()) +     " then return 2\n"
        "elseif  CPC < " + QString::number(cpcAlertaMaxSpb->value())  + " then return 3\n"
        "                  else return 4\n"
        "end";

  return true;
}

