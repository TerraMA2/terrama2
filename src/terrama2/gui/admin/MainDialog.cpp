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
  \file terrama2/gui/admin/MainDialog.cpp

  \brief Admin main dialog implementation.

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class MainDialog

*/

// TerraMA2
#include "MainDialog.hpp"
// #include "PreviewCustom.hpp"
// #include "DbManager.hpp"
// #include "consoleDlg.h"
// #include "ServicesDialog.hpp"


// Qt
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QHostInfo>

// 

// #include "soapModNotificacaoProxy.h"

// STL
#include <cassert>
#include <memory>
#include <QIcon>
#include <QString>

//! Construtor
MainDialog::MainDialog(QWidget* parent, Qt::WindowFlags f)
: QMainWindow(parent, f)
{
  setupUi(this);

  // // Conecta ações do menu de configurações
  // connect(newAct,    SIGNAL(triggered()), SLOT(newRequested()));
  // connect(openAct,   SIGNAL(triggered()), SLOT(openRequested()));
  // connect(saveAct,   SIGNAL(triggered()), SLOT(saveRequested()));
  // connect(saveAsAct, SIGNAL(triggered()), SLOT(saveAsRequested()));
  // connect(renameAct, SIGNAL(triggered()), SLOT(renameRequested()));
  // connect(removeAct, SIGNAL(triggered()), SLOT(removeRequested()));
  // connect(exitAct,   SIGNAL(triggered()), SLOT(close()));

  // Conecta ações do menu de serviços
  // connect(consoleAct,  SIGNAL(triggered()), SLOT(showConsoles()));
  // connect(servicesAct, SIGNAL(triggered()), SLOT(manageServices()));
  
  // // Conecta ações relacionadas ao comportamento da interface
  // connect(saveBtn,   SIGNAL(clicked()), SLOT(saveRequested()));
  // connect(cancelBtn, SIGNAL(clicked()), SLOT(cancelRequested()));
  
  // connect(dbCheckConnectionBtn, SIGNAL(clicked()), SLOT(dbCheckConnectionRequested()));
  // connect(dbCreateDatabaseBtn,  SIGNAL(clicked()), SLOT(dbCreateDatabaseRequested()));
  
  // connect(loadBalRefreshBtn,      SIGNAL(clicked()),         SLOT(loadBalanceReloadData()));
  // connect(anaInstancesRefreshBtn, SIGNAL(clicked()),         SLOT(reloadInstancesTableData()));
  // connect(anaInstancesSpb,        SIGNAL(valueChanged(int)), SLOT(numInstancesChanged(int)));

  // //Conecta a ação de enviar um email de teste
  // connect(testSendMailBtn, SIGNAL(clicked()), SLOT(testSendMailRequested()));

  // //conecta a acao de escolher o diretorio dos pdf
  // connect(selectPDFDirectoryBtn, SIGNAL(clicked()), SLOT(selectPDFPath()));

  // //Conecta a geração do pacote de visualizacao
  // connect(generatePackageBtn, SIGNAL(clicked()), SLOT(generatePackage()));
  // connect(selectLogoBtn, SIGNAL(clicked()), SLOT(selectLogo()));
  // connect(selectDirectoryBtn, SIGNAL(clicked()), SLOT(selectOutputPath()));


  // Para monitorar a mudança de config. na lista de configurações disponíveis, estamos
  // usando um sinal do modelo de seleção e não o tradicional currentRowChanged()
  // Isso é feito pois em currentRowChanged() não conseguimos voltar para a seleção
  // anterior caso o usuário deseje cancelar a troca.
  // connect(configListWidget->selectionModel(), 
  //         SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
  //         SLOT(configListSelectionChanged(const QItemSelection&, const QItemSelection&)));

  // // Prepara menu de contexto para a lista de configurações
  // configListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
  // configListWidget->insertAction(NULL, renameAct);
  // configListWidget->insertAction(NULL, saveAct);
  // configListWidget->insertAction(NULL, saveAsAct);
  // configListWidget->insertAction(NULL, removeAct);

  // QAction* sep = new QAction(this);
  // sep->setSeparator(true);
  // configListWidget->insertAction(saveAct, sep);
  // sep = new QAction(this);
  // sep->setSeparator(true);
  // configListWidget->insertAction(removeAct, sep);
  
  // Seta validadores para campos numéricos
  // setupValidators();

  // Connecta ações para interceptar mudanças de campos
  // setupDataChanged();

  // Inicializa campos
  // _ignoreChangeEvents = false;
  // _currentConfigIndex = -1;
  // _name               = "";
  // _newData            = false;
  // clearDataChanged();

  // loadBalTable->setColumnWidth(0, 40);
  // loadBalTable->setSelectionMode(QAbstractItemView::SingleSelection);
  // anaInstancesTable->setColumnWidth(0, 60);
  // anaInstancesTable->setSelectionMode(QAbstractItemView::SingleSelection);
  
  // Carrega lista de configuraçoes disponiveis
  // load();
}

//! Destrutor
MainDialog::~MainDialog()
{
}

//! Prepara a interceptação de sinal emitido quando dados são alterados por cada elem. de interface
// void MainDialog::setupDataChanged()
// {
  // // Campos da base de dados
  // connect(dbTypeCmb,     SIGNAL(currentIndexChanged(int)),   SLOT(setDataChanged()));
  // connect(dbAddressLed,  SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(dbPortLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(dbUserLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(dbPasswordLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(dbDatabaseLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(dbStudyChk,    SIGNAL(clicked(bool)),              SLOT(studyChkClicked(bool)));

  // // Campos do módulo de análise
  // connect(anaInstancesSpb,   SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(anaAddressLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(anaPortLed,        SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(anaLogFileLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(anaIntervalMinSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(anaIntervalSecSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(anaInstancesTable, SIGNAL(cellChanged(int, int)),      SLOT(setDataChanged()));
  // connect(anaDirNameLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos do módulo de coleta
  // connect(aqAddressLed,    SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(aqPortLed,       SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(aqLogFileLed,    SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(aqTimeoutMinSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(aqTimeoutSecSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(aqDirNameLed,    SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos do módulo de planos
  // connect(layersAddressLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(layersPortLed,    SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(layersLogFileLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos do modulo de animacao
  // connect(animAddressLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(animLogFileLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(animPortLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(animImageDirLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos do módulo de visualização
  // connect(webTitleLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webLogoLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webLongitudeLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webLatitudeLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webZoomLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webGoogleLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webTerraOGCLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webOpacityLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webIntervalLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(webOutputLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos do módulo de notificação
  // connect(notifAddressLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifPortLed,        SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifLogFileLed,     SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifIntervalMinSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(notifIntervalSecSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(notifIntervalBulletinHouSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(notifIntervalBulletinMinSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(notifIntervalBulletinSecSpb, SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(notifSmtpServerLed,  SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifSmtpPortLed,    SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifSmtpMailLed,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifSmtpPsswrdLed,  SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifSmtpAliasLed,   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifSmtpTimeOutSpb, SIGNAL(valueChanged(int)),		   SLOT(setDataChanged()));
  // connect(notifSmtpPriorityLed,SIGNAL(currentIndexChanged(int)),   SLOT(setDataChanged()));
  // connect(notifSmtpSSLChk,	   SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));
  // connect(notifProxyUserLed,   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(notifProxyPsswrdLed, SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(sendUpEmailChk,	   SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));
  // connect(removeZeroChk,	   SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));
  // connect(bulletinPDFChk,	   SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));
  // connect(pdfPathLed,		   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(bulletinPDFChk,	   SIGNAL(clicked(bool)), pdfPathLed, SLOT(setEnabled(bool)));
  // connect(bulletinPDFChk,	   SIGNAL(clicked(bool)), selectPDFDirectoryBtn, SLOT(setEnabled(bool)));
  // connect(readConfirmationChk, SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));
  // connect(imagePDFChk,		   SIGNAL(stateChanged(int)),          SLOT(setDataChanged()));

  // connect(legendDirNameLed,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(xlegendSpb,	       SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));
  // connect(ylegendSpb,	       SIGNAL(valueChanged(int)),          SLOT(setDataChanged()));

  // connect(level0Led,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(level1Led,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(level2Led,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(level3Led,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));
  // connect(level4Led,	   SIGNAL(textEdited(const QString&)), SLOT(setDataChanged()));

  // // Campos da aba de balanceamento de carga
  // connect(loadBalTable, SIGNAL(cellChanged(int, int)), SLOT(setDataChanged()));
// }

