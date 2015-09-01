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
  \file terrama2/gui/config/ArchivingItemsListDialog.cpp

  \brief Definition ArchivingItemsListDialog.cpp class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ArchivingItemsListDialog.hpp"
#include "remoteWeatherServerDataList.h"

// STL
#include <assert.h>

//! Construtor
ArchivingItemsListDialog::ArchivingItemsListDialog(RemoteWeatherServerList* rwsl, AnalysisList* anaList, QList< QPair<QString, int> > conditionList, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(itemTypeCmb, SIGNAL(currentIndexChanged(int)), SLOT(setTypeChanged(int)));
  connect(itemNameCmb, SIGNAL(currentIndexChanged(int)), SLOT(setObjectChanged(int)));
  connect(actionCmb,   SIGNAL(currentIndexChanged(int)), SLOT(setActionChanged(int)));

  connect(conditionRemoveAllRdb, SIGNAL(toggled(bool)), SLOT(enableConditionDate(bool)));

  itemTypeCmb->blockSignals(true);
  itemTypeCmb->addItem(getTypeString(WS_ARRU_TYPE_DATASERVER), WS_ARRU_TYPE_DATASERVER);
  itemTypeCmb->addItem(getTypeString(WS_ARRU_TYPE_ANALYSIS), WS_ARRU_TYPE_ANALYSIS);
  itemTypeCmb->blockSignals(false);

  conditionRemoveAllRdb->setChecked(true);
  conditionSpb->setDisabled(true);
  conditionCmb->setDisabled(true);

  // The kind of first item is Fonte de Dados with the Remove action.
  createFilterCbx->setEnabled(true);
  createFilterCbx->setChecked(true);

  for(int i = 0; i < conditionList.size(); ++i)
    conditionCmb->addItem(conditionList[i].first, conditionList[i].second);

  _anaList = anaList;
  _rwsl = rwsl;

  if(_rwsl->count()) 
  {
    itemTypeCmb->setCurrentIndex(0); //Fonte de dados
    fillItemNameCmb(0);
  }
  else if(_anaList->count())
  {
    itemTypeCmb->setCurrentIndex(1); //Analise
    fillItemNameCmb(1);
  }
}

//! Destructor
ArchivingItemsListDialog::~ArchivingItemsListDialog()
{
}

void ArchivingItemsListDialog::setTypeChanged(int index)
{
  fillItemNameCmb(index);
}

void ArchivingItemsListDialog::setObjectChanged(int index)
{
  actionCmb->blockSignals(true);
  actionCmb->clear();

  bool deleteSurfaces = false;

  int type = itemTypeCmb->currentIndex();
  if(type == 0)
  {
    int wdsID = itemNameCmb->itemData(index).toInt();
    RemoteWeatherServerData* rwServerData = NULL;

    for(int i=0; i < (int)_rwsl->count() && !rwServerData; i++)
    {
      RemoteWeatherServer* map = _rwsl->at(i);
      RemoteWeatherServerDataList* remoteWeatherServerDataList = map->getRemoteWeatherServerDataList();

      if (remoteWeatherServerDataList != NULL)
        rwServerData = remoteWeatherServerDataList->findServerDataById(wdsID);
    }

    if(rwServerData && rwServerData->geometry() == WS_WDSGEOM_POINTS)
      deleteSurfaces = true;

    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_DATA), WS_ARRU_ACTION_DELETE_DATA);
    if(deleteSurfaces)
      actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_SURFACE), WS_ARRU_ACTION_DELETE_SURFACE);

    createFilterCbx->setEnabled(true);
    createFilterCbx->setChecked(true);
  }
  else if(type == 1)
  {
    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_LOG), WS_ARRU_ACTION_DELETE_LOG);
    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_WARNING), WS_ARRU_ACTION_DELETE_WARNING);
    createFilterCbx->setEnabled(false);
    createFilterCbx->setChecked(false);
  }

  actionCmb->blockSignals(false); 
}

void ArchivingItemsListDialog::setActionChanged(int index)
{
  if(itemTypeCmb->currentIndex() == 0)
  {
    if(index == 0)
    {
      createFilterCbx->setEnabled(true);
      createFilterCbx->setChecked(true);
    }
    else
    {
      createFilterCbx->setEnabled(false);
      createFilterCbx->setChecked(false);
    }
  }
}

void ArchivingItemsListDialog::enableConditionDate(bool enable)
{
  conditionSpb->setDisabled(enable);
  conditionCmb->setDisabled(enable);
}

