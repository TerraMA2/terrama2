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
  \file terrama2/gui/admin/ServicesDialog.hpp

  \brief Services QT Dialog

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/

// STL
#include <assert.h>

// TerraMA2
#include "ServicesDialog.hpp"
#include "../core/ConfigManager.hpp"
#include "AdminApp.hpp"
#include "../Exception.hpp"
#include "../../ws/collector/client/WebProxyAdapter.hpp"

// QT
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QThread>
#include <QDebug>

struct terrama2::gui::admin::ServicesDialog::Impl
{
  Ui::ServicesDialogForm* ui_;
  
  Impl()
    : ui_(new Ui::ServicesDialogForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }
};

//! Constructor
terrama2::gui::admin::ServicesDialog::ServicesDialog(terrama2::gui::admin::AdminApp* adminapp, terrama2::gui::core::ConfigManager& configData, QString nameConfig)
    : QDialog(adminapp), adminapp_(adminapp), pimpl_(new Impl), configManager_(configData), idNameConfig_(nameConfig), changed_(false)
{
 pimpl_->ui_->setupUi(this);

 QJsonObject selectedMetadata = configManager_.getfiles().take(idNameConfig_);
 configManager_.setDataForm(selectedMetadata);

 connect(pimpl_->ui_->cancelBtn,       SIGNAL(clicked()), SLOT(reject()));
 connect(pimpl_->ui_->verifyBtn,       SIGNAL(clicked()), SLOT(verifyRequested()));
 connect(pimpl_->ui_->executeBtn,      SIGNAL(clicked()), SLOT(execRequested()));
 connect(pimpl_->ui_->saveBtn,         SIGNAL(clicked()), SLOT(saveRequested()));
 connect(pimpl_->ui_->closeServiceBtn, SIGNAL(clicked()), SLOT(closeRequested()));

 connect(pimpl_->ui_->servicesTable, SIGNAL(cellChanged(int, int)), SLOT(setDataChanged(int, int)));

 pimpl_->ui_->servicesTable->setColumnWidth(0, 40);
 pimpl_->ui_->servicesTable->setAlternatingRowColors(true);
 pimpl_->ui_->servicesTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
 pimpl_->ui_->servicesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
 pimpl_->ui_->servicesTable->horizontalHeader()->setHighlightSections(false);
 pimpl_->ui_->servicesTable->horizontalHeader()->setStretchLastSection(true);

 pimpl_->ui_->closeServiceBtn->setEnabled(false);

 std::string host = "http://localhost:" + std::to_string(configManager_.getCollection()->servicePort_);
// std::string host = "http://" + configManager_.getCollection()->address_.toStdString() + ":" + std::to_string(configManager_.getCollection()->servicePort_);
 terrama2::ws::collector::client::WebProxyAdapter* webProxyAdapter = new terrama2::ws::collector::client::WebProxyAdapter(host);
 client = new terrama2::ws::collector::client::Client(webProxyAdapter);

 setDialogData(idNameConfig_);
}

//! Destructor
terrama2::gui::admin::ServicesDialog::~ServicesDialog()
{
}

// Mark the table data has changed
void terrama2::gui::admin::ServicesDialog::setDataChanged(int row, int col)
{
  if(changed_ || col == 0)
    return;
  changed_ = true;
  pimpl_->ui_->saveBtn->setEnabled(true);
}

// Mark the data have not changed
void terrama2::gui::admin::ServicesDialog::clearDataChanged()
{
  if(!changed_)
    return;
  changed_ = false;
  pimpl_->ui_->saveBtn->setEnabled(false);
}

// Fills a line of the data table
void terrama2::gui::admin::ServicesDialog::setLine(int line, const QString& module, const terrama2::gui::core::CommonData& data)
{
  QTableWidgetItem* item;

// Active column
  item = new QTableWidgetItem(QIcon::fromTheme("ping-unknown"), "");
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 0, item);

// Module column
  item = new QTableWidgetItem(module);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 1, item);