//! Seta validadores nos campos de texto que necessitam
// void MainDialog::setupValidators()
// {
  // dbPortLed->setValidator       (new QIntValidator(0, 65535, dbPortLed));
  // anaPortLed->setValidator      (new QIntValidator(0, 65535, anaPortLed));
  // aqPortLed->setValidator       (new QIntValidator(0, 65535, aqPortLed));
  // layersPortLed->setValidator   (new QIntValidator(0, 65535, layersPortLed));
  // animPortLed->setValidator     (new QIntValidator(0, 65535, animPortLed));
  // notifPortLed->setValidator    (new QIntValidator(0, 65535, notifPortLed));
  // notifSmtpPortLed->setValidator(new QIntValidator(0, 65535, notifSmtpPortLed));
// }

// //! Carrega configurações existentes e preenche lista
// void MainDialog::load()
// {
//   // Limpa lista no dialogo
//   configListWidget->clear();
//   clearDialogData();
  
//   // Carrega lista de configurações
//   QString dir = QDir::currentPath();
//   _configManager.loadAvailableConfigurations(dir);
  
// #if !defined QT_NO_DEBUG 
//   if(!_configManager.numConfigurations())
//   {
//     // Auxilio ao processo de debug:
//     // Procura no diretório do executável (ou no anterior se o path incluir debug no final)
//     dir = QCoreApplication::applicationDirPath();
//     if(dir.endsWith("/debug")) // Estamos rodando de dentro do debugger...
//       dir.chop(6); // remove diretório 
//     _configManager.loadAvailableConfigurations(dir);
//   }    
// #endif
  
//   // Preenche a lista com as configurações disponiveis
//   int listSize = _configManager.numConfigurations();
//   if(listSize)
//   {
//     for(int i=0; i<listSize; i++)
//       configListWidget->addItem(_configManager.configuration(i).name());
  
//     // Seleciona a primeira configuração (o que ira preencher campos da tela)
//     enableFields(true);
//     configListWidget->setCurrentRow(0);  
//   }  
//   else
//     enableFields(false);
// }

// /*! \brief Salva configuração atual.  Se newfilename for vazio, usa o nome corrente para
//            salvar a configuração (que neste caso não pode ser nova).  Caso contrário salva
//            a configuração com este novo nome.
// */           
// bool MainDialog::save(QString newfilename, QString& err)
// {
//   // Obtem dados da interface
//   ConfigData newvalues;
//   if(_newData)
//     newvalues.loadDefaults(anaInstancesSpb->value() + 1);
//   else
//   {
//     newvalues    = _configManager.configuration(_currentConfigIndex);
//     int ninst    = newvalues.analysis()._instanceData.size();
//     int newninst = anaInstancesSpb->value();
//     if(newninst > ninst) // Inicializa comandos e parametros de novas instâncias
//     {
//       QString defcmd, defparams;
//       ConfigData::getDefaultAnalysisCommands(defcmd, defparams);
//       newvalues.analysis()._instanceData.resize(newninst);
//       for(int i=ninst; i<newninst; i++)
//       {
//         newvalues.analysis()._instanceData[i]._cmd    = defcmd;
//         newvalues.analysis()._instanceData[i]._params = defparams;
//       }
//     }
//   }  
//   getDialogData(newvalues);

//   // Salva dados de instâncias alteradas no banco
//   QList<int> instances;
//   getLoadBalanceData(instances);
//   assert(instances.size() == _loadBalData.size());

//   QList<DbManager::AnaData> changedData;
//   for(int i=0, size=instances.size(); i<size; i++)
//   {
//     if(instances[i] != _loadBalData[i]._instance)
//     {
//       DbManager::AnaData a = _loadBalData[i];
//       a._instance = instances[i];
//       changedData.push_back(a);
//     }
//   }

//   if(changedData.size())
//   {
//     if(!DbManager::saveAnaInstanceData(newvalues.db(), changedData))
//       return false;

//     // Atualiza dados de instâncias em memória
//     for(int i=0, size=instances.size(); i<size; i++)
//       _loadBalData[i]._instance = instances[i];
//   }

//   // Salva dados em arquivo
//   bool ok;
//   if(newfilename.isEmpty())  
//   {
//     assert(!_newData);
//     ok = _configManager.saveConfiguration(_currentConfigIndex, newvalues, err);
//   }
//   else
//   {
//     ok = _configManager.saveNewConfiguration(newfilename, newvalues, err);
//   }
//   if(!ok)
//     return false;

//   // Atualiza estado para dados não alterados
//   clearDataChanged();
//   _newData = false;
//   dbStudyChk->setEnabled(false);
//   return true;
// }

// //! Valida dados da conexão com a abse contidos na interface.  Retorna true se ok para salvar
// bool MainDialog::validateDbData(QString& err)
// {
//   err = "";
//   if(dbAddressLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Endereço' da ficha 'Base de dados'.");
//   else if(dbDatabaseLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Base de dados' da ficha 'Base de dados'.");
//   else if(!dbPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta' da ficha 'Base de dados' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
  
//   return (err == "");
// }

// //! Valida dados de instâncias adicionais do módulo de análise.  Retorna true se ok para salvar
// bool MainDialog::validateInstanceTableData(QString& err)
// {
//   for(int i=0, size=anaInstancesTable->rowCount(); i<size; i++)
//   {
//     QTableWidgetItem* item = anaInstancesTable->item(i, 2);
//     QString port_str = item->data(Qt::DisplayRole).toString().trimmed();
//     int     port     = item->data(Qt::DisplayRole).toInt();
//     if(port_str.isEmpty() || port < 0 || port > 65535)
//     {
//       err = tr("O campo 'Porta do serviço' da instância de análise %1 deve ser preenchido\n"
//                "com um número inteiro entre 0 e 65535.").arg(i+2);
//       return false;
//     }
//     item = anaInstancesTable->item(i, 3); 
//     if(item->data(Qt::DisplayRole).toString().trimmed().isEmpty())
//     {
//       err = tr("Por favor preencha o campo 'Arquivo de Log' da instância de análise %1.").arg(i+2);
//       return false;
//     }
//   }
//   return true;
// }

// //! Valida dados contidos na interface.  Retorna true se ok para salvar.
// bool MainDialog::validate(QString& err)
// {
//   err = "";
  
//   if(!validateDbData(err))
//     return false;

//   if(!validateInstanceTableData(err))
//     return false;

//   //Criar pasta para armazenar as imagens geradas pelas analises se esta ainda nao existir
//   QString imagePath = anaDirNameLed->text().trimmed();

//   imagePath = imagePath.replace(QChar('\\'), QChar('/'));
//   if(!imagePath.endsWith('/'))
//   {
// 	  imagePath.append('/');
//   }
//   anaDirNameLed->setText(imagePath);

//   if(imagePath.isEmpty())
//   {
// 	  err = tr("Por favor preencha o campo 'Diretório para salvar imagens' da ficha 'Análise'.");
// 	  return false;
//   }

//   QDir dir(imagePath);
//   if(!dir.exists())
//   {
// 	  if(!dir.mkdir(imagePath))
// 	  {
// 		  err = tr("Não foi possível criar o diretório: ") + imagePath;
// 		  return false;
// 	  }
//   }

//   //Diretorio para salvar dados coletados
//   QString collectionDirPath = aqDirNameLed->text().trimmed();
//   collectionDirPath = collectionDirPath.replace(QChar('\\'), QChar('/'));
//   aqDirNameLed->setText(collectionDirPath);

//   //Este diretorio eh opcional. Se o usuario informou, entao tentar cria-lo
//   if(!collectionDirPath.isEmpty())
//   {
// 	  QDir dir(collectionDirPath);
// 	  if(!dir.exists() && !dir.mkdir(collectionDirPath))
// 	  {
// 		  err = tr("Não foi possível criar o diretório: ") + collectionDirPath;
// 		  return false;
// 	  }
//   }

//   // Arquivos de log
//   if(anaLogFileLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Arquivo de Log' da ficha 'Análise'.");
//   else if(aqLogFileLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Arquivo de Log' da ficha 'Coleta'.");
//   else if(layersLogFileLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Arquivo de Log' da ficha 'Planos'.");
//   else if(notifLogFileLed->text().trimmed().isEmpty())
//     err = tr("Por favor preencha o campo 'Arquivo de Log' da ficha 'Notificação'.");
//   // Portas
//   else if(!anaPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta do serviço' da ficha 'Análise' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
//   else if(!aqPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta do serviço' da ficha 'Coleta' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
//   else if(!layersPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta do serviço' da ficha 'Planos' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
//   else if(!animPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta do serviço' da ficha 'Animação' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
//   else if(!notifPortLed->hasAcceptableInput())
//     err = tr("O campo 'Porta do serviço' da ficha 'Notificação' deve ser preenchido\n"
//              "com um número inteiro entre 0 e 65535.");
//   else
//   {
//     // Verifica se existe na tabela de bal. de carga alguma instância maior 
//     // que o número de instâncias.
//     // Se existir, pergunta ao usuário se realmente deseja prosseguir
//     QList<int> instances;
//     getLoadBalanceData(instances);
//     assert(instances.size() == _loadBalData.size());

