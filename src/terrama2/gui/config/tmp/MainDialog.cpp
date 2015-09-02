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
 * \file MainDialog.hpp
 *
 * \brief Definicao dos metodos declarados na classe MainDialog
 *
 * \author Carlos Augusto Teixeira Mendes
 * \date february, 2008 
 * \version $Id
 *
 */

/*!
\class MainDialog

Para facilitar a modularização do código, o controle da interface está 
separado por ficha.  Cada ficha possui uma classe própria (MainDialogXxxxxTab)
que controla seu comportamento.  

Basicamente esta classe gerencia o processo de criação e inicialização 
das fichas que compõe a janela principal, o processo de carga dos dados 
quando a configuração atual é alterada e avisa o usuário da necessidade de 
salvar os dados quando trocando de ficha.

*/

// TerraMA2
#include "MainDialog.hpp"
// #include "MainDialogWeatherTab.hpp"
// #include "MainDialogRiskTab.h"
// #include "MainDialogAditionalTab.h"
// #include "MainDialogCellularSpaceTab.h"
// #include "MainDialogAnalysisTab.h"
// #include "MainDialogUsersTab.h"
// #include "MainDialogBulletinTab.h"
// #include "configDlg.h"
// #include "consoleDlg.h"
// #include "studyDlg.h"
// #include "Services.hpp"
// #include "ArchivingRulesDialog.hpp"
// #include "terraMEPlayerDlg.h"

// STL
#include <assert.h>

// QT
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>


//! Construtor
MainDialog::MainDialog(QWidget* parent, Qt::WindowFlags f)
: QMainWindow(parent, f)
{
  setupUi(this);

//   _changeConfig = false;

//   // Cria gerenciador de serviços
//   _manager = new Services;

//   // Cria gerenciadores para cada ficha
//   // Gerenciadores devem ser incluidos na lista na mesma ordem em 
//   // que os tabs aparecem na janela
  // MainDialogTab *weatherTab		= new MainDialogWeatherTab(this, _manager);
//   MainDialogTab *riskTab			= new MainDialogRiskTab(this, _manager);
//   MainDialogTab *additionalTab		= new MainDialogAditionalTab(this, _manager);
//   MainDialogTab *cellularSpaceTab	= new MainDialogCellularSpaceTab(this, _manager);
//   MainDialogTab *analysisTab		= new MainDialogAnalysisTab(this, _manager);
//   MainDialogTab *usersTab			= new MainDialogUsersTab(this, _manager);
//   MainDialogTab *BulletinTab		= new MainDialogBulletinsTab(this, _manager);
  
  // _tabList.append(weatherTab);
//   _tabList.append(riskTab);
//   _tabList.append(additionalTab);
//   _tabList.append(cellularSpaceTab);
//   _tabList.append(analysisTab);
//   _tabList.append(usersTab);
//   _tabList.append(BulletinTab);
//   mainTabWidget->setCurrentIndex(0);
//   _currentTabIndex = 0;
  
//   // Conecta açoes para troca de tab
//   connect(mainTabWidget, SIGNAL(currentChanged(int)), SLOT(tabChangeRequested(int)));

//   // Conecta Acoes do menu de configurações
//   connect(openAct,       SIGNAL(activated()), SLOT(configOpenRequested()));
//   // connect(saveAct,       SIGNAL(activated()), SLOT(saveRequested()));
//   connect(propertiesAct, SIGNAL(activated()), SLOT(configPropertiesRequested()));
//   connect(refreshAct,    SIGNAL(activated()), SLOT(refreshRequested()));
//   connect(exitAct,       SIGNAL(activated()), SLOT(close()));

//   connect(studyAct,       SIGNAL(activated()), SLOT(executeStudyRequested()));
//   connect(archivingAct,   SIGNAL(activated()), SLOT(archiveRequested()));
//   connect(showConsoleAct, SIGNAL(activated()), SLOT(showConsoleRequested()));
//   connect(terraMEPlayerAct, SIGNAL(activated()), SLOT(executeTerraMEPlayerRequested()));


//   connect(weatherTab, SIGNAL(editServerStarted()),  this, SLOT(disableRefreshAction()));
//   connect(weatherTab, SIGNAL(editGridStarted()),    this, SLOT(disableRefreshAction()));
//   connect(weatherTab, SIGNAL(editPointStarted()),   this, SLOT(disableRefreshAction()));
//   connect(weatherTab, SIGNAL(editPointDiffStarted()),   this, SLOT(disableRefreshAction()));
//   connect(weatherTab, SIGNAL(editServerFinished()), this, SLOT(enableRefreshAction()));
//   connect(weatherTab, SIGNAL(editGridFinished()),   this, SLOT(enableRefreshAction()));
//   connect(weatherTab, SIGNAL(editPointFinished()),  this, SLOT(enableRefreshAction()));
//   connect(weatherTab, SIGNAL(editPointDiffFinished()),  this, SLOT(enableRefreshAction()));
  
//   connect(riskTab, SIGNAL(editRiskMapStarted()),  this, SLOT(disableRefreshAction()));
//   connect(riskTab, SIGNAL(editRiskMapFinished()), this, SLOT(enableRefreshAction()));
  
//   connect(additionalTab, SIGNAL(editAdditionalMapStarted()),  this, SLOT(disableRefreshAction()));
//   connect(additionalTab, SIGNAL(editAdditionalMapFinished()), this, SLOT(enableRefreshAction()));

//   connect(cellularSpaceTab, SIGNAL(editCellularSpaceStarted()),  this, SLOT(disableRefreshAction()));
//   connect(cellularSpaceTab, SIGNAL(editCellularSpaceFinished()), this, SLOT(enableRefreshAction()));
  
//   connect(analysisTab, SIGNAL(editAnalysisRuleStarted()),  this, SLOT(disableRefreshAction()));
//   connect(analysisTab, SIGNAL(editAnalysisRuleFinished()), this, SLOT(enableRefreshAction()));
  
//   connect(usersTab, SIGNAL(editUserStarted()),  this, SLOT(disableRefreshAction()));
//   connect(usersTab, SIGNAL(editUserFinished()), this, SLOT(enableRefreshAction()));

//   connect(BulletinTab, SIGNAL(editBulletinStarted()),  this, SLOT(disableRefreshAction()));
//   connect(BulletinTab, SIGNAL(editBulletinFinished()), this, SLOT(enableRefreshAction()));
  
//   connect(additionalTab, SIGNAL(additionalMapDatabaseChanged()), analysisTab, SLOT(additionalMapChanged()));
//   connect(additionalTab, SIGNAL(editAdditionalMapFinished()), analysisTab, SLOT(additionalMapChanged()));

  
//   // Conecta acoes associadas com projecoes
// //  connect(defaultProjectionAct, SIGNAL(activated()), SLOT(projectionDialogRequested()));

//   // Altera cor das mensagens da barra de status
//   statusBar()->setStyleSheet("color: darkred");

//   // Carrega última configuração
//   loadLastConfig();

//   bool isStudyDB;
//   _manager->getDBType(isStudyDB);
//   studyAct->setEnabled(isStudyDB);
}

