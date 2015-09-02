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
  \file terrama2/gui/config/AnalysisFillCellOpDialog.cpp

  \brief Definition of class AnalysisFillCellOpDialog methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "AnalysisFillCellOpDialog.hpp"
#include "Services.hpp"
#include "analysis.h"
#include "weatherGridList.h"

// QT
#include <ComboBoxDelegate.hpp>
#include <QListWidget>
#include <QMessageBox>
#include <QHeaderView>

//! Constructor
AnalysisFillCellOpDialog::AnalysisFillCellOpDialog(Services* manager, QListWidget* analysisTerraMEListWidget, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  connect(okBtn,     SIGNAL(clicked()), SLOT(validateOperations()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(fillOpAddBtn, SIGNAL(clicked()), SLOT(addFillOp()));
  connect(fillOpDelBtn, SIGNAL(clicked()), SLOT(delFillOp()));

  fillOpDelBtn->setEnabled(false);

  _changed      = false;
  _ignoreChangeEvents = false;

  fillOpTableWidget->horizontalHeader()->setHighlightSections(false);
  fillOpTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  WeatherGridList* weatherGridList = manager->weatherGridList();
  ComboBoxDelegate* cmbWeatherGridDelegate = new ComboBoxDelegate();

  for(int i = 0, num = (int)analysisTerraMEListWidget->count(); i < num; i++)
  {
    int id = analysisTerraMEListWidget->item(i)->data(Qt::UserRole).toInt();
    WeatherGrid* weatherGrid = weatherGridList->findGrid(id);

    if(!weatherGrid)
      continue;

    QString weatherGridName = weatherGrid->name();
    cmbWeatherGridDelegate->insertItem(weatherGridName);

    _weatherGridNameToIndexMap.insert(weatherGridName, i);
    _weatherGridNameToIDMap.insert(weatherGridName, id);
  }

  fillOpTableWidget->setItemDelegateForColumn(0, cmbWeatherGridDelegate);

  // Fill up the map with the available operations in the same order of wsFillOperationType enum
  // And then, with the same names at wsUtils::FillOperationType::strFromWs(wsFillOperationType type) method
  _fillOpsMap.insert(0, "p_maximo");
  _fillOpsMap.insert(1, "p_minimo");
  _fillOpsMap.insert(2, "p_media");
  _fillOpsMap.insert(3, "p_maximo_pn");
  _fillOpsMap.insert(4, "p_minimo_pn");
  _fillOpsMap.insert(5, "p_media_pn");

  ComboBoxDelegate* cmbFillOpDelegate = new ComboBoxDelegate();
  for(int i = 0; i < _fillOpsMap.size(); ++i)
    cmbFillOpDelegate->insertItem(_fillOpsMap[i]);

  fillOpTableWidget->setItemDelegateForColumn(1, cmbFillOpDelegate);

  connect(fillOpTableWidget, SIGNAL(cellChanged(int, int)), SLOT(setChanged()));
}

//! Destructor
AnalysisFillCellOpDialog::~AnalysisFillCellOpDialog()
{
}

//! Dialog initializer
void AnalysisFillCellOpDialog::setFields(Services* manager, std::vector<struct wsFillOperation> fillOperations)
{
  _ignoreChangeEvents = true;
  _changed = false;

  fillOpTableWidget->setRowCount(fillOperations.size());

  if(fillOperations.size() > 0)
    fillOpDelBtn->setEnabled(true);

  // Obter a lista de fontes de dados para recuperar o nome da fonte associada a uma operação de preenchimento
  WeatherGridList* weatherGridList = manager->weatherGridList();

  for(int i = 0; i < fillOperations.size(); ++i)
  {
    wsFillOperation fillOp = fillOperations.at(i);

    WeatherGrid* weatherGrid = weatherGridList->findGrid(fillOp.weatherDataSourceID);

    if(!weatherGrid)
      continue;

    fillOpTableWidget->setItem(i, 0, new QTableWidgetItem(weatherGrid->name()));
    fillOpTableWidget->item(i, 0)->setData(Qt::UserRole, _weatherGridNameToIndexMap[weatherGrid->name()]); //o valor de Qt::UserRole precisa ser o índice do elemento no ComboBoxDelegate

    fillOpTableWidget->setItem(i, 1, new QTableWidgetItem(_fillOpsMap[fillOp.opType]));
    fillOpTableWidget->item(i, 1)->setData(Qt::UserRole, fillOp.opType);

    fillOpTableWidget->setItem(i, 2, new QTableWidgetItem());
    fillOpTableWidget->item(i, 2)->setData(Qt::DisplayRole, (unsigned int)fillOp.hours);

    fillOpTableWidget->setItem(i, 3, new QTableWidgetItem(fillOp.column.c_str()));
  }

  _ignoreChangeEvents = false;
}

//! Retrieve the output dialog result
void AnalysisFillCellOpDialog::getFields(std::vector<struct wsFillOperation>& fillOperations, bool& changed)
{
  changed = _changed;

  if (_changed)
  {
    fillOperations.clear();

    for (int i = 0; i < fillOpTableWidget->rowCount(); i++)
    {
      wsFillOperation op;
      
      QString weatherDataSourceName = fillOpTableWidget->item(i, 0)->data(Qt::DisplayRole).toString();
      op.weatherDataSourceID = _weatherGridNameToIDMap[weatherDataSourceName];
      
      op.opType = (wsFillOperationType) fillOpTableWidget->item(i, 1)->data(Qt::UserRole).toInt();
      
      int hours = fillOpTableWidget->item(i, 2)->data(Qt::DisplayRole).toInt();
      op.hours = hours >= 0 ? hours : 0;

      op.column = fillOpTableWidget->item(i, 3)->data(Qt::DisplayRole).toString().toStdString();

      fillOperations.push_back(op);
    }
  }
}

//! It warns that there is modification to enable the Save button
void AnalysisFillCellOpDialog::setChanged() 
{
  if(_ignoreChangeEvents)
    return;

  _changed = true;
}

//! Remove the selected lines on fill operations table
void AnalysisFillCellOpDialog::delFillOp()
{
  QList<QTableWidgetItem*> selectedItems = fillOpTableWidget->selectedItems();

  QList<int> itemsToRemove;
  QTableWidgetItem* item;

  if (selectedItems.size() > 0)
  {
    for(int i = 0; i < selectedItems.size(); i++)
    {
      item = selectedItems.at(i);
      if(item->column() != 0)
        continue;

      itemsToRemove.push_front(item->row());
    }

    // Store the row numbers to be removed in asc order
    qSort(itemsToRemove.begin(), itemsToRemove.end()); 

    for(int i = 0; i < itemsToRemove.size();)
      fillOpTableWidget->removeRow(itemsToRemove.takeLast());

    if(fillOpTableWidget->rowCount() == 0)
      fillOpDelBtn->setEnabled(false);

    emit setChanged();
  }
}

//! Insert a row in fill operations table
void AnalysisFillCellOpDialog::addFillOp()
{
  if(_weatherGridNameToIDMap.size() == 0)
    return;

  fillOpDelBtn->setEnabled(true);

  int rows = fillOpTableWidget->rowCount();
  fillOpTableWidget->insertRow(rows);

  fillOpTableWidget->setItem(rows, 0, new QTableWidgetItem());
  fillOpTableWidget->setItem(rows, 1, new QTableWidgetItem());
  fillOpTableWidget->setItem(rows, 2, new QTableWidgetItem());
  fillOpTableWidget->item(rows, 2)->setData(Qt::DisplayRole, (unsigned int)0);
  fillOpTableWidget->setItem(rows, 3, new QTableWidgetItem());
}

//! Validate the informations stored in fill operations table
void AnalysisFillCellOpDialog::validateOperations()
{
  QSet<QString> columnNamesSet;

  for (int i = 0; i < fillOpTableWidget->rowCount(); i++)
  {
    QString weatherDataSourceName = fillOpTableWidget->item(i, 0)->data(Qt::DisplayRole).toString();
    QString operation = fillOpTableWidget->item(i, 1)->data(Qt::DisplayRole).toString();
    QString column = fillOpTableWidget->item(i, 3)->data(Qt::DisplayRole).toString();

    if(weatherDataSourceName.isEmpty() || operation.isEmpty() || column.isEmpty())
    {
      QMessageBox::warning(this, tr("Erro..."), tr("Todos os campos da tabela devem ser preenchidos."));
      return;
    }

    columnNamesSet.insert(column);
  }

  if(columnNamesSet.size() < fillOpTableWidget->rowCount())
  {
    QMessageBox::warning(this, tr("Erro..."), tr("Não pode haver nomes repetidos na coluna \"Coluna de saída\"."));
    return;
  }

  accept();
}