//     int maxInstance = anaInstancesSpb->value() + 1;
//     foreach(int inst, instances)
//     {
//       if(inst == 0 || inst > maxInstance)
//       {
//         QMessageBox::StandardButton answer;
//         answer = QMessageBox::question(this, tr("Aviso:"),
//                                        tr("Existem na tabela análises associadas a instâncias\n"
//                                           "inválidas.  Estas análises nunca serão executadas.\n\n"
//                                           "Deseja manter esta configuração?"),
//                                           QMessageBox::Yes | QMessageBox::No,
//                                           QMessageBox::Yes);

//         // Se a operação foi cancelada, basta retornar false
//         if(answer == QMessageBox::No)
//           return false;
//         break; // Não queremos repetir a pergunta, mesmo que haja vários valores acima do num. config. de instâncias
//       }
//     }
//   }

//   return (err == "");
// }

// //! Preenche a interface com dados de uma configuração
// void MainDialog::setDialogData(const ConfigData& data)
// {
//   _name = data.name();
  
//   _ignoreChangeEvents = true;
  
//   // Campos da base de dados
//   dbTypeCmb->setCurrentIndex((int)data.db()._driver);  // Ordem dos itens na combo = ordem enumeração
//   dbAddressLed->setText(data.db()._hostName);
//   dbPortLed->setText(QString::number(data.db()._port));
//   dbUserLed->setText(data.db()._user);
//   dbPasswordLed->setText(data.db()._password);
//   dbDatabaseLed->setText(data.db()._dbName);
//   bool isStudyDB = data.db()._study;
//   dbStudyChk->setChecked(isStudyDB);
//   dbStudyChk->setEnabled(false); //Nao é possivel editar a opcao que define se um banco é destinado a estudos 
  
//   // Campos do módulo de análise
//   anaInstancesSpb->setValue(data.analysis()._numInstances - 1);
//   anaAddressLed->setText(data.analysis()._address);   
//   anaPortLed->setText(QString::number(data.analysis()._servicePort));
//   anaLogFileLed->setText(data.analysis()._logFile);
//   anaIntervalMinSpb->setValue(data.analysis()._interval / 60);
//   anaIntervalSecSpb->setValue(data.analysis()._interval % 60);;
//   setInstanceTableData(data.analysis());
//   anaDirNameLed->setText(data.analysis()._imageFilePath);

//   //Habilita/desabilita campos configuraveis somente em caso de estudo
//   anaInstancesSpb->setDisabled(isStudyDB);
//   anaIntervalMinSpb->setDisabled(isStudyDB);
//   anaIntervalSecSpb->setDisabled(isStudyDB);
//   anaInstancesTable->setDisabled(isStudyDB);
//   anaInstancesRefreshBtn->setDisabled(isStudyDB);

//   // Campos do módulo de coleta
//   aqAddressLed->setText(data.collection()._address); 
//   aqPortLed->setText(QString::number(data.collection()._servicePort));
//   aqLogFileLed->setText(data.collection()._logFile);
//   aqTimeoutMinSpb->setValue(data.collection()._timeout / 60);
//   aqTimeoutSecSpb->setValue(data.collection()._timeout % 60);
//   aqDirNameLed->setText(data.collection()._dirPath);

//   // Campos do módulo de planos
//   layersAddressLed->setText(data.plans()._address);     
//   layersPortLed->setText(QString::number(data.plans()._servicePort));
//   layersLogFileLed->setText(data.plans()._logFile);

//   // Campos do módulo de animacao
//   animAddressLed->setText(data.animation()._address);
//   animPortLed->setText(QString::number(data.animation()._servicePort));
//   animLogFileLed->setText(data.animation()._logFile);
//   animImageDirLed->setText(data.animation()._imageDirectory);

//   // Campos do módulo de visualização
//   webTitleLed->setText(data.preview()._title);
//   webLogoLed->setText(data.preview()._logo);
//   webLongitudeLed->setText(data.preview()._longitude);
//   webLatitudeLed->setText(data.preview()._latitude);
//   webZoomLed->setText(data.preview()._zoomLevel);
//   webGoogleLed->setText(data.preview()._googleKey);
//   webTerraOGCLed->setText(data.preview()._terraOGC);
//   webOpacityLed->setText(data.preview()._opacidade);
//   webIntervalLed->setText(data.preview()._intervalo);
//   webOutputLed->setText(data.preview()._outputPath);
  

//   // Campos do módulo de notificação
//   notifAddressLed->setText(data.notification()._address);
//   notifPortLed->setText(QString::number(data.notification()._servicePort));
//   notifLogFileLed->setText(data.notification()._logFile);
//   notifIntervalMinSpb->setValue(data.notification()._interval / 60);
//   notifIntervalSecSpb->setValue(data.notification()._interval % 60);
//   int minsB = data.notification()._intervalBulletin / 60;
//   notifIntervalBulletinHouSpb->setValue(minsB / 60);
//   notifIntervalBulletinMinSpb->setValue(minsB % 60);
//   notifIntervalBulletinSecSpb->setValue(data.notification()._intervalBulletin % 60);
//   removeZeroChk->setChecked(data.notification()._removeLevelZero);
//   bulletinPDFChk->setChecked(data.notification()._bulletinPDF);
//   pdfPathLed->setEnabled(data.notification()._bulletinPDF);
//   selectPDFDirectoryBtn->setEnabled(data.notification()._bulletinPDF);
//   pdfPathLed->setText(data.notification()._bulletinPath);
//   notifSmtpServerLed->setText(data.notification()._smtpServer);
//   notifSmtpPortLed->setText(QString::number(data.notification()._smtpPort));
//   notifSmtpMailLed->setText(data.notification()._smtpMail);
//   notifSmtpPsswrdLed->setText(data.notification()._smtpPsswrd);
//   notifSmtpAliasLed->setText(data.notification()._smtpAlias);
//   notifSmtpTimeOutSpb->setValue(data.notification()._smtpTimeout);
//   readConfirmationChk->setChecked(data.notification()._readConfirmation);
//   notifSmtpPriorityLed->setCurrentIndex(data.notification()._smtpPriority == "low"?		0:
// 									    data.notification()._smtpPriority == "high"?	2: 
// 																						1);
//   notifSmtpSSLChk->setChecked(data.notification()._smtpSSL);
//   notifProxyUserLed->setText(data.notification()._proxyUser);
//   notifProxyPsswrdLed->setText(data.notification()._proxyPsswrd);
//   sendUpEmailChk->setChecked(data.notification()._sendUpEmail);
//   imagePDFChk->setChecked(data.notification()._imagePDF);

//   //legenda
//   legendDirNameLed->setText(data.notification()._legendPath);
//   xlegendSpb->setValue(data.notification()._xPositionLegend);
//   ylegendSpb->setValue(data.notification()._yPositionLegend);

//   //texto alertas
//   level0Led->setText(data.notification()._levelTexts[0]);
//   level1Led->setText(data.notification()._levelTexts[1]);
//   level2Led->setText(data.notification()._levelTexts[2]);
//   level3Led->setText(data.notification()._levelTexts[3]);
//   level4Led->setText(data.notification()._levelTexts[4]);

//   // Campos da aba de balanceamento de carga
//   loadBalTable->clearContents();
//   loadBalTable->setRowCount(0);
//   loadBalTable->setDisabled(isStudyDB);
//   loadBalRefreshBtn->setDisabled(isStudyDB);
//   _loadBalData.clear();

  
//   _ignoreChangeEvents = false;
  
//   // Dados na tela estão atualizados...
//   clearDataChanged();
// }

// //! Preenche tabela de parâmetros da análise
// void MainDialog::setInstanceTableData(const ConfigData::Analysis& data)
// {
//   // Preenche tabela
//   anaInstancesTable->setRowCount(data._instanceData.size());
//   for(int i=0, size=data._instanceData.size(); i<size; i++)
//   {
//     const ConfigData::CommonData& par = data._instanceData[i];
//     QTableWidgetItem* item;
    
//     item = new QTableWidgetItem(QString::number(i+2));
//     item->setFlags(item->flags() & ~Qt::ItemIsEditable);
//     anaInstancesTable->setItem(i, 0, item);
    
//     anaInstancesTable->setItem(i, 1, new QTableWidgetItem(par._address));
    
//     item = new QTableWidgetItem();
//     item->setData(Qt::DisplayRole, (unsigned int)par._servicePort); // cast para unsigned garante que delegate vai permitir a entrada apenas de números > 0
//     anaInstancesTable->setItem(i, 2, item);

//     anaInstancesTable->setItem(i, 3, new QTableWidgetItem(par._logFile));
//   }
//   anaInstancesTable->resizeRowsToContents();
//   anaInstancesTable->setAlternatingRowColors(true);
// }

//! Preenche a estrutura com dados contidos na interface.  Assume que estes já foram validados
// void MainDialog::getDialogData(ConfigData& data)
// {
//   data.setName(_name);
  