//! Destrutor
MainDialog::~MainDialog()
{
  // delete _manager;
}


// void MainDialog::configOpenRequested()
// {
//   // Obtem arquivo a ser aberto  
//   QString s = QFileDialog::getOpenFileName(this, tr("Escolha uma configuração"),
//                                            ".", tr("Configurações (*.xml)"));
//   if(s.isEmpty())
//     return;

//   // Carrega configuração
//   QString err;
//   if(!_manager->loadConfiguration(s, err))
//   {
//     QMessageBox::warning(this, tr("Erro..."), err);
//     return;
//   }  
  
//   // Reconecta com novos serviços  
//   loadData();
//   setConfigFileName(s);
//   enableConfigActions(true);
//   saveLastConfig();
//   _changeConfig = false;

//   bool isStudyDB;
//   _manager->getDBType(isStudyDB);
//   studyAct->setEnabled(isStudyDB);
// }

// //! Slot chamado quando o usuário solicita salvar as configurações
// void MainDialog::saveRequested()
// {
//   if(!_changeConfig)
//     return;

//   QString err;
//   if(!_manager->saveConfiguration(_configFileName, err))
//   {
//     QMessageBox::warning(this, tr("Erro..."), err);
//     return;
//   }
//   _changeConfig = false;
// }

// //! Slot chamado quando o usuário solicita alterar propriedades de configuração
// void MainDialog::configPropertiesRequested()
// {
//   ConfigDlg dlg(_manager);

//   if (dlg.exec()==QDialog::Accepted)		// else QDialog::Rejected
//   {
// 	  _changeConfig = dlg.changed();
// 	  if(_changeConfig)
//       loadData();
//   }
// }

// //! Recarrega dados do banco
// void MainDialog::loadData(bool initOnly)
// {
//   this->statusBar()->showMessage(tr("Carregando dados."));

//   _manager->connect(initOnly);

//   foreach(MainDialogTab* tab, _tabList)
//     tab->load();

//   this->statusBar()->clearMessage();
// }

// // Executa estudos
// void MainDialog::executeStudyRequested()
// {
// 	wsTimestampInterval interval;
// 	_manager->getValidStudyInterval(interval);

