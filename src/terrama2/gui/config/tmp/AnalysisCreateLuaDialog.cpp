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
  \file terrama2/gui/config/AnalysisCreateLuaDialog.cpp

  \brief Definition of class AnalysisCreateLuaDialog to create lua file for utilization with TerraME

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Anderson Araujo Lopes
*/

/*!
\class AnalysisCreateLuaDialog

*/

// TerraMA2
#include "AnalysisCreateLuaDialog.hpp"
#include "ComboBoxDelegate.hpp"
#include "utils.h"

// STL
#include <assert.h>

// QT
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>

//! Construtor
AnalysisCreateLuaDialog::AnalysisCreateLuaDialog(Services* manager, int cellularSpaceID, const std::vector<std::string> columnCelularSpaceList, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  _columnCelularSpaceList = columnCelularSpaceList;
  _manager = manager;
  _cellularSpaceID = cellularSpaceID;

  setupUi(this);

  connect(analysisCreateLuaAddClassCodeItemBtn, SIGNAL(clicked()), SLOT(analysisCreateLuaAddClassCodeItem()));
  connect(analysisCreateLuaRemoveClassCodeItemBtn, SIGNAL(clicked()), SLOT(analysisCreateLuaRemoveClassCodeItem()));
  connect(reloadClassCodesBtn, SIGNAL(clicked()), SLOT(reloadClassCodesBtnSlot()));
  connect (classCodeTableWidget, SIGNAL (cellChanged ( int , int )), this, SLOT (classCodeTableWidgetCellChanged ( int , int )));
  connect (classCodeTableWidget, SIGNAL (cellClicked ( int , int )), this, SLOT (classCodeTableWidgetCellClicked ( int , int )));
  
  connect(analysisCreateLuaAddRainItemBtn, SIGNAL(clicked()), SLOT(analysisCreateLuaAddRainItem()));
  connect(analysisCreateLuaRemoveRainItemBtn, SIGNAL(clicked()), SLOT(analysisCreateLuaRemoveRainItem()));
  connect(openFileRainPushButton, SIGNAL(clicked()), SLOT(openFileRainPushButtonSlot()));
  connect (rainTableWidget, SIGNAL (cellChanged ( int , int )), this, SLOT (rainTableWidgetCellChanged(int , int )));
  connect (rainTableWidget, SIGNAL (cellClicked ( int , int )), this, SLOT (rainTableWidgetCellClicked(int , int )));

  connect (hidrodynamicModelColumnsTableWidget, SIGNAL (cellChanged ( int , int )), this, SLOT (hidrodynamicModelColumnsTableWidgetCellChanged(int , int )));
  connect (hidrodynamicModelColumnsTableWidget, SIGNAL (cellClicked ( int , int )), this, SLOT (hidrodynamicModelColumnsTableWidgetCellClicked ( int , int )));

  connect(okBtn,     SIGNAL(clicked()), SLOT(okPressed()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  enterModelInitialPositionLed->setValidator(new QDoubleValidator(enterModelInitialPositionLed));
  enterModelFinalPositionLed->setValidator(new QDoubleValidator(enterModelFinalPositionLed));
  enterModelDTLed->setValidator(new QDoubleValidator(enterModelDTLed));
  enterModelResLed->setValidator(new QDoubleValidator(enterModelResLed));
  enterModelIa_SLed->setValidator(new QDoubleValidator(enterModelIa_SLed));
  enterModelInfAMCLimitLed->setValidator(new QDoubleValidator(enterModelInfAMCLimitLed));
  enterModelSupAMCLimitLed->setValidator(new QDoubleValidator(enterModelSupAMCLimitLed));

  ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate();
  for(unsigned int i=0; i < _columnCelularSpaceList.size(); ++i)
  {
    std::string str = _columnCelularSpaceList[i];
    comboBoxDelegate->insertItem(str.c_str());
  }
  hidrodynamicModelColumnsTableWidget->setItemDelegateForColumn(1, comboBoxDelegate);

  // Creation of column lines of dynamic hydro model
  addNewLineHidrodynamicModelColumnsTableWidget(-1, "DECLIV");
  addNewLineHidrodynamicModelColumnsTableWidget(-1, "LDD");
  addNewLineHidrodynamicModelColumnsTableWidget(-1, "ACUM");
  addNewLineHidrodynamicModelColumnsTableWidget(-1, "VIS");

  classCodeTableWidget->resizeRowsToContents();
  classCodeTableWidget->resizeColumnsToContents();

  hidrodynamicModelColumnsTableWidget->resizeRowsToContents();
  hidrodynamicModelColumnsTableWidget->resizeColumnsToContents();

  rainTableWidget->resizeRowsToContents();
  rainTableWidget->resizeColumnsToContents();

  reloadClassCodesBtn->setEnabled(false);

}

//! Destrudtor
AnalysisCreateLuaDialog::~AnalysisCreateLuaDialog()
{
}

//! Create an new item of column data of dynamic hydro model
void AnalysisCreateLuaDialog::addNewLineHidrodynamicModelColumnsTableWidget(int position, QString value)
{
  if(position > hidrodynamicModelColumnsTableWidget->rowCount() || position == -1)
    position = hidrodynamicModelColumnsTableWidget->rowCount();

  hidrodynamicModelColumnsTableWidget->insertRow(position);
  hidrodynamicModelColumnsTableWidget->setItem(position, 0, new QTableWidgetItem(value));
  hidrodynamicModelColumnsTableWidget->item(position,0)->setData(Qt::UserRole, true);
}

//! Modified column data cell of dynamic hydro model
void AnalysisCreateLuaDialog::hidrodynamicModelColumnsTableWidgetCellChanged ( int row, int col)
{
  if(col == 0 && !_oldHidrodynamicModelColumns.isEmpty())
  {
    QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item ( row, col );
    item->setText(_oldHidrodynamicModelColumns);
  }
  if(row == 3 && col == 1)
  {
    reloadClassCodesBtn->setEnabled(true);
  }
  hidrodynamicModelColumnsTableWidget->resizeRowsToContents();
  hidrodynamicModelColumnsTableWidget->resizeColumnsToContents();
}

//! Cell has been clicked
void AnalysisCreateLuaDialog::hidrodynamicModelColumnsTableWidgetCellClicked( int row, int col)
{
  if(col == 0)
  {
    QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item ( row, col );
    _oldHidrodynamicModelColumns = item->text();
  }
}

//! Add an class line code
void AnalysisCreateLuaDialog::analysisCreateLuaAddClassCodeItem()
{
  QModelIndex modelIndex = classCodeTableWidget->currentIndex();
  if(modelIndex.row() >=0)
  {
    addNewLineClassCodeTableWidget(modelIndex.row()+1, QString("Classe_%1").arg(modelIndex.row()+1), "-1.0");
  }
  else
  {
    addNewLineClassCodeTableWidget(classCodeTableWidget->rowCount(), QString("Classe_%1").arg(classCodeTableWidget->rowCount()), "-1.0");
  }
  classCodeTableWidget->resizeRowsToContents();
  classCodeTableWidget->resizeColumnsToContents();

}

//! Remove an class line code
void AnalysisCreateLuaDialog::analysisCreateLuaRemoveClassCodeItem()
{
  int row = classCodeTableWidget->currentRow();
  if(row > -1 && classCodeTableWidget->rowCount() > 1)
  {
    classCodeTableWidget->removeRow(row);
  }
  classCodeTableWidget->resizeRowsToContents();
  classCodeTableWidget->resizeColumnsToContents();
}

//! Load the classes from VIS values
void AnalysisCreateLuaDialog::reloadClassCodesBtnSlot()
{
  QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item ( 3, 1);
  if(!item)
    return;
  QString columnName = item->text();

  // If it is VIS
  if(columnName.isEmpty())
    return;
  std::vector<std::string> result;
  if(!_manager->getValuesByColumnName(_cellularSpaceID, columnName.toStdString(), result))
  {
    QMessageBox::warning(NULL, "", tr("Erro ao carregar o espaço celular!"));
    return;
  }

  classCodeTableWidget->clearContents();
  classCodeTableWidget->setRowCount(0);

  for(unsigned int i=0; i< result.size(); i++)
  {
    addNewLineClassCodeTableWidget(-1, result[i].c_str(), "-1.00");
  }

  classCodeTableWidget->resizeRowsToContents();
  classCodeTableWidget->resizeColumnsToContents();
}

//! Include a new class line code and CN@values
void AnalysisCreateLuaDialog::addNewLineClassCodeTableWidget(int position, QString classCodeName, QString value)
{
  if(position > classCodeTableWidget->rowCount() || position == -1)
    position = classCodeTableWidget->rowCount();

  bool ok;
  value.toDouble(&ok);
  if(!ok)
  {
    return;
  }

  classCodeTableWidget->insertRow(position);
  classCodeTableWidget->setItem(position, 0, new QTableWidgetItem(classCodeName));
  classCodeTableWidget->item(position,0)->setData(Qt::UserRole, true);

  classCodeTableWidget->setItem(position, 1, new QTableWidgetItem(value));
  classCodeTableWidget->item(position,1)->setData(Qt::UserRole, true);
}


//! The class code cell has been clicked
void AnalysisCreateLuaDialog::classCodeTableWidgetCellClicked( int row, int column)
{
  QTableWidgetItem *item = classCodeTableWidget->item (row, column);
  if(column == 0)
  {
    _oldClassCodeNameValue = item->text();
  }
  else
  {
    _oldClassCodeCNValue = item->text();
  }
}

//! Modified a class code cell
void AnalysisCreateLuaDialog::classCodeTableWidgetCellChanged ( int row, int column)
{
  QTableWidgetItem *item = classCodeTableWidget->item ( row, column );

  QString s = item->text();
  if(column == 0)
  {
    QRegExp classCodeRegExp = QRegExp("^[a-zA-Z][\\w]*$");
    if(!classCodeRegExp.exactMatch(s))
    {
      item->setText(_oldClassCodeNameValue);
    }
    else
    {
      _oldClassCodeNameValue = s;
    }
  }
  else
  {
    bool ok;
    s.toDouble(&ok);
    if(!ok)
    {
      item->setText(_oldClassCodeCNValue);
    }
    else
    {
      _oldClassCodeCNValue = s;
    }
  }
}

//! Creating a new rain data item
void AnalysisCreateLuaDialog::addNewLineRainTableWidget(int position, QString value)
{
  if(position > rainTableWidget->rowCount() || position == -1)
    position = rainTableWidget->rowCount();

  bool ok;
  value.toDouble(&ok);

  if(ok)
  {
    rainTableWidget->insertRow(position);
    rainTableWidget->setItem(position, 0, new QTableWidgetItem(value));
    rainTableWidget->item(position,0)->setData(Qt::UserRole, true);
    enterModelFinalPositionLed->setText(QString("%1").arg(rainTableWidget->rowCount()));
  }
}

//! Cria um novo item do dado de chuva.
void AnalysisCreateLuaDialog::analysisCreateLuaAddRainItem()
{
  QModelIndex modelIndex = rainTableWidget->currentIndex();
  if(modelIndex.row() >=0)
  {
    addNewLineRainTableWidget(modelIndex.row()+1, "0.00");
  }
  else
  {
    addNewLineRainTableWidget(rainTableWidget->rowCount(), "0.00");
  }

}

//! Remove um alinha no dado de chuva.
void AnalysisCreateLuaDialog::analysisCreateLuaRemoveRainItem()
{
  int row = rainTableWidget->currentRow();
  if(row > -1 && rainTableWidget->rowCount() > 1)
    {
      rainTableWidget->removeRow(row);
      enterModelFinalPositionLed->setText(QString("%1").arg(rainTableWidget->rowCount()));
    }
}

void AnalysisCreateLuaDialog::rainTableWidgetCellChanged ( int row, int column)
{
  QTableWidgetItem *item = rainTableWidget->item ( row, column );
  QString s = item->text();

  bool ok;
  s.toDouble(&ok);
  if(!ok)
  {
    item->setText(_oldRainValue);
  }
  else
  {
    _oldRainValue = s;
  }
}

void AnalysisCreateLuaDialog::rainTableWidgetCellClicked ( int row , int column)
{
  QTableWidgetItem *item = rainTableWidget->item ( row, column );
  _oldRainValue = item->text();
}

void AnalysisCreateLuaDialog::openFileRainPushButtonSlot()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Escolha o arquivo de chuvas"), "",
           tr("Arquivos de Texto (*.txt);;CSV (*.csv);;Todos os Arquivos (*.*)"));

  QStringList listFileValues;
  if (fileName != "") 
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) 
    {
      QMessageBox::critical(this, tr("Error"), tr("Não foi possível abrir o arquivo"));
      return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine();
      listFileValues.append(line.split(";"));
    }
    file.close();
  }
  rainTableWidget->clearContents();
  rainTableWidget->setRowCount(0);

  for(int i=0; i<listFileValues.size(); i++)
  {
    addNewLineRainTableWidget(-1, listFileValues.at(i));
  }

  enterModelFinalPositionLed->setText(QString("%1").arg(rainTableWidget->rowCount()));
}