//   // Campos da base de dados
//   data.db()._driver   = (ConfigData::DatabaseDriver)dbTypeCmb->currentIndex(); // Ordem dos itens na combo = ordem enumeração
//   data.db()._hostName = dbAddressLed->text().trimmed();
//   data.db()._port     = dbPortLed->text().toInt();
//   data.db()._user     = dbUserLed->text().trimmed();
//   data.db()._password = dbPasswordLed->text().trimmed();
//   data.db()._dbName   = dbDatabaseLed->text().trimmed();
//   data.db()._study    = dbStudyChk->isChecked();

//   // Campos do módulo de análise
//   data.analysis()._numInstances = anaInstancesSpb->value() + 1;
//   data.analysis()._address      = anaAddressLed->text().trimmed();   
//   data.analysis()._servicePort  = anaPortLed->text().toInt();
//   data.analysis()._logFile      = anaLogFileLed->text().trimmed();
//   data.analysis()._interval     = anaIntervalMinSpb->value() * 60 + anaIntervalSecSpb->value();
//   data.analysis()._imageFilePath = anaDirNameLed->text().trimmed();
  
//   int ninst = anaInstancesSpb->value();
//   data.analysis()._instanceData.resize(ninst);
//   for(int i=0; i<ninst; i++)
//   {
//     QTableWidgetItem* item = anaInstancesTable->item(i, 1);
//     data.analysis()._instanceData[i]._address = item->data(Qt::DisplayRole).toString().trimmed();   
//     item = anaInstancesTable->item(i, 2); 
//     data.analysis()._instanceData[i]._servicePort = item->data(Qt::DisplayRole).toInt();
//     item = anaInstancesTable->item(i, 3); 
//     data.analysis()._instanceData[i]._logFile = item->data(Qt::DisplayRole).toString().trimmed();      
//   }

//   // Campos do módulo de coleta
//   data.collection()._address     = aqAddressLed->text().trimmed();   
//   data.collection()._servicePort = aqPortLed->text().toInt();
//   data.collection()._logFile     = aqLogFileLed->text().trimmed();
//   data.collection()._timeout     = aqTimeoutMinSpb->value() * 60 + aqTimeoutSecSpb->value();
//   data.collection()._dirPath	 = aqDirNameLed->text().trimmed();
  
//   // Campos do módulo de planos
//   data.plans()._address     = layersAddressLed->text().trimmed();   
//   data.plans()._servicePort = layersPortLed->text().toInt();
//   data.plans()._logFile     = layersLogFileLed->text().trimmed();

//   // Campos do módulo de animacao
//   data.animation()._address = animAddressLed->text().trimmed();
//   data.animation()._servicePort = animPortLed->text().toInt();
//   data.animation()._logFile = animLogFileLed->text().trimmed();
//   data.animation()._imageDirectory = animImageDirLed->text().trimmed();
  
//   // Campos do módulo de notificação
//   data.notification()._address      = notifAddressLed->text().trimmed();   
//   data.notification()._servicePort  = notifPortLed->text().toInt();
//   data.notification()._logFile      = notifLogFileLed->text().trimmed();
//   data.notification()._interval     = notifIntervalMinSpb->value() * 60 + notifIntervalSecSpb->value();
//   data.notification()._smtpServer   = notifSmtpServerLed->text().trimmed();
//   data.notification()._smtpPort     = notifSmtpPortLed->text().toInt();
//   data.notification()._smtpMail	    = notifSmtpMailLed->text().trimmed();
//   data.notification()._smtpPsswrd   = notifSmtpPsswrdLed->text().trimmed();
//   data.notification()._smtpAlias    = notifSmtpAliasLed->text().trimmed();
//   data.notification()._smtpTimeout  = notifSmtpTimeOutSpb->value();
//   data.notification()._readConfirmation = readConfirmationChk->isChecked();
//   data.notification()._smtpPriority = notifSmtpPriorityLed->currentIndex() == 0? "low":
// 									  notifSmtpPriorityLed->currentIndex() == 2? "high":
// 																				 "normal";

//   data.notification()._smtpSSL		= notifSmtpSSLChk->isChecked();
//   data.notification()._proxyUser	= notifProxyUserLed->text().trimmed();
//   data.notification()._proxyPsswrd  = notifProxyPsswrdLed->text().trimmed();

//   data.notification()._sendUpEmail	= sendUpEmailChk->isChecked();
//   data.notification()._intervalBulletin   = notifIntervalBulletinHouSpb->value() * 3600 + notifIntervalBulletinMinSpb->value() * 60 + notifIntervalBulletinSecSpb->value();
//   data.notification()._imagePDF	= imagePDFChk->isChecked();

//   data.notification()._removeLevelZero = removeZeroChk->isChecked();
//   data.notification()._bulletinPDF = bulletinPDFChk->isChecked();
//   data.notification()._bulletinPath = pdfPathLed->text().trimmed();
  
//   data.notification()._legendPath = legendDirNameLed->text().trimmed();
//   data.notification()._xPositionLegend = xlegendSpb->value();
//   data.notification()._yPositionLegend = ylegendSpb->value();

//   data.notification()._levelTexts.clear();
//   data.notification()._levelTexts << level0Led->text().trimmed();
//   data.notification()._levelTexts << level1Led->text().trimmed();
//   data.notification()._levelTexts << level2Led->text().trimmed();
//   data.notification()._levelTexts << level3Led->text().trimmed();
//   data.notification()._levelTexts << level4Led->text().trimmed();  

//   // Campos do módulo de visualizacao
//   data.preview()._title		 = webTitleLed->text().trimmed();
//   data.preview()._logo		 = webLogoLed->text().trimmed();
//   data.preview()._longitude  = webLongitudeLed->text().trimmed();
//   data.preview()._latitude	 = webLatitudeLed->text().trimmed();
//   data.preview()._zoomLevel  = webZoomLed->text().trimmed();
//   data.preview()._googleKey  = webGoogleLed->text().trimmed();
//   data.preview()._terraOGC   = webTerraOGCLed->text().trimmed();
//   data.preview()._opacidade  = webOpacityLed->text().trimmed();
//   data.preview()._intervalo  = webIntervalLed->text().trimmed();
//   data.preview()._outputPath = webOutputLed->text().trimmed();

// }

// //! Retorna lista contendo as instâncias associadas a cada análise, na 
// // mesma ordem colocada na tabela
// void MainDialog::getLoadBalanceData(QList<int>& instances)
// {
//   instances.clear();
//   for(int i=0, size=loadBalTable->rowCount(); i<size; i++)
//   {
//     QTableWidgetItem* item = loadBalTable->item(i, 2);
//     instances.push_back(item->data(Qt::DisplayRole).toInt());
//   }
// }

// //! Limpa todos os campos da interface (com exceção da lista de configurações)
// void MainDialog::clearDialogData()
// {
//   _ignoreChangeEvents = true;

//   QString hostname = QHostInfo::localHostName();

//   // Campos da base de dados
//   dbTypeCmb->setCurrentIndex(0);
//   dbAddressLed->setText("");
//   dbPortLed->setText("");
//   dbUserLed->setText("");
//   dbPasswordLed->setText("");
//   dbDatabaseLed->setText("");
//   dbStudyChk->setChecked(false);

//   // Campos do módulo de análise
//   anaInstancesSpb->setValue(0);
//   anaAddressLed->setText(hostname);   
//   anaPortLed->setText("32105");
//   anaLogFileLed->setText("log_analise.txt");
//   anaIntervalMinSpb->setValue(5);
//   anaIntervalSecSpb->setValue(0);;
//   anaInstancesTable->clearContents();
//   anaInstancesTable->setRowCount(0);
//   anaDirNameLed->setText(QDir::homePath() + "/imgTerraMA2/");

//   // Campos do módulo de coleta
//   aqAddressLed->setText(hostname); 
//   aqPortLed->setText("32100");
//   aqLogFileLed->setText("log_coleta.txt");
//   aqTimeoutMinSpb->setValue(3);
//   aqTimeoutSecSpb->setValue(0);
//   aqDirNameLed->setText("");

//   // Campos do módulo de planos
//   layersAddressLed->setText(hostname);     
//   layersPortLed->setText("32101");
//   layersLogFileLed->setText("log_planos.txt");

//   // Campos do módulo de animacao
//   animAddressLed->setText(hostname);     
//   animPortLed->setText("32103");
//   animLogFileLed->setText("log_animacao.txt");
//   animImageDirLed->setText("");

//   // Campos do módulo de visualização
//   webTitleLed->setText("TerraMA2");
//   webLogoLed->setText("");
//   webLongitudeLed->setText("0.000");
//   webLatitudeLed->setText("0.000");
//   webZoomLed->setText("6");
//   webGoogleLed->setText("");
// #ifdef WIN32
//   webTerraOGCLed->setText("http://localhost/teogc/terraogcwmscgi.exe");
// #else
//   webTerraOGCLed->setText("http://localhost/teogc/terraogcwmscgi");
// #endif
//   webOpacityLed->setText("0.7");
//   webIntervalLed->setText("600000");
//   webOutputLed->setText("");