// 	if(!interval.valid)
// 	{
// 		QMessageBox::warning(this, tr("Erro..."), tr("Não há dados coletados suficientes para estudo."));
// 		return;
// 	}
	
// 	StudyDlg dlg(_manager, interval);
// 	dlg.exec();
// }

// //! Slot chamado quando o usuário pressiona o botão de arquivamento de dados
// void MainDialog::archiveRequested()
// {	
// 	ArchivingRulesDialog dlg(_configFileName, _manager, NULL, Qt::Dialog | Qt::WindowTitleHint);

// 	std::vector<struct wsArchivingRule> archivingRules;
// 	_manager->getArchivingRules(archivingRules);

// 	dlg.setFields(archivingRules);

// 	if (dlg.exec() == QDialog::Accepted)	
// 	{
// 		std::vector<struct wsArchivingRule> rules = dlg.getFields();
// 		_manager->saveArchivingRules(rules);
// 	}
// }

// //! Slot chamado quando o usuario pressiona o botao para visualizar os resultados de analises TerraME
// void MainDialog::executeTerraMEPlayerRequested()
// {
// 	TerraMEPlayerDlg dlg(_manager, this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
// 	dlg.exec();
// }

// //! Mostra console
// void MainDialog::showConsoleRequested()
// {
//   ConsoleDlg dlg(*_manager->getConfigData());
//   dlg.exec();
// }

// //! Slot chamado quando o usuário altera o tab corrente
// void MainDialog::tabChangeRequested(int index)
// {
//   if(index == _currentTabIndex)
//     return;
  
//   // Verifica se o tab pode ser trocado
//   MainDialogTab* tab = _tabList.at(_currentTabIndex);
  
//   if(tab->verifyAndEnableChange(true))
//     _currentTabIndex = index;  
//   else
//     mainTabWidget->setCurrentIndex(_currentTabIndex);
// }

// //! Carrega a última configuração especificada pelo usuário
// void MainDialog::loadLastConfig()
// {
//   QSettings s(QSettings::UserScope, "INPE-DPI", "SISMADEN");
//   QString lastFile = s.value("lastConfiguration").toString();

//   if(lastFile.isEmpty())
//     lastFile = "serviceconfiguration.xml";
//   else
//   {  
//     QFileInfo info(lastFile);
//     if(!info.exists())
//       lastFile = "serviceconfiguration.xml";
//   }    

//   setConfigFileName(lastFile);

//   // Carrega o arquivo
//   QString err;
//   bool ok = _manager->loadConfiguration(lastFile, err);
//   if(!ok)
//     QMessageBox::warning(this, tr("Erro..."), err);

//   loadData(!ok);
//   enableConfigActions(ok);
// }

// void MainDialog::saveLastConfig()
// {
//   QSettings s(QSettings::UserScope, "INPE-DPI", "SISMADEN");
//   s.setValue("lastConfiguration", _configFileName);
// }

// void MainDialog::setConfigFileName(QString configFileName)
// {
//   _configFileName = configFileName;

//   if (_configFileName.isEmpty())
//     setWindowTitle(tr("TerraMA² - Módulo de Configuração [SemNome])"));
//   else
//     setWindowTitle(tr("TerraMA² - Módulo de Configuração [")+abrevFileName(_configFileName, 30)+"]");
// }

// QString MainDialog::abrevFileName(QString fileName, int size)
// {
//   QString ret;

//   if (fileName.size()>size)
//   {
//     QFileInfo fi(fileName);

//     if (fi.fileName().size() > size)
//       ret = ".../"+fi.fileName().left(size)+"...";
//     else
//       ret = fi.path().left(size - fi.fileName().size())+".../"+fi.fileName();
//   }
//   else
// 	  ret = fileName;

//   return ret;
// }

// //! Slot chamado quando o usuário solicita recarregar os dados
// void MainDialog::refreshRequested()
// {
//   loadData();

//   bool isStudyDB;
//   _manager->getDBType(isStudyDB);
//   studyAct->setEnabled(isStudyDB);
// }


// void MainDialog::closeEvent(QCloseEvent* close)
// {
//   MainDialogTab* tab = _tabList.at(_currentTabIndex);
  
//   if(tab->verifyAndEnableChange(false))
//     close->accept();
//   else
//     close->ignore();  
// }


// void MainDialog::enableRefreshAction()
// {
// 	refreshAct->setEnabled(true);
// }


// void MainDialog::disableRefreshAction()
// {
// 	refreshAct->setEnabled(false);
// }

// void MainDialog::enableConfigActions(bool mode)
// {
// //  saveAct->setEnabled(mode);
//   propertiesAct->setEnabled(mode);
//   refreshAct->setEnabled(mode);
// }

