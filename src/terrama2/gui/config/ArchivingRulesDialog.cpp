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
  \file terrama2/gui/config/ArchivingRulesDialog.cpp

  \brief Definition ArchivingRulesDialog.cpp class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// TerraMA2
#include "ArchivingRulesDialog.hpp"

// STL
#include <assert.h>

// QT
#include <QMessageBox>
#include <QProcess>
#include <QHeaderView>
#include <QFile>

//! Constructor
ArchivingRulesDialog::ArchivingRulesDialog(QString configFileName, Services* manager, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _manager = manager;
  _remoteWeatherServerList = _manager->remoteWeatherServerList();
  _analysisList = _manager->analysisList();
  _configFileName = configFileName;

  _changed = false;
  saveBtn->setDisabled(true);
  progressBar->hide();

  archivingItemsTbl->horizontalHeader()->setHighlightSections(false);
  archivingItemsTbl->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  connect(saveBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(executeBtn, SIGNAL(clicked()), SLOT(executeArchiving()));
  connect(addItemBtn, SIGNAL(clicked()), SLOT(addItem()));
  connect(deleteItemBtn, SIGNAL(clicked()), SLOT(deleteItem()));
  connect(archivingItemsTbl,  SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(itemUpdate(QTableWidgetItem*)));
  connect(&_process, SIGNAL(readyReadStandardError()), SLOT(readArchivingError()));
  connect(&_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(archivingFinishedSlot(int)));

  _conditionList.clear();
  _conditionList.push_back(QPair<QString, int>(QObject::tr("Dia(s)"), 1));
  _conditionList.push_back(QPair<QString, int>(QObject::tr("Semana(s)"), 7));
  _conditionList.push_back(QPair<QString, int>(QObject::tr("Mês(es)"), 30));
  _conditionList.push_back(QPair<QString, int>(QObject::tr("Ano(s)"), 365));
}

//! Destructor
ArchivingRulesDialog::~ArchivingRulesDialog()
{
}

void ArchivingRulesDialog::executeArchiving()
{ 
  if(_changed)
  { 
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(this, tr("O banco de dados deve ser atulalizado antes da execução"), 
        tr("Deseja salvar as regras de arquivamento?"),
      QMessageBox::Yes | QMessageBox::Cancel,
      QMessageBox::Cancel);

    if(answer == QMessageBox::Cancel)
      return;

    // Save the archiving roles
    std::vector<struct wsArchivingRule> rules = getFields();
    _manager->saveArchivingRules(rules);
    
    // Update the table with the generated IDS for each archiving role in database
    std::vector<struct wsArchivingRule> archivingRules;
    _manager->getArchivingRules(archivingRules);
    setFields(archivingRules);

    saveBtn->setDisabled(true);
    _changed = false;
  }

  QList<QTableWidgetItem*> selectedItems = archivingItemsTbl->selectedItems();

  QStringList arguments;
  arguments.push_back(_configFileName);

  QTableWidgetItem* item;

  if (selectedItems.size() > 0)
  {
    for(int i=0; i<selectedItems.size(); i++)
    {
      item = selectedItems.at(i);
      if(item->column() != 0)
        continue;

      arguments.push_back(item->data(Qt::UserRole).toString());
    }
  }
  else
  {
    QMessageBox::warning(this, tr("Resultado"), tr("Nenhuma regra de arquivamento está selecionada!\nO arquivamento não foi realizado!"), QMessageBox::Ok);
    return;
  }

  enableButtons(false);
  QString archivingProgramPath = QCoreApplication::applicationDirPath() + "/arquivador";

  #ifdef WIN32
    archivingProgramPath.append(".exe");
  #endif

  // Check if the "arquivador" program is at correct folder and it can be executed
  if(!QFile::exists(archivingProgramPath))
  {
    enableButtons(true);
    QMessageBox::warning(this, tr("Resultado"), tr("O arquivamento não foi realizado!\n Programa \"arquivador\" não foi encontrado."), QMessageBox::Ok);
    return;
  }

  _process.start(archivingProgramPath, arguments);
}

void ArchivingRulesDialog::readArchivingError()
{
  QMessageBox::warning(this, tr("Resultado"), _process.readAllStandardError(), QMessageBox::Ok);
}

void ArchivingRulesDialog::archivingFinishedSlot(int exitCode)
{
  if(exitCode)
  {
    QMessageBox::information(this, tr("Resultado"), tr("Dados removidos corretamente"), QMessageBox::Ok);
  }

  enableButtons(true);
}

void ArchivingRulesDialog::enableButtons(bool enable)
{
  progressBar->setVisible(!enable);

  archivingItemsTbl->setEnabled(enable);
  addItemBtn->setEnabled(enable);
  deleteItemBtn->setEnabled(enable);
  executeBtn->setEnabled(enable);
  saveBtn->setEnabled(enable);
  cancelBtn->setEnabled(enable);
}

void ArchivingRulesDialog::addItem()
{
  ArchivingItemsListDialog dlg(_remoteWeatherServerList, _analysisList, _conditionList);
  
  if (dlg.exec() == QDialog::Accepted)  
  {
    _changed = true;
    saveBtn->setDisabled(false);

    wsArchivingRule newRule = dlg.getFields();
    
    QString newItemType(ArchivingItemsListDialog::getTypeString(newRule.type));
    QString newItemName(newRule.name.c_str());
    QString newItemAction(ArchivingItemsListDialog::getActionString(newRule.action));
    QString newItemCondition(conditionIntToStr(newRule.condition));
    QString newItemCreateFilter = createFilterToStr(newRule.type, newRule.createFilter);

    if(newItemType.isEmpty() || newItemName.isEmpty() || 
       newItemAction.isEmpty() || newItemCondition.isEmpty() || newItemCreateFilter.isEmpty())
    {
      QMessageBox::warning(this, tr("Ação cancelada"), tr("Dados incompletos!"));
      return;
    }

    int nRows = archivingItemsTbl->rowCount();
    for(int i=0; i<nRows; ++i)
    {
      if(archivingItemsTbl->item(i, 0)->text() == newItemType)
        if(archivingItemsTbl->item(i, 1)->text() == newItemName)
          if(archivingItemsTbl->item(i, 2)->text() == newItemAction)
          {
            QMessageBox::warning(this, tr("Ação cancelada"), tr("Item já está presente na tabela!"));
            return;
          }
    }

    QTableWidgetItem* itemType = new QTableWidgetItem(newItemType);
    QTableWidgetItem* itemName = new QTableWidgetItem(newItemName);
    QTableWidgetItem* itemAction = new QTableWidgetItem(newItemAction);
    QTableWidgetItem* itemCondition = new QTableWidgetItem(newItemCondition);
    QTableWidgetItem* itemCreateFilter = new QTableWidgetItem(newItemCreateFilter);

    itemType->setData(Qt::UserRole, -1); //Como a regra ainda nao foi inserida no banco de dados, nao possui ID
    itemName->setData(Qt::UserRole, newRule.itemID);
    itemAction->setData(Qt::UserRole, (int)newRule.action);
    itemCondition->setData(Qt::UserRole, newRule.condition);
    itemCreateFilter->setData(Qt::UserRole, newRule.createFilter);

    archivingItemsTbl->setRowCount(nRows + 1);

    archivingItemsTbl->setItem(nRows, 0, itemType);
    archivingItemsTbl->setItem(nRows, 1, itemName);
    archivingItemsTbl->setItem(nRows, 2, itemAction);
    archivingItemsTbl->setItem(nRows, 3, itemCondition);
    archivingItemsTbl->setItem(nRows, 4, itemCreateFilter);
  }
}

void ArchivingRulesDialog::deleteItem()
{
  _changed = true;
  saveBtn->setDisabled(false);
  
  QList<QTableWidgetItem*> selectedItems = archivingItemsTbl->selectedItems();

  QList<int> itemsToRemove;
  QTableWidgetItem* item;

  if (selectedItems.size() > 0)
  {
    itemsToRemove.clear();

    for(int i=0; i<selectedItems.size(); i++)
    {
      item = selectedItems.at(i);
      if(item->column() != 0)
        continue;

      itemsToRemove.push_front(item->row());
    }

    qSort(itemsToRemove.begin(), itemsToRemove.end());

    for(int i = 0; i < itemsToRemove.size();)
      archivingItemsTbl->removeRow(itemsToRemove.takeLast());
  }
}

void ArchivingRulesDialog::itemUpdate(QTableWidgetItem* item)
{
  if(item != NULL)
  {
        ArchivingItemsListDialog dlg(_remoteWeatherServerList, _analysisList, _conditionList);

    int itemRow = item->row();

    int condition = archivingItemsTbl->item(itemRow, 3)->data(Qt::UserRole).toInt();
    int condValue = 0;
    QString condStr = "";
    getConditionIntAndStr(condition, condValue, condStr);

    dlg.setFields(archivingItemsTbl->item(itemRow, 0)->text(),
            archivingItemsTbl->item(itemRow, 1)->data(Qt::UserRole).toInt(),
            (wsArchivingRuleAction) archivingItemsTbl->item(itemRow, 2)->data(Qt::UserRole).toInt(),
            condValue,
            condStr,
            archivingItemsTbl->item(itemRow, 4)->data(Qt::UserRole).toBool());
    
    if (dlg.exec() == QDialog::Accepted)
    {
      _changed = true;
      saveBtn->setDisabled(false);
      wsArchivingRule newRule = dlg.getFields();

      int nRows = archivingItemsTbl->rowCount();
      for(int i=0; i<nRows; ++i)
      {
        if(i != itemRow && archivingItemsTbl->item(i, 0)->text() == ArchivingItemsListDialog::getTypeString(newRule.type))
          if(archivingItemsTbl->item(i, 1)->text().toStdString() == newRule.name)
            if(archivingItemsTbl->item(i, 2)->data(Qt::UserRole).toInt() == newRule.action)
            {
              QMessageBox::warning(this, tr("Ação cancelada"), tr("Item já está presente na tabela!"));
              return;
            }
      }

      archivingItemsTbl->item(itemRow, 2)->setText(ArchivingItemsListDialog::getActionString(newRule.action));
      archivingItemsTbl->item(itemRow, 2)->setData(Qt::UserRole, (int)newRule.action);
      archivingItemsTbl->item(itemRow, 3)->setText(conditionIntToStr(newRule.condition));
      archivingItemsTbl->item(itemRow, 3)->setData(Qt::UserRole, newRule.condition);
      archivingItemsTbl->item(itemRow, 4)->setText(createFilterToStr(newRule.type, newRule.createFilter));
      archivingItemsTbl->item(itemRow, 4)->setData(Qt::UserRole, newRule.createFilter);
    }
  }
}

std::vector<struct wsArchivingRule> ArchivingRulesDialog::getFields()
{
  std::vector<struct wsArchivingRule> archivingRules;
  wsArchivingRule archivingRule;

  int nRows = archivingItemsTbl->rowCount();

  for(int i=0; i<nRows; ++i)
  {
    QString typeStr = archivingItemsTbl->item(i, 0)->text();
    archivingRule.type = ArchivingItemsListDialog::getTypeWS(typeStr);
    
    archivingRule.name = archivingItemsTbl->item(i, 1)->text().toStdString();
    archivingRule.action = (wsArchivingRuleAction) archivingItemsTbl->item(i, 2)->data(Qt::UserRole).toInt();
    archivingRule.condition = archivingItemsTbl->item(i, 3)->data(Qt::UserRole).toInt();
    archivingRule.createFilter = archivingItemsTbl->item(i, 4)->data(Qt::UserRole).toBool();

    archivingRule.ruleID = archivingItemsTbl->item(i, 0)->data(Qt::UserRole).toInt();
    archivingRule.itemID = archivingItemsTbl->item(i, 1)->data(Qt::UserRole).toInt();

    archivingRules.push_back(archivingRule);
  }

  return archivingRules;
}

void ArchivingRulesDialog::setFields(std::vector<struct wsArchivingRule> archivingRules)
{
  int size = archivingRules.size();
  archivingItemsTbl->setRowCount(archivingRules.size());
  wsArchivingRule rule;

  for(int i=0; i<archivingRules.size(); ++i)
  {
    rule = archivingRules.at(i);

    QTableWidgetItem* itemType = new QTableWidgetItem(ArchivingItemsListDialog::getTypeString(rule.type));
    QTableWidgetItem* itemName = new QTableWidgetItem(rule.name.c_str());
    QTableWidgetItem* itemAction = new QTableWidgetItem(ArchivingItemsListDialog::getActionString(rule.action));
    QTableWidgetItem* itemCondition = new QTableWidgetItem(conditionIntToStr(rule.condition));
    QTableWidgetItem* itemCreateFilter = new QTableWidgetItem(createFilterToStr(rule.type, rule.createFilter));

    itemType->setData(Qt::UserRole, rule.ruleID);
    itemName->setData(Qt::UserRole, rule.itemID);
    itemAction->setData(Qt::UserRole, (int)rule.action);
    itemCondition->setData(Qt::UserRole, rule.condition);
    itemCreateFilter->setData(Qt::UserRole, rule.createFilter);

    archivingItemsTbl->setItem(i, 0, itemType);
    archivingItemsTbl->setItem(i, 1, itemName);
    archivingItemsTbl->setItem(i, 2, itemAction);
    archivingItemsTbl->setItem(i, 3, itemCondition);
    archivingItemsTbl->setItem(i, 4, itemCreateFilter);
  }
}

QString ArchivingRulesDialog::conditionIntToStr(int cond)
{
  if(cond == 0)
    return QObject::tr("Remover todos os dados");

  int value = 0;
  QString str = "";

  getConditionIntAndStr(cond, value, str);

  return QString("%1 %2").arg(value).arg(str);
}

// From an condition (int cond), calculate the representation values in string format
// Example: If cond == 14 then the final values will be: value=2; str="Semana(s)"
void ArchivingRulesDialog::getConditionIntAndStr(int cond, int& value, QString& str)
{
  if(cond % _conditionList[3].second == 0)
  {
    str = _conditionList[3].first;
    value = cond/_conditionList[3].second;
  }
  else if(cond % _conditionList[2].second == 0)
  {
    str = _conditionList[2].first;
    value = cond/_conditionList[2].second;
  }
  else if(cond % _conditionList[1].second == 0)
  {
    str = _conditionList[1].first;
    value = cond/_conditionList[1].second;
  }
  else
  {
    str = _conditionList[0].first;
    value = cond;
  }
}

QString ArchivingRulesDialog::createFilterToStr(wsArchivingRuleType ruleType, bool createFilter)
{
  if(ruleType == WS_ARRU_TYPE_DATASERVER)
  {
    if(createFilter)
      return QObject::tr("Sim");
    else
      return QObject::tr("Não");
  }
  else
  {
    return QString(" - ");
  }
}