//   // Campos do módulo de notificação
//   notifAddressLed->setText(hostname);
//   notifPortLed->setText("32102");
//   notifLogFileLed->setText("log_notificacao.txt");
//   notifIntervalMinSpb->setValue(5);
//   notifIntervalSecSpb->setValue(0);
//   notifIntervalBulletinHouSpb->setValue(0);
//   notifIntervalBulletinMinSpb->setValue(5);
//   notifIntervalBulletinSecSpb->setValue(0);
//   removeZeroChk->setChecked(false);
//   bulletinPDFChk->setChecked(false);
//   pdfPathLed->setText("");
//   pdfPathLed->setEnabled(false);
//   selectPDFDirectoryBtn->setEnabled(false);
//   readConfirmationChk->setChecked(false);
//   notifSmtpServerLed->setText("");
//   notifSmtpPortLed->setText("");
//   notifSmtpMailLed->setText("");
//   notifSmtpPsswrdLed->setText("");
//   notifSmtpAliasLed->setText("");
//   notifSmtpTimeOutSpb->setValue(30000);
//   notifSmtpPriorityLed->setCurrentIndex(1);
//   notifSmtpSSLChk->setChecked(false);
//   notifProxyUserLed->setText("");
//   notifProxyPsswrdLed->setText("");
//   sendUpEmailChk->setChecked(false);
//   imagePDFChk->setChecked(false);

//   //texto alertas
//   level0Led->setText(tr("Nenhum"));
//   level1Led->setText(tr("Observação"));
//   level2Led->setText(tr("Atenção"));
//   level3Led->setText(tr("Alerta"));
//   level4Led->setText(tr("Alerta Máximo"));

//   legendDirNameLed->setText("");
//   xlegendSpb->setValue(600);
//   ylegendSpb->setValue(500);

//   // Campos da aba de balanceamento de carga
//   loadBalTable->clearContents();
//   loadBalTable->setRowCount(0);
//   _loadBalData.clear();

//   _ignoreChangeEvents = false;

//   // Dados na tela estão atualizados...
//   clearDataChanged();
// }

// //! Habilita ou desabilita campos de interface
// void MainDialog::enableFields(bool mode)
// {
//   // Acoes
//   saveAct->setEnabled(mode);
//   saveAsAct->setEnabled(mode);
//   renameAct->setEnabled(mode);
//   removeAct->setEnabled(mode);

//   consoleAct->setEnabled(mode);
//   servicesAct->setEnabled(mode);
  
//   dbCheckConnectionBtn->setEnabled(mode);
//   dbCreateDatabaseBtn->setEnabled(mode);
  
//   // Campos da base de dados
//   dbTypeCmb->setEnabled(mode);
//   dbAddressLed->setEnabled(mode);
//   dbPortLed->setEnabled(mode);
//   dbUserLed->setEnabled(mode);
//   dbPasswordLed->setEnabled(mode);
//   dbDatabaseLed->setEnabled(mode);
//   dbStudyChk->setEnabled(mode);

//   // Campos do módulo de análise
//   anaInstancesSpb->setEnabled(mode);
//   anaAddressLed->setEnabled(mode);
//   anaPortLed->setEnabled(mode);
//   anaLogFileLed->setEnabled(mode);
//   anaIntervalMinSpb->setEnabled(mode);
//   anaIntervalSecSpb->setEnabled(mode);
//   anaInstancesTable->setEnabled(mode);
//   anaInstancesRefreshBtn->setEnabled(mode);
//   anaDirNameLed->setEnabled(mode);

//   // Campos do módulo de coleta
//   aqAddressLed->setEnabled(mode);
//   aqPortLed->setEnabled(mode);
//   aqLogFileLed->setEnabled(mode);
//   aqTimeoutMinSpb->setEnabled(mode);
//   aqTimeoutSecSpb->setEnabled(mode);
//   aqDirNameLed->setEnabled(mode);

//   // Campos do módulo de planos
//   layersAddressLed->setEnabled(mode);
//   layersPortLed->setEnabled(mode);
//   layersLogFileLed->setEnabled(mode);

//   // Campos do módulo de animacao
//   animAddressLed->setEnabled(mode);
//   animPortLed->setEnabled(mode);
//   animLogFileLed->setEnabled(mode);
//   animImageDirLed->setEnabled(mode);
  
//   // Campos do módulo de visualização
//   webTitleLed->setEnabled(mode);
//   webLogoLed->setEnabled(mode);
//   webLongitudeLed->setEnabled(mode);
//   webLatitudeLed->setEnabled(mode);
//   webZoomLed->setEnabled(mode);
//   webGoogleLed->setEnabled(mode);
//   webTerraOGCLed->setEnabled(mode);
//   webOpacityLed->setEnabled(mode);
//   webIntervalLed->setEnabled(mode);
//   webOutputLed->setEnabled(mode);
//   selectLogoBtn->setEnabled(mode);
//   selectDirectoryBtn->setEnabled(mode);
//   generatePackageBtn->setEnabled(mode);

//   // Campos do módulo de notificação
//   notifAddressLed->setEnabled(mode);
//   notifPortLed->setEnabled(mode);
//   notifLogFileLed->setEnabled(mode);
//   notifIntervalMinSpb->setEnabled(mode);
//   notifIntervalSecSpb->setEnabled(mode);
//   notifIntervalBulletinHouSpb->setEnabled(mode);
//   notifIntervalBulletinMinSpb->setEnabled(mode);
//   notifIntervalBulletinSecSpb->setEnabled(mode);
//   notifSmtpServerLed->setEnabled(mode);
//   notifSmtpPortLed->setEnabled(mode);
//   notifSmtpMailLed->setEnabled(mode);
//   notifSmtpPsswrdLed->setEnabled(mode);
//   notifSmtpAliasLed->setEnabled(mode);
//   notifSmtpTimeOutSpb->setEnabled(mode);
//   notifSmtpPriorityLed->setEnabled(mode);
//   notifSmtpSSLChk->setEnabled(mode);
//   notifProxyUserLed->setEnabled(mode);
//   notifProxyPsswrdLed->setEnabled(mode);
//   sendUpEmailChk->setEnabled(mode);
//   imagePDFChk->setEnabled(mode);
//   removeZeroChk->setEnabled(mode);
//   bulletinPDFChk->setEnabled(mode);
//   if(mode)
//   {
// 	  pdfPathLed->setEnabled(bulletinPDFChk->isChecked());
// 	  selectPDFDirectoryBtn->setEnabled(bulletinPDFChk->isChecked());
//   }
//   else
//   {
// 	  pdfPathLed->setEnabled(mode);
// 	  selectPDFDirectoryBtn->setEnabled(mode);
//   }
  
//   readConfirmationChk->setEnabled(mode);
//   testSendMailBtn->setEnabled(mode);

//   legendDirNameLed->setEnabled(mode);
//   xlegendSpb->setEnabled(mode);
//   ylegendSpb->setEnabled(mode);

//   level0Led->setEnabled(mode);
//   level1Led->setEnabled(mode);
//   level2Led->setEnabled(mode);
//   level3Led->setEnabled(mode);
//   level4Led->setEnabled(mode);

//   // Campos da aba de balanceamento de carga
//   loadBalTable->setEnabled(mode);
//   loadBalRefreshBtn->setEnabled(mode);
// }

// //! Descarta alterações feitas na configuração atual.  Se restore_data = true, preenche tela com dados originais
// void MainDialog::discardChanges(bool restore_data)
// {
//   if(_newData)
//   {
//     // Estamos descartando uma configuração recém criada
//     // 1) Remove entrada da lista (interface).  Deve ser a última linha
//     assert(_currentConfigIndex == configListWidget->count()-1);
//     _ignoreChangeEvents = true;
//     delete configListWidget->takeItem(_currentConfigIndex);    
//     configListWidget->setCurrentRow(-1);
//     _currentConfigIndex = -1;
//     _ignoreChangeEvents = false;

//     // 2) Desmarca indicador de nova regra e de dados modificados
//     _newData = false;
//     clearDataChanged();  
    
//     // 3) Se precisamos restaurar os dados, marca a primeira linha da lista
//     if(restore_data)
//     {
//       if(configListWidget->count())
//         configListWidget->setCurrentRow(0);
//       else
//       {
//         clearDialogData();  
//         enableFields(false);
//       }  
//     }  
//   }
//   else
//   {
//     // Estamos descartando as edições feitas em uma configuração antiga
//     if(restore_data)
//     {
//       setDialogData(_configManager.configuration(_currentConfigIndex));
//       configListWidget->item(_currentConfigIndex)->setText(_name);
//     }  
//     else  
//       clearDataChanged(); 
//   }  
// }

