/************************************************************************************
TerraMA� is a development platform of Monitoring, Analysis and Alert for
geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
tailings basins, epidemics and others.Copyright � 2012 INPE.

This code is part of the TerraMA� framework.
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

#include <assert.h>

#include <QHeaderView>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>

#include "ServicesDialog.h"
#include "serviceManager.h"


//! Construtor
ServicesDialog::ServicesDialog(ConfigDataManager& configManager, int id, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f), _configManager(configManager), _id(id), _changed(false)
{
  setupUi(this);

  connect(cancelBtn,       SIGNAL(clicked()), SLOT(reject()));
  connect(saveBtn,         SIGNAL(clicked()), SLOT(saveRequested()));
  connect(executeBtn,      SIGNAL(clicked()), SLOT(execRequested()));
  connect(verifyBtn,       SIGNAL(clicked()), SLOT(verifyRequested()));
  connect(closeServiceBtn, SIGNAL(clicked()), SLOT(closeRequested()));

  connect(servicesTable, SIGNAL(cellChanged(int, int)), SLOT(setDataChanged(int, int)));

  servicesTable->setColumnWidth(0, 40);
  servicesTable->setAlternatingRowColors(true);
  servicesTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  servicesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  servicesTable->horizontalHeader()->setHighlightSections(false);
  servicesTable->horizontalHeader()->setStretchLastSection(true);

  setDialogData(_configManager.configuration(_id));
}

//! Destrutor
ServicesDialog::~ServicesDialog()
{
}

//! Salva dados alterados no di�logo
void ServicesDialog::saveRequested()
{
  // Obtem dados do dialogo
  ConfigData newdata = _configManager.configuration(_id);
  getDialogData(newdata);
  
  // Salva
  QString err;
  if(!_configManager.saveConfiguration(_id, newdata, err))
  {
    QMessageBox::warning(this, tr("Erro salvando dados..."), 
                         tr("Erro salvando dados.  Opera��o cancelada.\n%1").arg(err));
    return;
  }
  
  // Desmarca altera��es
  clearDataChanged();
}

//! Sinal chamado quando o usu�rio solicita que as linhas marcadas sejam executadas
void ServicesDialog::execRequested()
{
  // Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."), 
                               tr("Selecione na tabela um ou mais m�dulos a serem executados."));
    return;                               
  }

  // Executa comandos
  QString err;
  int     errCount = 0;
  for(int i=0, size=lines.size(); i<size; i++)
  {
    QString cmdErr;
    ConfigData::CommonData data;
    getLine(lines[i], data);
    if(!runCmd(lines[i], data._cmd, data._params, cmdErr))
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
                                 tr("Nenhum dos m�dulos selecionados foi executado com sucesso.\n"
                                    "Erros:\n\n%1").arg(err));
    }
    else
    {
      QMessageBox::warning(this, tr("Erro executando comandos..."), 
                                 tr("%1 dos %2 m�dulos selecionados foram executados com sucesso.\n"
                                    "Utilize a ferramenta 'Verificar conex�o' para testar\n"
                                    "se estes servi�os foram inicializados corretamente.\n\n"
                                    "Erros na inicializa��o dos demais m�dulos:\n\n%3")
                                    .arg(lines.size()-errCount).arg(lines.size()).arg(err));
    }
  }
  else
  {
    QMessageBox::information(this, tr("Inicializa��o conclu�da"), 
                                   tr("M�dulos executados com sucesso.\n\n"
                                      "Utilize a ferramenta 'Verificar conex�o' para testar\n"
                                      "se todos os servi�os foram inicializados corretamente."));
  }
}

//! Sinal chamado quando o usu�rio solicita que as linhas marcadas sejam "pingadas"
void ServicesDialog::verifyRequested()
{
  // Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."), 
                               tr("Selecione na tabela um ou mais m�dulos a serem verificados."));
    return;                               
  }

  // Passa todas as linhas para o estado de ?
  int size=lines.size();
  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = servicesTable->item(lines[i], 0);
    item->setIcon(QIcon(":/global/icons/ping_unknown.png"));
  }
  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Efetua o ping para cada linha selecionada
  const ConfigData& cfg = _configManager.configuration(_id);
  for(int i=0; i<size; i++)
  {
    bool ok = ServiceManager::ping(cfg, lines[i]);

    QTableWidgetItem* item = servicesTable->item(lines[i], 0);
    item->setIcon(QIcon(ok ? ":/global/icons/ping_ok.png" : ":/global/icons/ping_error.png"));
    QCoreApplication::processEvents();
  }
  
  QApplication::restoreOverrideCursor(); 
}

//! Sinal chamado quando o usu�rio solicita que as linhas marcadas sejam "fechadas"
void ServicesDialog::closeRequested()
{
  // Obtem lista das linhas selecionadas
  QList<int> lines;
  getSelectedLines(lines);
  if(lines.isEmpty())
  {
    QMessageBox::warning(this, tr("Erro..."), 
                               tr("Selecione na tabela um ou mais m�dulos a serem finalizados."));
    return;                               
  }

  // Passa todas as linhas para o estado de ?
  int size=lines.size();
  for(int i=0; i<size; i++)
  {
    QTableWidgetItem* item = servicesTable->item(lines[i], 0);
    item->setIcon(QIcon(":/global/icons/ping_unknown.png"));
  }
  QCoreApplication::processEvents();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Envia solicita��o de finaliza��o para cada linha selecionada
  QString err;
  int errCount = 0;
  
  const ConfigData& cfg = _configManager.configuration(_id);
  for(int i=0; i<size; i++)
  {
    bool ok = ServiceManager::close(cfg, lines[i]);
    if(!ok)
    {
      QString modname = servicesTable->item(lines[i], 1)->text();
      err += tr("Erro enviando solicita��o de finaliza��o para o m�dulo %1\n").arg(modname);
      errCount++;
    }
  }
  QApplication::restoreOverrideCursor(); 
  
  // Mostra mensagens de erro ou sucesso
  if(errCount)
  {
    if(errCount == lines.size())
    {
      QMessageBox::warning(this, tr("Erro enviando solicita��es..."), 
                                 tr("Nenhum dos m�dulos selecionados foi notificado com sucesso.\n"));
    }
    else
    {
      QMessageBox::warning(this, tr("Erro enviando solicita��es..."), 
                                 tr("%1 dos %2 m�dulos selecionados foram notificados com sucesso.\n"
                                    "Utilize a ferramenta 'Verificar conex�o' para testar\n"
                                    "se estes servi�os foram finalizados corretamente.\n\n"
                                    "Erros no envio de solicita��o aos demais m�dulos:\n\n%3")
                                    .arg(lines.size()-errCount).arg(lines.size()).arg(err));
    }
  }
  else
  {
    QMessageBox::information(this, tr("Solicita��o conclu�da"), 
                                   tr("Solicita��es de finaliza��o enviadas com sucesso.\n\n"
                                      "Utilize a ferramenta 'Verificar conex�o' para testar\n"
                                      "se todos os servi�os foram finalizados corretamente."));
  }
}

//! Marca que os dados da tabela foram alterados
void ServicesDialog::setDataChanged(int row, int col)
{
  (void)row;  // Retirar warning
  
  if(_changed || col == 0)
    return;
  _changed = true;
  saveBtn->setEnabled(true);
}

//! Marca que os dados n�o foram alterados
void ServicesDialog::clearDataChanged()
{
  if(!_changed)
    return;
  _changed = false;
  saveBtn->setEnabled(false);
}

//! Preenche uma linha da tabela de dados
void ServicesDialog::setLine(int line, const QString& module, const ConfigData::CommonData& data)
{
  QTableWidgetItem* item;
    
  // Ativo
  item = new QTableWidgetItem(QIcon(":/global/icons/ping_unknown.png"), "");
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  servicesTable->setItem(line, 0, item);
  // Modulo
  item = new QTableWidgetItem(module);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  servicesTable->setItem(line, 1, item);
  // Localiza��o
  item = new QTableWidgetItem(data._address + QString(" : %1").arg(data._servicePort));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  servicesTable->setItem(line, 2, item);
  // Comando
  servicesTable->setItem(line, 3, new QTableWidgetItem(data._cmd));
  // Par�metros
  servicesTable->setItem(line, 4, new QTableWidgetItem(data._params));
}

//! Preenche estrutura com dados dos campos de comando e par�metros da linha da tabela
void ServicesDialog::getLine(int line, ConfigData::CommonData& data)
{
  QTableWidgetItem* item = servicesTable->item(line, 3);
  data._cmd = item->data(Qt::DisplayRole).toString().trimmed();
  
  item = servicesTable->item(line, 4);
  data._params = item->data(Qt::DisplayRole).toString().trimmed();
}

//! Retorna lista contendo indices das linhas selecionada
void ServicesDialog::getSelectedLines(QList<int>& list)
{
  list.clear();
  
  QModelIndexList indexlist = servicesTable->selectionModel()->selectedRows();
  for(int i=0, size=indexlist.size(); i<size; i++)
    list.push_back(indexlist[i].row());
}

//! Preenche tabela com dados de configura��o
void ServicesDialog::setDialogData(const ConfigData& config)
{
  // Preenche nome da configura��o
  configLbl->setText(config.name());

  // Ajusta tamanho da tabela
  servicesTable->setRowCount(4 + config.analysis()._numInstances);
  assert(config.analysis()._numInstances == config.analysis()._instanceData.size() + 1);

  // Preenche primeiro com informa��es dos m�dulos de coleta, planos notifica��o e an�lise
  // OBS: Se esta ordem for alterada, todos as demais fun��es que leem dados da tabela
  // precisam ser alteradas.  Al�m disso, o c�digo em ServiceManager tamb�m precisara
  // ser modificado
  setLine(0, tr("Coleta"),      config.collection());
  setLine(1, tr("Planos"),      config.plans());
  setLine(2, tr("Notifica��o"), config.notification());
  setLine(3, tr("Anima��o"),	config.animation());
  setLine(4, tr("An�lise 1"),   config.analysis());

  // Agora coloca as inst�ncias adicionais
  const QVector<ConfigData::CommonData>& idata = config.analysis()._instanceData;
  for(int i=0, size=idata.size(); i<size; i++)
  {
    setLine(5+i, tr("An�lise %1").arg(i+2), idata[i]);
  }

  // Ajusta espessura das linhas
  servicesTable->resizeRowsToContents();
  
  // Marca dados como n�o alterados
  clearDataChanged();
}

//! Preenche estrutura com dados obtidos do dialogo
void ServicesDialog::getDialogData(ConfigData& config)
{
  assert(servicesTable->rowCount() == 4 + config.analysis()._numInstances);

  // Obtem dados das linhas padronizadas  
  getLine(0, config.collection());  
  getLine(1, config.plans());
  getLine(2, config.notification());
  getLine(3, config.animation());
  getLine(4, config.analysis());

  // Obtem dados das demais inst�ncias de an�lise
  QVector<ConfigData::CommonData>& idata = config.analysis()._instanceData;
  assert(servicesTable->rowCount() == 5 + idata.size());
  for(int i=0, size=idata.size(); i<size; i++)
    getLine(5+i, idata[i]);
}

//! Executa o comando recebido como par�metro
bool ServicesDialog::runCmd(int line, QString cmd, QString param, QString& err)
{
  // Verifica se o comando existe e � execut�vel
  QFileInfo info(cmd);
  QString dir = QCoreApplication::applicationDirPath();
  if(!info.exists())
  {
//#if !defined QT_NO_DEBUG 	
    // Auxilio ao processo de debug:
    // Procura no diret�rio do execut�vel
    info.setFile(dir + "/" + cmd);       
  	if(!info.exists())
  	{
      err = tr("Comando inexistente: %1").arg(cmd);
		  return false;
    }
    cmd = dir + "/" + cmd;
//#else
//    err = tr("Comando inexistente: %1").arg(cmd);
//    return false;
//#endif    
  }  
  if(!info.isExecutable())
  {
    err = tr("Comando n�o � execut�vel: %1").arg(cmd);
    return false;
  }  
  
  /* Efetua substitui��es nos par�metros:
       %c = Path do arquivo de configura��o
       %i = N�mero da inst�ncia para an�lise
       %m = Nome do m�dulo [analise, coleta, planos ou notificacao]
       %a = Nome da m�quina
  */
  {
    static const char* module_names[] = {"coleta", "planos", "notificacao", "animacao", "analise"};
    const ConfigData& conf = _configManager.configuration(_id);
    QString address;
    if     (line == 0) address = conf.collection()._address;
    else if(line == 1) address = conf.plans()._address;
    else if(line == 2) address = conf.notification()._address;
	else if(line == 3) address = conf.animation()._address;
    else if(line == 4) address = conf.analysis()._address;
    else               address = conf.analysis()._instanceData[line-5]._address;
    
    param.replace("%c", QString("\"%1\"").arg(_configManager.configurationFile(_id)), Qt::CaseInsensitive);
    param.replace("%i", (line<4) ? "0" : QString::number(line-3), Qt::CaseInsensitive);
    param.replace("%m", module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
	param.replace("%p", dir + "/" + module_names[line<=4 ? line : 4], Qt::CaseInsensitive);
    param.replace("%a", address, Qt::CaseInsensitive);
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
