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
  \file terrama2/gui/config/IntersectionDialog.cpp

  \brief Definition of Class IntersectionDialog.hpp

  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ui_IntersectionDialogForm.h"
#include "IntersectionDialog.hpp"
#include "../../core/ApplicationController.hpp"
#include "../../core/IntersectionDAO.hpp"
#include "../../core/Intersection.hpp"
#include "../../core/DataManager.hpp"
#include "../Exception.hpp"
#include "../core/DataType.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>

// QT
#include <QMessageBox>

struct terrama2::gui::config::IntersectionDialog::Impl
{
  Impl(const terrama2::core::Intersection& intersection, terrama2::gui::core::Database* database)
    : ui_(new Ui::IntersectionDialogForm),
      intersection_(intersection)
  {
    try
    {
      std::map<std::string, std::string> connInfo;
      connInfo["PG_HOST"] = database->host_.toStdString();
      connInfo["PG_PORT"] = std::to_string(database->port_);
      connInfo["PG_USER"] = database->user_.toStdString();
      connInfo["PG_PASSWORD"] = database->password_.toStdString();
      connInfo["PG_DB_NAME"] = database->dbName_.toStdString();
      connInfo["PG_CLIENT_ENCODING"] = "UTF-8";
      dataSource_ = te::da::DataSourceFactory::make("POSTGIS");
      dataSource_->setConnectionInfo(connInfo);
      dataSource_->open();
    }
    catch (...)
    {
      // todo: handling terralib exception
    }
  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::IntersectionDialogForm* ui_;
  terrama2::core::Intersection intersection_;
  std::unique_ptr<te::da::DataSource> dataSource_;
};

terrama2::gui::config::IntersectionDialog::IntersectionDialog(const terrama2::core::Intersection& intersection,
                                                              terrama2::gui::core::Database* database,
                                                              QWidget* parent,
                                                              Qt::WindowFlags f)
: QDialog(parent, f), pimpl_(new Impl(intersection, database))
{
  pimpl_->ui_->setupUi(this);

  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(onOkBtnClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));
  connect(pimpl_->ui_->goToThemePageBtn, SIGNAL(clicked()), SLOT(showVectorialPage()));
  connect(pimpl_->ui_->goToGridPageBtn, SIGNAL(clicked()), SLOT(showGridPage()));

  connect(pimpl_->ui_->themeList, SIGNAL(itemSelectionChanged()), SLOT(onDatasetSelected()));
  connect(pimpl_->ui_->vectorAttributesTableWidget, SIGNAL(cellDoubleClicked(int, int)), SLOT(onAttributeSelected(int, int)));
  connect(pimpl_->ui_->dynamicGridsTableWidget, SIGNAL(cellDoubleClicked(int, int)), SLOT(onRasterSelected(int, int)));

  fillVectorialList();
  fillRasterList();

  pimpl_->ui_->geometryStc->setCurrentIndex(2);
}


terrama2::gui::config::IntersectionDialog::~IntersectionDialog()
{
  delete pimpl_;
}

void terrama2::gui::config::IntersectionDialog::onOkBtnClicked()
{
  accept();
}

void terrama2::gui::config::IntersectionDialog::onDatasetSelected()
{
  if(!pimpl_->ui_->themeList->currentItem())
    return;

  std::string item = pimpl_->ui_->themeList->currentItem()->text().toStdString();

  auto dsType = pimpl_->dataSource_->getDataSetType(item);
  fillAttributeTable(dsType);

}

void terrama2::gui::config::IntersectionDialog::fillAttributeTable(std::auto_ptr<te::da::DataSetType> dsType)
{
  if(dsType.get() == nullptr)
    return;

  pimpl_->ui_->vectorAttributesTableWidget->clearContents();


  pimpl_->ui_->vectorAttributesTableWidget->setRowCount(dsType->size());

  auto attrMap = pimpl_->intersection_.attributeMap();
  auto it = attrMap.find(dsType->getCompositeName());


  for(unsigned int j = 0; j < dsType->size(); ++j)
  {
    std::string propertyName = dsType->getProperty(j)->getName();
    QTableWidgetItem *newItem = new QTableWidgetItem(propertyName.c_str());
    pimpl_->ui_->vectorAttributesTableWidget->setItem(j, 0, newItem);

    if(it != attrMap.end())
    {
      auto found = std::find(it->second.begin(), it->second.end(), propertyName);
      if(found != it->second.end())
      {
        QTableWidgetItem* iconItem = new QTableWidgetItem(QIcon::fromTheme("confirm"), "");
        pimpl_->ui_->vectorAttributesTableWidget->setItem(j, 1, iconItem);
      }
    }
  }
}

void terrama2::gui::config::IntersectionDialog::onRasterSelected(int row, int column)
{

  auto tableItem = pimpl_->ui_->dynamicGridsTableWidget->item(row, 0);
  std::string datasetName = tableItem->text().toStdString();

  try
  {
    terrama2::core::DataSet dataSet = terrama2::core::DataManager::getInstance().findDataSet(datasetName);
    auto map = pimpl_->intersection_.bandMap();
    auto it = map.find(dataSet.id());
    if(it == map.end())
    {
      QTableWidgetItem* newItem = new QTableWidgetItem(QIcon::fromTheme("confirm"), "");
      pimpl_->ui_->dynamicGridsTableWidget->setItem(row, 1, newItem);
      map[dataSet.id()] = pimpl_->ui_->bandLed->text().toStdString();
      pimpl_->intersection_.setBandMap(map);
    }
    else
    {
      tableItem->setIcon(QIcon());
      map.erase(it);
      pimpl_->intersection_.setBandMap(map);
      QTableWidgetItem* newItem = new QTableWidgetItem("");
      pimpl_->ui_->dynamicGridsTableWidget->setItem(row, 1, newItem);
    }
  }
  catch(...)
  {
    QMessageBox::information(nullptr, "Error", "Could not retrieve dataset information");
  }
}


