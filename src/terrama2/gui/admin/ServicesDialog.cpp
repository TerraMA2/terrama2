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

//#include "../core/DataType.hpp"
//#include "ServiceManager.hpp"

// QT
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

struct ServicesDialog::Impl
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

//! Construtor
ServicesDialog::ServicesDialog(AdminApp* adminapp, ConfigManager& configData, QString nameConfig)
    : adminapp_(adminapp), pimpl_(new Impl), configManager_(configData), idNameConfig_(nameConfig), changed_(false)
{
 pimpl_->ui_->setupUi(this);

 QJsonObject selectedMetadata = configManager_.getfiles().take(nameConfig);
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

 setDialogData(nameConfig);
}

//! Destrutor
ServicesDialog::~ServicesDialog()
{
}

//! Marca que os dados da tabela foram alterados
void ServicesDialog::setDataChanged(int row, int col)
{
  if(changed_ || col == 0)
    return;
  changed_ = true;
  pimpl_->ui_->saveBtn->setEnabled(true);
}

//! Marca que os dados não foram alterados
void ServicesDialog::clearDataChanged()
{
  if(!changed_)
    return;
  changed_ = false;
  pimpl_->ui_->saveBtn->setEnabled(false);
}

//! Preenche uma linha da tabela de dados
void ServicesDialog::setLine(int line, const QString& module, const CommonData& data)
{
  QTableWidgetItem* item;

// Coluna Ativo
  item = new QTableWidgetItem(QIcon::fromTheme("ping-unknown"), "");
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 0, item);

// Coluna Modulo
  item = new QTableWidgetItem(module);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 1, item);

// Coluna Localização
  item = new QTableWidgetItem(data.address_ + QString(" : %1").arg(data.servicePort_));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  pimpl_->ui_->servicesTable->setItem(line, 2, item);

// Coluna Comando
  pimpl_->ui_->servicesTable->setItem(line, 3, new QTableWidgetItem(data.cmd_));

// Coluna Parâmetros
  pimpl_->ui_->servicesTable->setItem(line, 4, new QTableWidgetItem(data.params_));

}

//! Preenche tabela com dados de configuração
void ServicesDialog::setDialogData(QString nameConfig)
{
// Preenche nome da configuração
 pimpl_->ui_->configLbl->setText(nameConfig);

// Ajusta tamanho da tabela
 pimpl_->ui_->servicesTable->setRowCount(4);

// Preenche a tabela com dados da Coleta
 setLine(0, tr("Coleta"),*configManager_.getCollection());

// Ajusta espessura das linhas
 pimpl_->ui_->servicesTable->resizeRowsToContents();

// Marca dados como não alterados
 clearDataChanged();
}


//! Preenche estrutura com dados dos campos de comando e parâmetros da linha da tabela
void ServicesDialog::getLine(int line, CommonData& data)
{
  QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(line, 3);
  data.cmd_ = item->data(Qt::DisplayRole).toString().trimmed();

  item = pimpl_->ui_->servicesTable->item(line, 4);
  data.params_ = item->data(Qt::DisplayRole).toString().trimmed();
}

//! Retorna lista contendo indices das linhas selecionada
void ServicesDialog::getSelectedLines(QList<int>& list)
{
  list.clear();

  QModelIndexList indexlist = pimpl_->ui_->servicesTable->selectionModel()->selectedRows();

  for(int i=0, size=indexlist.size(); i<size; i++)
    list.push_back(indexlist[i].row());
}


//! Preenche estrutura com dados obtidos do dialogo
void ServicesDialog::getDialogData(QString nameConfig)
{
  // Obtem dados das linhas padronizadas
  getLine(0, *configManager_.getCollection());

}

//! Sinal chamado quando o usuário solicita que as linhas marcadas sejam "pingadas"
void ServicesDialog::verifyRequested()
{
  // Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."),
                               tr("Selecione na tabela um ou mais módulos a serem verificados."));
    return;
  }

  // Passa todas as linhas para o estado de ?
  int size=lines.size();
  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
    item->setIcon(QIcon::fromTheme("ping-unknown"));
  }
  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Efetua o ping para cada linha selecionada
  //const ConfigData& cfg = _configManager.configuration(_id);
  for(int i=0; i<size; i++)
  {
    //bool ok = ServiceManager::ping(cfg, lines[i]);
      bool ok;

    QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
    item->setIcon(QIcon::fromTheme(ok ? "ping-success" : "ping-error"));
    QCoreApplication::processEvents();
  }

  QApplication::restoreOverrideCursor();
}

//! Salva dados alterados no diálogo
void ServicesDialog::saveRequested()
{
  getLine(0, *configManager_.getCollection());

  adminapp_->save();

// Desmarca alterações
  clearDataChanged();
}