// Location column
  item = new QTableWidgetItem(data.address_ + QString(" : %1").arg(data.servicePort_));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 2, item);

// Command column
  pimpl_->ui_->servicesTable->setItem(line, 3, new QTableWidgetItem(data.cmd_));

// Parameters column
  pimpl_->ui_->servicesTable->setItem(line, 4, new QTableWidgetItem(data.params_));

}

// Fill table with configuration data
void terrama2::gui::admin::ServicesDialog::setDialogData(QString nameConfig)
{
// Fills configuration name
  pimpl_->ui_->configLbl->setText(nameConfig);

// Adjusting table size
  pimpl_->ui_->servicesTable->setRowCount(1); /* Adding line in table */

// Populates the table with data collection
  setLine(0, tr("Coleta"),*configManager_.getCollection());

// Adjusting the thickness (espessura) of the lines
  pimpl_->ui_->servicesTable->resizeRowsToContents();

// Mark data as unchanged
  clearDataChanged();
}

// Structure fills with data from field commands and table line parameters
void terrama2::gui::admin::ServicesDialog::getLine(int line, terrama2::gui::core::CommonData& data)
{
  QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(line, 3);
  data.cmd_ = item->data(Qt::DisplayRole).toString().trimmed();

  item = pimpl_->ui_->servicesTable->item(line, 4);
  data.params_ = item->data(Qt::DisplayRole).toString().trimmed();
}

// Returns list with indices of selected lines
void terrama2::gui::admin::ServicesDialog::getSelectedLines(QList<int>& list)
{
  list.clear();

  QModelIndexList indexlist = pimpl_->ui_->servicesTable->selectionModel()->selectedRows();

  for(int i=0, size=indexlist.size(); i<size; i++)
    list.push_back(indexlist[i].row());
}

// Signal called when the user requests that marked lines are "checked with ping"
void terrama2::gui::admin::ServicesDialog::verifyRequested()
{
// Get list of selected lines
  QList<int> lines;
  getSelectedLines(lines);

  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Error..."),
                               tr("Select the table one or more modules to be checked"));
    return;
  }

// Changes all the lines for the state (ping-unknown)
  int size=lines.size();

  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
    item->setIcon(QIcon::fromTheme("ping-unknown"));
  }

  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  bool ok;
  std::string answer;

  if (client!= nullptr)
  {
    try
    {
      for(int i=0; i<size; i++)
      {

        client->ping(answer);

        if (answer.empty())
         {
          ok = false;
          pimpl_->ui_->closeServiceBtn->setEnabled(false);
         }
        else
         {
          ok = true;
          pimpl_->ui_->closeServiceBtn->setEnabled(true);
         }

        QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
        item->setIcon(QIcon::fromTheme(ok ? "ping-success" : "ping-error"));
        QCoreApplication::processEvents();
      }
    }
    catch(const terrama2::Exception& e)
    {
      QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[0], 0);
      item->setIcon(QIcon::fromTheme("ping-error"));
      QCoreApplication::processEvents();
    }
    catch(std::exception& e)
    {
      qDebug() << "ServicesDialog::verifyRequested " << e.what();
    }
    catch(...)
    {
      qDebug() << "ServicesDialog::verifyRequested unkown error";
    }
  }
  QApplication::restoreOverrideCursor();
}

// Save changed data in the dialog
void terrama2::gui::admin::ServicesDialog::saveRequested()
{
  getLine(0, *configManager_.getCollection());

  adminapp_->save();

// clear changes
  clearDataChanged();
}