void terrama2::gui::config::IntersectionDialog::onAttributeSelected(int row, int column)
{
  auto item = pimpl_->ui_->vectorAttributesTableWidget->item(row, 0);

  std::string attrName = item->text().toStdString();

  auto tableItem = pimpl_->ui_->themeList->currentItem();
  if(!tableItem)
    return;

  std::string tableName = tableItem->text().toStdString();

  auto map = pimpl_->intersection_.attributeMap();
  auto it = map.find(tableName);
  if(it == map.end())
  {
    tableItem->setIcon(QIcon::fromTheme("confirm"));

    QTableWidgetItem* newItem = new QTableWidgetItem(QIcon::fromTheme("confirm"), "");
    pimpl_->ui_->vectorAttributesTableWidget->setItem(row, 1, newItem);
    std::vector<std::string> attrVec;
    attrVec.push_back(attrName);
    map[tableName] = attrVec;
    pimpl_->intersection_.setAttributeMap(map);
  }
  else
  {
    auto attrVec = it->second;
    auto itAttr = std::find(attrVec.begin(), attrVec.end(), attrName);
    if(itAttr == attrVec.end())
    {
      QTableWidgetItem* newItem = new QTableWidgetItem(QIcon::fromTheme("confirm"), "");
      pimpl_->ui_->vectorAttributesTableWidget->setItem(row, 1, newItem);
      attrVec.push_back(attrName);
      map[tableName] = attrVec;
      pimpl_->intersection_.setAttributeMap(map);
    }
    else
    {
      attrVec.erase(itAttr);
      if(attrVec.empty())
      {
        tableItem->setIcon(QIcon());
        pimpl_->intersection_.setAttributeMap(map);
        map.erase(tableName);
      }
      else
      {
        map[tableName] = attrVec;
      }

      QTableWidgetItem* newItem = new QTableWidgetItem("");
      pimpl_->ui_->vectorAttributesTableWidget->setItem(row, 1, newItem);
      pimpl_->intersection_.setAttributeMap(map);
    }
  }
}

void terrama2::gui::config::IntersectionDialog::fillVectorialList()
{
  std::string sql = "select * from geometry_columns where f_table_name != 'filter' and f_table_name not like 'storage%'";

  auto transactor = pimpl_->dataSource_->getTransactor();

  std::auto_ptr<te::da::DataSet> queryResult = transactor->query(sql);

  auto attrMap = pimpl_->intersection_.attributeMap();
  while(queryResult->moveNext())
  {
    std::string item = queryResult->getAsString("f_table_schema") + "."  + queryResult->getAsString("f_table_name");

    if(attrMap.find(item) != attrMap.end())
    {
      QListWidgetItem* tableItem = new QListWidgetItem(QIcon::fromTheme("confirm"), item.c_str());
      this->pimpl_->ui_->themeList->addItem(tableItem);
    }
    else
    {
      this->pimpl_->ui_->themeList->addItem(item.c_str());
    }
  }

  this->pimpl_->ui_->themeList->setCurrentRow(0);
}


void terrama2::gui::config::IntersectionDialog::fillRasterList()
{
  std::string sql = "select ds.name from terrama2.dataset ds, terrama2.dataset_type dt where ds.kind = dt.id and dt.name = 'Grid'";

  auto transactor = pimpl_->dataSource_->getTransactor();
  std::auto_ptr<te::da::DataSet> queryResult = transactor->query(sql);

  pimpl_->ui_->dynamicGridsTableWidget->setRowCount(queryResult->size());

  auto bandMap = pimpl_->intersection_.bandMap();

  int i = 0;
  while(queryResult->moveNext())
  {
    std::string datasetName = queryResult->getAsString("name");
    terrama2::core::DataSet dataSet = terrama2::core::DataManager::getInstance().findDataSet(datasetName);
    if(bandMap.find(dataSet.id()) != bandMap.end())
    {
      QTableWidgetItem* newItem = new QTableWidgetItem(QIcon::fromTheme("confirm"), "");
      pimpl_->ui_->dynamicGridsTableWidget->setItem(i, 1, newItem);
    }

    QTableWidgetItem* newItem = new QTableWidgetItem(datasetName.c_str());
    pimpl_->ui_->dynamicGridsTableWidget->setItem(i, 0, newItem);

    i++;

  }

}

void terrama2::gui::config::IntersectionDialog::showVectorialPage()
{
  pimpl_->ui_->geometryStc->setCurrentIndex(2);
  pimpl_->ui_->layerTypeStc->setCurrentIndex(0);
}

void terrama2::gui::config::IntersectionDialog::showGridPage()
{
  pimpl_->ui_->geometryStc->setCurrentIndex(3);
  pimpl_->ui_->layerTypeStc->setCurrentIndex(1);
}

terrama2::core::Intersection terrama2::gui::config::IntersectionDialog::getIntersection() const
{
  return pimpl_->intersection_;
}