// //! Mostra nome do arquivo associado à config. corrente na statusbar
// void MainDialog::showFileName()
// {
//   if(_currentConfigIndex == -1)
//     statusBar()->clearMessage();
//   else
//   {
//     QString msg;
//     if(_newData)
//       msg = tr("[Nova configuração] ");
//     else
//       msg = _configManager.configurationFile(_currentConfigIndex) + " ";

//     if(_dataChanged)
//       msg += tr("[Alterada]");
//     statusBar()->showMessage(msg);
//   }
// }


// /*! \brief Salva dados correntes, se válidos  
  
//   Verifica se os dados são válidos.  Se não forem, mostra mensagem
//   de erro e retorna false.  Se forem, tenta salvar.  Em caso de 
//   erro, mostra mensagem e retorna false. Se a operação de salvar
//   foi bem sucedida, retorna true.
  
//   \param newfile Indica se devemos usar o nome atual do arquivo ou 
//                  pedir ao usuário um novo nome
// */
// bool MainDialog::validateAndSaveChanges(bool newfile)
// {
//   QString err = "";
  
//   // Valida dados
//   if(!validate(err))
//   {
//     // Dados invalidos. Mostra mensagem de erro (a não ser que err esteja vazio)
//     if(!err.isEmpty())
//       QMessageBox::warning(this, tr("Erro validando dados..."), err);
//     return false;
//   }
  
//   // Se a configuração for nova, pede ao usuário que forneça o nome do arquivo
//   QString newfilename;
//   if(newfile)
//   {
//     newfilename = QFileDialog::getSaveFileName(this, tr("Informe o nome para a configuração"), 
//                                                ".", tr("Configurações (*.xml)"));
//     if(newfilename.isEmpty())
//       return false;

// 	if(!newfilename.endsWith(".xml"))
// 		newfilename.append(".xml");

//     if(_configManager.hasConfigurationFile(newfilename))
//     {
//       QMessageBox::warning(this, tr("Erro..."), 
//                            tr("Não é possível salvar uma configuração sobre um arquivo de configuração existente.\n"
//                               "Se desejar realmente sobrescrever uma configuração existente, remova-a primeiro."));
//       return false;
//     }
//   }
  
//   // Tenta salvar...
//   if(!save(newfilename, err))
//   {
//     // Se não conseguiu salvar, mostra mensagem de erro
//     QMessageBox::warning(this, tr("Erro salvando dados..."), 
//                          tr("Erro salvando dados.  Operação cancelada.\n%1").arg(err));
//     return false;
//   }
//   return true;
// }

// /*! \brief Verifica se os dados atuais foram alterados e 
//            viabiliza (ou não) troca de acordo com opção do usuário
           
// Se os dados estiverem atualizados, retorna true.  Se não estiverem, 
// pergunta ao usuário se deseja salvar as alterações, descartar as 
// alterações ou cancelar a operação.

// Processa a decisão do usuário.  Se uma tentativa de salvar os dados
// for falha, cancela a operação.

// \return O retorno da função indica se a operação de troca dos dados visualizados 
//         deve (retorno true) ou não prosseguir (retorno false). 
// */
// bool MainDialog::verifyAndEnableChange()
// {
//   // Se os dados atuais na tela não foram modificados, nada a fazer
//   if(!_dataChanged)
//     return true;

//   // Dados modificados.  Pergunta ao usuário se deseja salvar 
//   // alterações, abandonar alterações ou cancelar a seleção de 
//   // nova regra
//   QMessageBox::StandardButton answer;
//   answer = QMessageBox::question(this, tr("Modificações não salvas!"),
//                                  tr("As alterações efetuadas na tela atual ainda\n"
//                                     "não foram salvas.  Deseja salvar as alterações?"),
//                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
//                                  QMessageBox::Save);

//   // Se a operação foi cancelada, basta retornar false
//   if(answer == QMessageBox::Cancel)
//     return false;
  
//   // Se os dados foram descartados pelo usuário, chama 
//   // discardChanges() e retorna true
//   if(answer == QMessageBox::Discard)
//   {
//     discardChanges(false);
//     return true;
//   }
 
//   // Usuário pediu para tentarmos salvar os dados
//   // Se operação Ok, retorna true, senão retorna false cancelando a operação
//   return validateAndSaveChanges(_newData);
// }


// //! Slot chamado quando algum dado da interface foi alterado
// void MainDialog::setDataChanged()
// {
//   if(_ignoreChangeEvents)
//     return;

//   _dataChanged = true;
//   saveBtn->setEnabled(true);
//   cancelBtn->setEnabled(true);
//   showFileName();
// }

// //! Slot chamado quando o usuário pressiona o checkbox que configura o banco para estudos
// void MainDialog::studyChkClicked(bool checked)
// {
// 	anaInstancesSpb->setDisabled(checked);
// 	anaIntervalMinSpb->setDisabled(checked);
// 	anaIntervalSecSpb->setDisabled(checked);
// 	anaInstancesTable->setDisabled(checked);
// 	anaInstancesRefreshBtn->setDisabled(checked);
// 	loadBalTable->setDisabled(checked);
// 	loadBalRefreshBtn->setDisabled(checked);

// 	if(checked)
// 	{
// 		anaInstancesSpb->setValue(0);
// 		anaIntervalMinSpb->setValue(1);
// 		anaIntervalSecSpb->setValue(0);
// 	}
// }

// //! Slot chamado para indicar que os dados da interface estão atuais com a configuração
// void MainDialog::clearDataChanged()
// {
//   _dataChanged = false;
//   saveBtn->setEnabled(false);
//   cancelBtn->setEnabled(false);
//   showFileName();
// }

// //! Slot chamado para criação de uma nova configuração
// void MainDialog::newRequested()
// {
//   // Verifica se podemos ir para nova analise
//   if(!verifyAndEnableChange())
//     return;
    
//   // Ok. Podemos criar nova entrada.
//   // 1) Gera um nome temporário para a análise
//   _name = tr("Nova configuração");
  
//   // 2) Inclui nome na lista de configurações e seleciona a mesma
//   //    Antes de selecionar, marca que estamos tratando de uma nova config.
//   _newData = true;
//   configListWidget->addItem(_name);
//   configListWidget->setCurrentRow(configListWidget->count()-1);
  
//   // 3) Tratador de seleção já limpou a tela.  Habilita os campos (lista podia estar vazia)
//   enableFields(true);
//   setDataChanged();
  
//   // 4) Chama diálogo para renomear config.
//   renameRequested();
// }

// //! Slot chamado para abertura de uma configuração não contida na lista de configurações
// void MainDialog::openRequested()
// {
//   // Verifica se podemos ir para nova analise
//   if(!verifyAndEnableChange())
//     return;

//   // Pega o nome do arquivo    
//   QString filename = QFileDialog::getOpenFileName(this, tr("Escolha uma configuração"),
//                                                   ".", tr("Configurações (*.xml)"));
//   if(filename.isEmpty())
//     return;
    
//   // Carrega
//   QString err;
//   if(!_configManager.loadConfiguration(filename, err))
//   {
//     QMessageBox::warning(this, tr("Erro abrindo configuração..."), err); 
//     return;
//   }
  
//   // Adiciona nova entrada criada na lista e seleciona a mesma
//   int pos = _configManager.numConfigurations() - 1;
//   configListWidget->addItem(_configManager.configuration(pos).name());
//   configListWidget->setCurrentRow(pos);  
//   enableFields(true);
//   dbStudyChk->setEnabled(false); //Nao é possivel editar a opcao que define se um banco é destinado a estudos 
//   if(dbStudyChk->isChecked())
// 	  studyChkClicked(true);
// }

// //! Slot chamado para salvar edições efetuadas na configuração corrente
// void MainDialog::saveRequested()
// {
//   if(_dataChanged)
//     validateAndSaveChanges(_newData);
// }

// //! Slot chamado para salvar edições efetuadas como uma nova configuração
// void MainDialog::saveAsRequested()
// {
//   if(validateAndSaveChanges(true))
//   {
//     // Adiciona nova entrada criada na lista e seleciona a mesma
//     int pos = _configManager.numConfigurations() - 1;
//     configListWidget->addItem(_configManager.configuration(pos).name());
//     configListWidget->setCurrentRow(pos);  
//   }
// }

// //! Slot chamado para renomear a configuração corrente
// void MainDialog::renameRequested()
// {
//   bool ok;
//   QString newname = QInputDialog::getText(this, tr("Renomear..."),
//                                           tr("Novo nome da configuração:"), 
//                                           QLineEdit::Normal,
//                                           _name, &ok);
//   if(!ok || newname.isEmpty())
//     return;
         
//   _name = newname;
//   configListWidget->item(_currentConfigIndex)->setText(_name);
//   setDataChanged();       
// }