//! Return the lua file lines
QString AnalysisCreateLuaDialog::getLuaFileLinesString()
{
  return _modelLua;
}

//! Read the original lua file
QString AnalysisCreateLuaDialog::readFile( QString fileName )
{
  QFile input(fileName);
  input.open(QFile::ReadOnly | QFile::Text);
  QString content = input.readAll();
  input.close();

  return content;
}

//! Slot pressed ok button
void AnalysisCreateLuaDialog::okPressed()
{
  if(classCodeTableWidget->rowCount() == 0)
  {
    QMessageBox::warning(NULL, "", QObject::tr("Tabela de classes está vazia."));
    return;
  }

  if(rainTableWidget->rowCount() == 0)
  {
    QMessageBox::warning(NULL, "", QObject::tr("Tabela de chuvas está vazia."));
    return;
  }

  for(int i=0; i< hidrodynamicModelColumnsTableWidget->rowCount(); i++)
  {
    QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item ( i, 1);
    if(!item || item->text().isEmpty())
    {
      QMessageBox::warning(NULL, "", QObject::tr("Valor da tabela de Colunas do modelo hidrodinâmico está vazio."));
      return;
    }
  }

  // Check if the CN valyes are differents of -1
  for(int row=0; row< classCodeTableWidget->rowCount(); row++)
  {
    QTableWidgetItem *item = classCodeTableWidget->item ( row, 0);

    if(!item || item->text().isEmpty())
    {
      QMessageBox::warning(NULL, "", QObject::tr("Nome da Classe não pode ser vazio."));
      return;
    }
    item = classCodeTableWidget->item ( row, 1);

    bool ok;
    double value = item->text().toDouble(&ok);
    if(!ok || value == -1.00)
    {
      QMessageBox::warning(NULL, QObject::tr("Valor do CN"), QObject::tr("Valor CN não poder ser -1."));
      return;
    }
  }


  _modelLua = readFile(":/models/modeloPereira");

  if(fillDescricaoAtributosEspacoCelular() && fillCodificacaoClassesVIS() && fillParametrosEntradaModelo())
    accept();
}