void ArchivingItemsListDialog::fillItemNameCmb(int type)
{
  itemNameCmb->blockSignals(true);
  actionCmb->blockSignals(true);

  itemNameCmb->clear();
  actionCmb->clear();

  bool deleteSurfaces = false;

  if(type == 0)
  {
    for(int i=0; i < (int)_rwsl->count(); i++)
    {
      RemoteWeatherServer* map = _rwsl->at(i);
      RemoteWeatherServerDataList* remoteWeatherServerDataList = map->getRemoteWeatherServerDataList();

      if (remoteWeatherServerDataList != NULL)
      {
        for(int j=0; j < (int) remoteWeatherServerDataList->count(); j++)
        {
          RemoteWeatherServerData* rwsd = remoteWeatherServerDataList->at(j);
          itemNameCmb->addItem(rwsd->name(), rwsd->id());

          // If the kind of first element at list is PCD, allow surface remove
          if(i == 0 && j == 0 && rwsd->geometry() == WS_WDSGEOM_POINTS)
            deleteSurfaces = true;
        }
      }
    }

    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_DATA), WS_ARRU_ACTION_DELETE_DATA);
    if(deleteSurfaces)
      actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_SURFACE), WS_ARRU_ACTION_DELETE_SURFACE);

    createFilterCbx->setEnabled(true);
    createFilterCbx->setChecked(true);
  }
  else if(type == 1)
  {
    for(int i=0; i < (int)_anaList->count(); i++)
      itemNameCmb->addItem(_anaList->at(i)->name(), _anaList->at(i)->id());

    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_LOG), WS_ARRU_ACTION_DELETE_LOG);
    actionCmb->addItem(getActionString(WS_ARRU_ACTION_DELETE_WARNING), WS_ARRU_ACTION_DELETE_WARNING);
    createFilterCbx->setEnabled(false);
    createFilterCbx->setChecked(false);
  }

  itemNameCmb->blockSignals(false);
  actionCmb->blockSignals(false);
}

wsArchivingRule ArchivingItemsListDialog::getFields()
{
  wsArchivingRule rule;

  rule.itemID = itemNameCmb->itemData(itemNameCmb->currentIndex()).toInt();

  rule.type = (wsArchivingRuleType) itemTypeCmb->currentIndex();
  rule.name = itemNameCmb->currentText().toStdString();
  rule.action = (wsArchivingRuleAction) actionCmb->itemData(actionCmb->currentIndex()).toInt();
  if(conditionRemoveByDateRdb->isChecked())
    rule.condition = conditionSpb->value() * conditionCmb->itemData(conditionCmb->currentIndex()).toInt();
  else
    rule.condition = 0;

  rule.createFilter = createFilterCbx->isChecked();

  return rule;
}

void ArchivingItemsListDialog::setFields(QString type, int id, wsArchivingRuleAction action, int condValue, QString condStr, bool createFilter)
{
  itemTypeCmb->setCurrentIndex(itemTypeCmb->findText((type)));
  itemTypeCmb->setDisabled(true);

  itemNameCmb->setCurrentIndex(itemNameCmb->findData(id));
  itemNameCmb->setDisabled(true);

  actionCmb->setCurrentIndex(actionCmb->findData(action));

  if(condValue > 0)
  {
    conditionRemoveByDateRdb->setChecked(true);
    conditionSpb->setValue(condValue);
    conditionCmb->setCurrentIndex(conditionCmb->findText(condStr));
  }

  createFilterCbx->setChecked(createFilter);
}

QString ArchivingItemsListDialog::getActionString(wsArchivingRuleAction action)
{
  if(action == WS_ARRU_ACTION_DELETE_DATA)
    return QObject::tr("Apagar dados");
  if(action == WS_ARRU_ACTION_DELETE_LOG)
    return QObject::tr("Apagar log");
  if(action == WS_ARRU_ACTION_DELETE_WARNING)
    return QObject::tr("Apagar alertas");
  if(action == WS_ARRU_ACTION_DELETE_SURFACE)
    return QObject::tr("Apagar superfícies");

  assert(0);
  return QObject::tr("Apagar dados");
}

QString ArchivingItemsListDialog::getTypeString(wsArchivingRuleType type)
{
  if(type == WS_ARRU_TYPE_DATASERVER)
    return QObject::tr("Fonte de dados");
  if(type == WS_ARRU_TYPE_ANALYSIS)
    return QObject::tr("Análise");

  assert(0);
  return QObject::tr("Fonte de dados");
}

wsArchivingRuleType ArchivingItemsListDialog::getTypeWS(QString typeStr)
{
  if(typeStr == QObject::tr("Fonte de dados"))
    return WS_ARRU_TYPE_DATASERVER;
  if(typeStr == QObject::tr("Análise"))
    return WS_ARRU_TYPE_ANALYSIS;

  assert(0);
  return WS_ARRU_TYPE_DATASERVER;
}