//! Executes the command received as a parameter
bool terrama2::gui::admin::ServicesDialog::runCmd(int line, QString cmd, QString param, QString& err)
{
// Check if the command exists and is executable
// TODO: verify function for Windows;
  cmd = "terrama2_mod_ws_collector_appserver";
  QFileInfo info(cmd);
  QString dir = QCoreApplication::applicationDirPath();
  if(!info.exists())
  {
    // Find the executable directory
    info.setFile(dir + "/" + cmd);
    if(!info.exists())
    {
      err = tr("Command does not exist: %1").arg(cmd);
      return false;
    }
    cmd = dir + "/" + cmd;
  }
  if(!info.isExecutable())
  {
    err = tr("Command is not executable: %1").arg(cmd);
    return false;
  }

  /* Efetua substituições nos parâmetros:
       %c = Path do arquivo de configuração
       %i = Número da instância para análise
       %m = Nome do módulo [analise, coleta, planos ou notificacao]
       %a = Nome da máquina
  */

  {
    static const char* module_names[] = {"coleta", "planos", "notificacao", "animacao", "analise"};

    QString address;
    QString dirfilename;

    QJsonObject fileSeleted = configManager_.getfiles().take(idNameConfig_);
    dirfilename = fileSeleted.take("path").toString();

    if (line == 0)
      address = configManager_.getCollection()->address_;

    if (param != "%c")
    {
      QMessageBox::warning(this, tr("Error..."),
                           tr("Parameters not exists!."));
      return false;
    }
    else
    {
      param.replace("%c", QString("\"%1\"").arg(dirfilename), Qt::CaseInsensitive);
      param.replace("%m", module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
      param.replace("%p", dir + "/" + module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
      param.replace("%a", address, Qt::CaseInsensitive);
    }
  }

  // Execute
  if (param == "")
  {
    QMessageBox::warning(this, tr("Error..."),
                         tr("Parameters not exists!."));
    return false;
  }
  else
  {
  //  QString cmdline = "./" + cmd + " " + QString::number(configManager_.getCollection()->servicePort_) + " " + param;
     QString cmdline = "./" + cmd + " " + param;

    pimpl_->ui_->closeServiceBtn->setEnabled(true);

    if(!QProcess::startDetached(cmdline))
    {
      err = tr("Error executing command: %1").arg(cmdline);
      return false;
    }
  }
  return true;
}

// Signal called when the user requests that marked lines are executed
void terrama2::gui::admin::ServicesDialog::execRequested()
{
// Get list of selected lines
  QList<int> lines;
  getSelectedLines(lines);

  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Error..."),
                               tr("Select the table one or more modules to be executed."));
    return;
  }

// Execute commands
  QString err;
  int errCount = 0;
  for(int i=0, size=lines.size(); i<size; i++)
  {
    QString cmdErr;
    terrama2::gui::core::CommonData data;

    getLine(lines[i], data);
    if(!runCmd(lines[i], data.cmd_, data.params_, cmdErr))
    {
      err += cmdErr + "\n";
      errCount++;
    }
  }
// FIXME: temporary, sleep function
// Wait a second to receive the SOAP response
  QThread::sleep(1);
  verifyRequested();
}

// Signal called when the user requests that marked lines are "closed"
void terrama2::gui::admin::ServicesDialog::closeRequested()
{
// Get list of selected lines
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Error..."),
                               tr("Select the table one or more modules to be finalized."));
    return;
  }

// Change all the lines to the state (ping-unknown)
  int size=lines.size();
  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
    item->setIcon(QIcon::fromTheme("ping-unknown"));
  }
  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

// Send completion request for each selected row  
  if (client!= nullptr)
  {
    try
    {
      client->shutdown();
      pimpl_->ui_->closeServiceBtn->setEnabled(false);
    }
    catch(const terrama2::Exception& e)
    {
      //TODO: review error message
      //TODO: log thid

      qDebug() << "ServicesDialog::closeRequested " << boost::get_error_info<terrama2::ErrorDescription>(e);
    }
    catch(std::exception& e)
    {
      //TODO: review error message
      //TODO: log thid

      qDebug() << "ServicesDialog::closeRequested " << e.what();
    }
    catch(...)
    {
      qDebug() << "ServicesDialog::verifyRequested unkown error";
    }
  }

  QApplication::restoreOverrideCursor();

  verifyRequested();

}