//! Fill the attributes description of CellularSpace
bool AnalysisCreateLuaDialog::fillDescricaoAtributosEspacoCelular()
{
  QString selectedCellularSpace;
  for(int i=0; i< hidrodynamicModelColumnsTableWidget->rowCount(); i++)
  {
    QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item ( i, 1);
    if(!item || item->text().isEmpty())
    {
      continue;
    }
    QString select = item->text();
    if(!selectedCellularSpace.isEmpty())
    {
        selectedCellularSpace += ",";
    }
    selectedCellularSpace += QString("\"%1\"").arg(select);

    switch(i)
    {
      //DECLIV
      case DECLIV:    
        _modelLua = _modelLua.replace("%DECLIV%",select);
        break;
      //LDD
      case LDD:   
        _modelLua = _modelLua.replace("%LDD%",select);
        break;
      //ACUM
      case ACUM:   
        _modelLua = _modelLua.replace("%ACUM%",select);
        break;
      //VIS
      case VIS:   
        _modelLua = _modelLua.replace("%VIS%",select);
        break;
    }
  }
  _modelLua = _modelLua.replace("%SELECTED_CELLULAR_SPACE%",selectedCellularSpace);
  return true;

}

//! Filll up the values of VIS Classes codification
bool AnalysisCreateLuaDialog::fillCodificacaoClassesVIS()
{
  QString textClassCodeLines;
  QString textCNLines;
  QString ifElseCurveNumber;

  bool ok;

  QTableWidgetItem *item = hidrodynamicModelColumnsTableWidget->item (VIS , 1);
  if(!item || item->text().isEmpty())
  {
    QMessageBox::warning(NULL, "", QObject::tr("Valor da coluna VIS está vazio."));
    return false;
  }
  QString stringVIS = "cell." + item->text();

  for(int i=0; i<classCodeTableWidget->rowCount(); i++)
  {
    QString textClassCode = classCodeTableWidget->item(i,0)->text();

    if(textClassCode.isEmpty())
      continue;

    QString textCN = "CN2_" + textClassCode;

    QString textCNValue = classCodeTableWidget->item(i,1)->text();
    textCNValue.toDouble(&ok);
    if(!ok)
      continue;

    if(!textClassCodeLines.isEmpty())
      textClassCodeLines += "\t";
    textClassCodeLines += QString("%1 = %2 \n").arg(textClassCode).arg(i+1);

    if(!textCNLines.isEmpty())
      textCNLines += "\t";

    textCNLines += QString ("%1 = %2 \n").arg(textCN).arg(textCNValue);

    if(ifElseCurveNumber.isEmpty())
      ifElseCurveNumber += QString("if(%1 == %2) then\n").arg(stringVIS).arg(textClassCode);
    else
      ifElseCurveNumber += QString("\n\t\telseif(%1 == %2) then\n").arg(stringVIS).arg(textClassCode);
    ifElseCurveNumber += QString("\t\t\tcell.CN[2] = %1\n").arg(textCN);
  }
  _modelLua = _modelLua.replace("%CODIFICACAO_DAS_CLASSES%",textClassCodeLines);
  _modelLua = _modelLua.replace("%VALORES_CURVA_CADA_CLASSE%",textCNLines);
  _modelLua = _modelLua.replace("%IF_ESLSE_ATRIBUICAO_VALORES_CURVA_NUMERO%",ifElseCurveNumber);

  return true;
}