//! Executa o comando recebido como parâmetro
bool ServicesDialog::runCmd(int line, QString cmd, QString param, QString& err)
{
  // Verifica se o comando existe e é executável
  QFileInfo info(cmd);
  QString dir = QCoreApplication::applicationDirPath();
  if(!info.exists())
  {
 // Procura no diretório do executável
    info.setFile(dir + "/" + cmd);
    if(!info.exists())
    {
      err = tr("Comando inexistente: %1").arg(cmd);
          return false;
    }
    cmd = dir + "/" + cmd;
  }
  if(!info.isExecutable())
  {
    err = tr("Comando não é executável: %1").arg(cmd);
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

   // const ConfigData& conf = _configManager.configuration(_id);


    QString address;

    if (line == 0)
      address = configManager_.getCollection()->address_;

  //  else if(line == 1) address = conf.plans()._address;
  //  else if(line == 2) address = conf.notification()._address;
  //  else if(line == 3) address = conf.animation()._address;
  //  else if(line == 4) address = conf.analysis()._address;
  //  else               address = conf.analysis()._instanceData[line-5]._address;

  //  param.replace("%c", QString("\"%1\"").arg(_configManager.configurationFile(_id)), Qt::CaseInsensitive);
  //  param.replace("%i", (line<4) ? "0" : QString::number(line-3), Qt::CaseInsensitive);
  //  param.replace("%m", module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
  //  param.replace("%p", dir + "/" + module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
  //  param.replace("%a", address, Qt::CaseInsensitive);
  }

  // Executa
  QString cmdline = cmd + " " + param;
  if(!QProcess::startDetached(cmdline))
  {
    err = tr("Erro executando comando: %1").arg(cmdline);
    return false;
  }
  return true;
}

//! Sinal chamado quando o usuário solicita que as linhas marcadas sejam executadas
void ServicesDialog::execRequested()
{
  // Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);

  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."),
                               tr("Selecione na tabela um ou mais módulos a serem executados."));
    return;
  }

  // Executa comandos
  QString err;
  int errCount = 0;
  for(int i=0, size=lines.size(); i<size; i++)
  {
    QString cmdErr;
    CommonData data;

    getLine(lines[i], data);
    if(!runCmd(lines[i], data.cmd_, data.params_, cmdErr))
    {
      err += cmdErr + "\n";
      errCount++;
    }
  }

  // Mostra mensagens de erro
  if(errCount)
  {
    if(errCount == lines.size())
    {
      QMessageBox::warning(this, tr("Erro executando comandos..."),
                                 tr("Nenhum dos módulos selecionados foi executado com sucesso.\n"
                                    "Erros:\n\n%1").arg(err));
    }
    else
    {
      QMessageBox::warning(this, tr("Erro executando comandos..."),
                                 tr("%1 dos %2 módulos selecionados foram executados com sucesso.\n"
                                    "Utilize a ferramenta 'Verificar conexão' para testar\n"
                                    "se estes serviços foram inicializados corretamente.\n\n"
                                    "Erros na inicialização dos demais módulos:\n\n%3")
                                    .arg(lines.size()-errCount).arg(lines.size()).arg(err));
    }
  }
  else
  {
    QMessageBox::information(this, tr("Inicialização concluída"),
                                   tr("Módulos executados com sucesso.\n\n"
                                      "Utilize a ferramenta 'Verificar conexão' para testar\n"
                                      "se todos os serviços foram inicializados corretamente."));
  }
}

//! Sinal chamado quando o usuário solicita que as linhas marcadas sejam "fechadas"
void ServicesDialog::closeRequested()
{
// Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."),
                               tr("Selecione na tabela um ou mais módulos a serem finalizados."));
    return;
  }

// Passa todas as linhas para o estado de ?
  int size=lines.size();
  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = pimpl_->ui_->servicesTable->item(lines[i], 0);
    item->setIcon(QIcon::fromTheme("ping-unknown"));
  }
  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

// Envia solicitação de finalização para cada linha selecionada
  QString err;
  int errCount = 0;

 // const ConfigData& cfg = _configManager.configuration(_id);

  for(int i=0; i<size; i++)
  {
   // bool ok = ServiceManager::close(cfg, lines[i]);
      bool ok;
    if(!ok)
    {
      QString modname = pimpl_->ui_->servicesTable->item(lines[i], 1)->text();
      err += tr("Erro enviando solicitação de finalização para o módulo %1\n").arg(modname);
      errCount++;
    }
  }
  QApplication::restoreOverrideCursor();

  // Mostra mensagens de erro ou sucesso
  if(errCount)
  {
    if(errCount == lines.size())
    {
      QMessageBox::warning(this, tr("Erro enviando solicitações..."),
                                 tr("Nenhum dos módulos selecionados foi notificado com sucesso.\n"));
    }
    else
    {
      QMessageBox::warning(this, tr("Erro enviando solicitações..."),
                                 tr("%1 dos %2 módulos selecionados foram notificados com sucesso.\n"
                                    "Utilize a ferramenta 'Verificar conexão' para testar\n"
                                    "se estes serviços foram finalizados corretamente.\n\n"
                                    "Erros no envio de solicitação aos demais módulos:\n\n%3")
                                    .arg(lines.size()-errCount).arg(lines.size()).arg(err));
    }
  }
  else
  {
    QMessageBox::information(this, tr("Solicitação concluída"),
                                   tr("Solicitações de finalização enviadas com sucesso.\n\n"
                                      "Utilize a ferramenta 'Verificar conexão' para testar\n"
                                      "se todos os serviços foram finalizados corretamente."));
  }
}