// //! Slot chamado para remoção da configuração corrente
// void MainDialog::removeRequested()
// {
//   // Verifca com o usuário se ele deseja realmente remover a config.
//   QMessageBox::StandardButton answer;
//   answer = QMessageBox::question(this, tr("Remover configuração..."), 
//                                  tr("A configuração selecionada será apagada permanentemente do sistema.\n"
//                                     "Deseja realmente remover esta configuração?"),
//                                  QMessageBox::Yes | QMessageBox::No,
//                                  QMessageBox::No);
//   if(answer == QMessageBox::No)
//     return;

//   // Remove objeto do _configManager (se não for uma config. nova)
//   QString err;
//   if(!_newData)
//   {
//     if(!_configManager.removeConfiguration(_currentConfigIndex, true, err))
//     {
//       QMessageBox::warning(this, tr("Erro removendo configuração..."), err); 
//       return;
//     }
//   }
  
//   // Remove objeto da lista (interface) e desmarca seleção
//   _ignoreChangeEvents = true;
//   delete configListWidget->takeItem(_currentConfigIndex);    
//   configListWidget->setCurrentRow(-1);
//   _currentConfigIndex = -1;
//   _ignoreChangeEvents = false;

//   // Desmarca indicadores de modificação e nova regra
//   clearDataChanged();  
//   _newData = false;

//   // Seleciona a primeira linha ou limpa campos se não houver nenhuma entrada na lista.
//   if(configListWidget->count())
//     configListWidget->setCurrentRow(0);
//   else
//   {
//     clearDialogData();  
//     enableFields(false);
//   }  
// }

// //! Slot chamado para cancelar edições na configuração corrente
// void MainDialog::cancelRequested()
// {
//   discardChanges(true);
// }

// //! Evento chamado quando o usuário solicita o encerramento da aplicação
void MainDialog::closeEvent(QCloseEvent* close)
{
  // if(verifyAndEnableChange())
  //   close->accept();
  // else
  //   close->ignore();  
}

// //! Ação chamada quando o usuário solicita a visualização dos consoles de uma configuração
// void MainDialog::showConsoles()
// {
//   // Dados devem ser salvos antes de podermos usar os consoles
//   if(_dataChanged)
//   {
//     // Dados modificados.  Pergunta ao usuário se deseja salvar 
//     // alterações ou cancelar a operação
//     QMessageBox::StandardButton answer;
//     answer = QMessageBox::question(this, tr("Modificações não salvas!"),
//                                    tr("Para acessar os consoles é necessário que as alterações na\n"
//                                       "configuração sejam salvas.  Deseja salvar as alterações?"),
//                                    QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save);

//     // Se a operação foi cancelada, retorna
//     if(answer == QMessageBox::Cancel)
//       return;

//     // Usuário pediu para tentarmos salvar os dados
//     // Se operação deu error, retorna
//     if(!validateAndSaveChanges(_newData))
//       return;
//   }

//   // Mostra dialogo
//   ConsoleDlg dlg(_configManager.configuration(_currentConfigIndex));
//   dlg.exec();
// }

// //! Ação chamada quando o usuário deseja gerenciar os serviços associados a uma configuração
// void MainDialog::manageServices()
// {
//   // Dados devem ser salvos antes de podermos usar o gerenciador de serviços
//   if(_dataChanged)
//   {
//     // Dados modificados.  Pergunta ao usuário se deseja salvar 
//     // alterações ou cancelar a operação
//     QMessageBox::StandardButton answer;
//     answer = QMessageBox::question(this, tr("Modificações não salvas!"),
//                                    tr("Para acessar o gerenciador de serviços é necessário que as\n"
//                                       "alterações na configuração sejam salvas.  Deseja salvar as alterações?"),
//                                    QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save);

//     // Se a operação foi cancelada, retorna
//     if(answer == QMessageBox::Cancel)
//       return;

//     // Usuário pediu para tentarmos salvar os dados
//     // Se operação deu error, retorna
//     if(!validateAndSaveChanges(_newData))
//       return;
//   }

//   // Executa dialogo.  Este será encarregado de alterar a configuração se o usuario
//   // Modificar algum parâmetro e salvar
//   ServicesDlg dlg(_configManager, _currentConfigIndex);
//   dlg.exec();
// }

// //! Slot chamado quando o usuário pede para que a conexão com a base de dados seja verificada
// void MainDialog::dbCheckConnectionRequested()
// {
//   // Valida config. de conexão contida na tela
//   QString err;
//   if(!validateDbData(err))
//   {
//     if(!err.isEmpty())
//       QMessageBox::warning(this, tr("Erro validando dados para conexão com base..."), err);
//     return;  
//   }
  
//   // Obtem dados da interface
//   ConfigData newvalues;
//   getDialogData(newvalues);
  
//   // Verifica conexão
//   DbManager::checkConnection(newvalues.db());
// }

// //! Slot chamado quando o usuário pede a criação de uma nova base de dados
// void MainDialog::dbCreateDatabaseRequested()
// {
//   // Valida config. de conexão contida na tela
//   QString err;
//   if(!validateDbData(err))
//   {
//     if(!err.isEmpty())
//       QMessageBox::warning(this, tr("Erro validando dados para conexão com base..."), err);
//     return;  
//   }
  
//   // Obtem dados da interface
//   ConfigData newvalues;
//   getDialogData(newvalues);
  
//   // Cria nova base
//   DbManager::createDatabase(newvalues.db());
// }


// //! Slot chamado quando o usuário pede para recarregar os dados da tabela de instâncias
// void MainDialog::loadBalanceReloadData()
// {
//   // Valida config. de conexão contida na tela
//   QString err;
//   if(!validateDbData(err))
//   {
//     if(!err.isEmpty())
//       QMessageBox::warning(this, tr("Erro validando dados para conexão com base..."), err);
//     return;  
//   }
  
//   // Obtem dados de conexão a partir da interface
//   ConfigData dbconf;
//   getDialogData(dbconf);

//   // Carrega dados
//   QList<DbManager::AnaData> data;
//   if(!DbManager::loadAnaInstanceData(dbconf.db(), data))
//     return;

//   // Preenche tabela
//   _ignoreChangeEvents = true;
//   loadBalTable->setRowCount(data.size());
//   for(int i=0, size=data.size(); i<size; i++)
//   {
//     DbManager::AnaData& a = data[i];
//     QTableWidgetItem* item;
    
//     item = new QTableWidgetItem(QString::number(a._id));
//     item->setFlags(item->flags() & ~Qt::ItemIsEditable);
//     loadBalTable->setItem(i, 0, item);
    
//     item = new QTableWidgetItem(a._name);
//     loadBalTable->setItem(i, 1, item);
//     item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    
//     item = new QTableWidgetItem();
//     item->setData(Qt::DisplayRole, (unsigned int)a._instance); // cast para unsigned garante que delegate vai permitir a entrada apenas de números > 0
//     loadBalTable->setItem(i, 2, item);
//   }
//   loadBalTable->resizeRowsToContents();
//   loadBalTable->setAlternatingRowColors(true);
//   _ignoreChangeEvents = false;

//   // ATualiza copia em memoria dos dados
//   _loadBalData = data;
// }

// //! Slot chamado quando o usuário deseja recarregar a tabela de parâmetros das inst. adicionais
// void MainDialog::reloadInstancesTableData()
// {
//   anaInstancesTable->setRowCount(0);
//   numInstancesChanged(anaInstancesSpb->value());
// }

// //! Altera o número de linhas da tabela de parâmetros de instâncias adicionais de acordo com alterações no spinbox
// void MainDialog::numInstancesChanged(int val)
// {
//   int oldcount = anaInstancesTable->rowCount();
//   anaInstancesTable->setRowCount(val);

//   if(val <= oldcount)
//     return;

//   QString host      = anaAddressLed->text().trimmed();   
//   int     base_port = anaPortLed->text().toInt();
//   QString base_log  = anaLogFileLed->text().trimmed();

//   int pos = base_log.lastIndexOf('.');
//   if(pos != -1)
//     base_log.insert(pos, "_%1");

//   for(int i=oldcount; i<val; i++)
//   {
//     QTableWidgetItem* item;
    
//     item = new QTableWidgetItem(QString::number(i+2));
//     item->setFlags(item->flags() & ~Qt::ItemIsEditable);
//     anaInstancesTable->setItem(i, 0, item);
    
//     anaInstancesTable->setItem(i, 1, new QTableWidgetItem(host));
    
//     item = new QTableWidgetItem();
//     item->setData(Qt::DisplayRole, (unsigned int)base_port+i+1); // cast para unsigned garante que delegate vai permitir a entrada apenas de números > 0
//     anaInstancesTable->setItem(i, 2, item);

//     anaInstancesTable->setItem(i, 3, new QTableWidgetItem(base_log.arg(i+2)));
//   }