//! Fill up the input values of model
bool AnalysisCreateLuaDialog::fillParametrosEntradaModelo()
{
  bool ok;
  QString text;

  enterModelInitialPositionLed->text().toDouble(&ok);
  ok ? text = enterModelInitialPositionLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%INI_TIME%",text);

  enterModelFinalPositionLed->text().toDouble(&ok);
  ok ? text = enterModelFinalPositionLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%FINAL_TIME%",text);

  enterModelAMCInicialConditioncomboBox->currentText().toInt(&ok);
  ok ? text = enterModelAMCInicialConditioncomboBox->currentText() : text = "1";
  _modelLua = _modelLua.replace("%AMC_INI%",text);

  enterModelDTLed->text().toDouble(&ok);
  ok ? text = enterModelDTLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%Dt%",text);

  enterModelResLed->text().toDouble(&ok);
  ok ? text = enterModelResLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%Res%",text);

  enterModelIa_SLed->text().toDouble(&ok);
  ok ? text = enterModelIa_SLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%Ia_S%",text);

  enterModelInfAMCLimitLed->text().toDouble(&ok);
  ok ? text = enterModelInfAMCLimitLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%LIMITE_AMC_INF%",text);

  enterModelSupAMCLimitLed->text().toDouble(&ok);
  ok ? text = enterModelSupAMCLimitLed->text() : text = "0.0";
  _modelLua = _modelLua.replace("%LIMITE_AMC_SUP%",text);

  text = "";
  for(int i=0; i<rainTableWidget->rowCount(); i++)
  {
    QString currentItemValue = rainTableWidget->item(i,0)->text();
    currentItemValue.toDouble(&ok);
    if(!ok)
      continue;
    if(i > 0)
      text += ",\n\t\t";
    text += currentItemValue ;
  }

  _modelLua = _modelLua.replace("%CHUVA%",text);

  return true;
}