//   anaInstancesTable->resizeRowsToContents();
// }

// //! Slot chamado quando a linha corrente é alterada na lista de configurações
// void MainDialog::configListSelectionChanged(const QItemSelection& selected, const QItemSelection& oldSelection)
// {
//   if(_ignoreChangeEvents)
//     return;

//   QModelIndexList selected_indexes = selected.indexes();
  
//   // Se usuário clicou na lista fora de qq. item, remarca item anterior
//   if(!selected_indexes.count())
//   {
//     if(oldSelection.indexes().count()) // Evita loop infinito se não existir seleção anterior...
//       configListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
//     return;
//   }
  
//   // Obtem a linha selecionada    
//   int row = selected_indexes[0].row();

//   // Verifica se estamos apenas voltando à mesma opção atual.  Ocorre 
//   // quando uma troca de regra foi cancelada
//   if(row == _currentConfigIndex)
//     return;

//   bool ok = true;
  
//   // Verifica se os dados atuais na tela foram modificados
//   // e em caso positivo se podemos efetuar a troca de dados
//   if(_currentConfigIndex != -1)
//     ok = verifyAndEnableChange();

//   if(ok)
//   {
//     // Operação permitida.  Troca dados na tela
//     _currentConfigIndex = row;
//     if(_newData)
//       clearDialogData(); // Estamos tratando a seleção de uma análise recém incluida na lista
//     else 
//     {
//       setDialogData(_configManager.configuration(row)); // Estamos tratando uma seleção normal feita pelo usuário
//       _ignoreChangeEvents = true;
//       configListWidget->setCurrentRow(_currentConfigIndex);
//       _ignoreChangeEvents = false;
//     }  
//   }
//   else
//   {
//     // Operação foi cancelada.  Devemos reverter à regra original
//     configListWidget->selectionModel()->select(oldSelection, QItemSelectionModel::SelectCurrent);
//     configListWidget->setCurrentRow(_currentConfigIndex);
//   }    
// }

// //! Slot chamado quando o botão testSendMail é clicado
// void MainDialog::testSendMailRequested()
// {
// 	// Obtem dados da interface
// 	ConfigData newvalues;
// 	getDialogData(newvalues);

// 	QString     addr;
// 	QByteArray  addrstr;

// 	//envia o email de teste
// 	std::auto_ptr<ModNotificacao> mod(new ModNotificacao);
// 	addr = newvalues.notification()._address + ":" + QString::number(newvalues.notification()._servicePort);
// 	mod->endpoint = (addrstr = addr.toLatin1()).data();

// 	//verifica se o notificacao esta no ar
// 	std::string ping;
// 	if(mod->notification__ping(ping) != SOAP_OK)
// 	{
// 		QMessageBox::information(NULL, tr("Envio de email de teste"), QObject::tr("Serviço de notificação desligado."));
// 		return;
// 	}

// 	//pede o email a ser enviado o email de teste
// 	bool ok;
// 	QString text = QInputDialog::getText(NULL, tr("Envio de email de teste"), tr("Email para onde será enviado o teste:"), QLineEdit::Normal, "", &ok);
// 	if (!ok || text.isEmpty())
// 		return;

// 	std::string email = text.toStdString();

// 	//envia o email de teste
// 	int ee = 0;
// 	if (mod->notification__sendTestMail(email, newvalues.notification()._smtpServer.toStdString(), newvalues.notification()._smtpPort, newvalues.notification()._smtpAlias.toStdString(),
// 		newvalues.notification()._smtpMail.toStdString(), newvalues.notification()._smtpPsswrd.toStdString(), newvalues.notification()._smtpTimeout, newvalues.notification()._smtpSSL,
// 		newvalues.notification()._proxyUser.toStdString(), newvalues.notification()._proxyPsswrd.toStdString(), newvalues.notification()._smtpPriority.toStdString(), ee) == SOAP_OK)
// 	{
// 		QMessageBox::information(NULL, tr("Envio de email de teste"), QObject::tr("Email de teste enviado com sucesso."));
// 	}
// 	else
// 	{
// 		QMessageBox::information(NULL, tr("Envio de email de teste"), QObject::tr("Erro ao enviar email de teste."));
// 	}
// }

// //! Validação dos itens de interface da customizacao do módulo de visualizacao
// bool MainDialog::validateCustomPreview()
// {
// 	if(webTitleLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina um título para o módulo de visualização!"));
// 		return false;
// 	}
	
// 	if(webLongitudeLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina a longitude inicial para o módulo de visualização!"));
// 		return false;
// 	}

// 	if(webLatitudeLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina a latitude inicial para o módulo de visualização!"));
// 		return false;
// 	}
	
// 	if(webZoomLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina o nível de zoom inicial para o módulo de visualização!"));
// 		return false;
// 	}

// 	if(webTerraOGCLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina o endereço do servidor TerraOGC para o módulo de visualização!"));
// 		return false;
// 	}

// 	if(webOpacityLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina a opacidade inicial para o módulo de visualização!"));
// 		return false;
// 	}

// 	if(webIntervalLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina o intervalo de atualização para o módulo de visualização!"));
// 		return false;
// 	}

// 	if(webOutputLed->text().trimmed().size() == 0)
// 	{
// 		QMessageBox::warning(NULL, tr("Validação da Customização do módulo de visualização"), QObject::tr("Defina o diretório que será gerado a customização do módulo de visualização!"));
// 		return false;
// 	}
	
// 	return true;
// }

// //! Slot chamado quando o botão generatePackageBtn é clicado para criar a customização do módulo de visualizacao
// void MainDialog::generatePackage()
// {
// 	if(!validateCustomPreview())
// 		return;

// 	ConfigData::Preview preview;
// 	preview._title		= webTitleLed->text().trimmed();
// 	preview._logo		= webLogoLed->text().trimmed();
// 	preview._longitude  = webLongitudeLed->text().trimmed();
// 	preview._latitude	= webLatitudeLed->text().trimmed();
// 	preview._zoomLevel  = webZoomLed->text().trimmed();
// 	preview._googleKey  = webGoogleLed->text().trimmed();
// 	preview._terraOGC   = webTerraOGCLed->text().trimmed();
// 	preview._opacidade  = webOpacityLed->text().trimmed();
// 	preview._intervalo  = webIntervalLed->text().trimmed();
// 	preview._outputPath = webOutputLed->text().trimmed();

// 	ConfigData::Animation animation;
// 	animation._address = animAddressLed->text().trimmed();
// 	animation._servicePort = animPortLed->text().toInt();
// 	animation._logFile = animLogFileLed->text().trimmed();
// 	animation._imageDirectory = animImageDirLed->text().trimmed();

// 	ConfigData::DataBaseServer database;
// 	database._driver   = (ConfigData::DatabaseDriver)dbTypeCmb->currentIndex(); // Ordem dos itens na combo = ordem enumeração
// 	database._hostName = dbAddressLed->text().trimmed();
// 	database._port     = dbPortLed->text().toInt();
// 	database._user     = dbUserLed->text().trimmed();
// 	database._password = dbPasswordLed->text().trimmed();
// 	database._dbName   = dbDatabaseLed->text().trimmed();
// 	database._study    = dbStudyChk->isChecked();

// 	PreviewCustom custom(preview, animation, database);
// 	if(custom.generate())
// 		QMessageBox::warning(NULL, tr("Customização do módulo de visualização"), QObject::tr("Customização do módulo de visualização gerada com sucesso!"));	
// }

// //! Slot chamado quando o botão selectLogoBtn é clicado para escolher o logo que será apresentado no módulo de visualizacao
// void MainDialog::selectLogo()
// {
// 	QString filename = QFileDialog::getOpenFileName(this, tr("Escolha um logo para o módulo de visualização"),
// 		".", tr("Imagem (*.png *.jpg *jpeg)"));
// 	if(filename.isEmpty())
// 		return;

// 	webLogoLed->setText(filename);
// 	setDataChanged();
// }

// //! Slot chamado quando o botão selectDirectoryBtn é clicado para escolher o logo que será apresentado no módulo de visualizacao
// void MainDialog::selectOutputPath()
// {
// 	QString filename = QFileDialog::getExistingDirectory(this, tr("Escolha um diretório de saída para a customização do módulo de visualização"),
// 		".");
// 	if(filename.isEmpty())
// 		return;

// 	webOutputLed->setText(filename);
// 	setDataChanged();
// }

// //! Slot chamado quando o botão selectPDFDirectoryBtn é clicado para escolher o diretorio para copiar o PDF
// void MainDialog::selectPDFPath()
// {
// 	QString filename = QFileDialog::getExistingDirectory(this, tr("Escolha um diretório de cópia dos Boletins em PDF"),
// 		".");
// 	if(filename.isEmpty())
// 		return;

// 	pdfPathLed->setText(filename);
// 	setDataChanged();
// }
