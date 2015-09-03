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
  \file terrama2/gui/admin/DbManager.cpp

  \brief Definition of DbManager Class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class DbManager

Clase responsável pelas funções de interface com a base de dados
TerraLib.

Permite ao usuário verificar conexões, criar novas bases, atualizar 
o esquema da base e também gerenciar a alocação de análises a instâncias 
do serviço de análises.
*/

#include <assert.h>

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>


#include "DbManager.hpp"
#include "AutoDatabase.hpp"
#include "AutoTransaction.hpp"
#include "Version.hpp"
#include "TeUpdateDBVersion.h"

#include "waitDlg.h"

#include <memory>

/*! \brief Verifica se é possível estabelecer uma conexão com a base de dados
           fornecida na configuração recebida como parâmetro.
           
Se a conexão for bem sucedida, verifica também se o esquema da base é apropriado

Mostra mensagem de progresso e resultado do teste           
*/
void DbManager::checkConnection(const ConfigData::DataBaseServer& config)
{
  WaitDlg dlg(WaitDlg::tr("Conectando com a base de dados..."));
  
  // Estabelece conexão
  AutoDatabase db(config);
	if(!db->isConnected())
	{
	  dlg.hide();
	  QMessageBox::warning(NULL, WaitDlg::tr("Erro conectando à base..."), db->errorMessage().c_str());
	  return;
  }

  // Verifica versão da base TerraLib
  dlg.setMsg(WaitDlg::tr("Verificando versão TerraLib..."));
  QCoreApplication::processEvents();
  std::string teVersion;
  bool teOk = !needUpdateDB(db, teVersion);
  
  // Verifica versão TerraMA²
  dlg.setMsg(WaitDlg::tr("Verificando versão TerraMA²..."));
  QCoreApplication::processEvents();
  QString sisErr;
  int sisSchema;
  bool sisOk = wsVersion::checkSchema(db, sisErr, &sisSchema);
  
  // Conexão Ok e esquema Ok.
  if(teOk && sisOk) 
  {
    dlg.hide();
    QMessageBox::information(NULL, WaitDlg::tr("Conexão com a base de dados"), QObject::tr("Conexão com a base de dados bem sucedida."));
    return;
  } 
  
  // Ops, temos que atualizar o esquema.  Pergunta para o usuário se ele deseja realmente fazer isso 
  QString who = teOk ? "" : "Terralib";
  if(!sisOk)
  {
    if(who != "") who += WaitDlg::tr(" e ");
    who += "TerraMA²"; 
  }
  QString msg = WaitDlg::tr("O esquema da base de dados precisa ser atualizado (%1)!\n\n"
                            "Esta operação pode ser demorada e sugere-se que uma cópia\n"
                            "de segurança da base seja efetuada antes de sua atualização.\n\n"
                            "Deseja atualizar a base agora?").arg(who);
  
  QMessageBox::StandardButton answer;
  answer = QMessageBox::question(NULL, WaitDlg::tr("Aviso!"), msg, 
                                 QMessageBox::Yes | QMessageBox::No, 
                                 QMessageBox::No);
  if(answer == QMessageBox::No)
    return;  // Nao vamos atualizar a base agora.

  // Atualização autorizada.
  // 1) Terralib
  if(!teOk)
  {
    dlg.setMsg(WaitDlg::tr("Atualizando versão Terralib..."));
    QCoreApplication::processEvents();
    std::string teError;
    if(!updateDBVersion(db, teVersion, teError))
    {
	    dlg.hide();
	    QMessageBox::warning(NULL, WaitDlg::tr("Erro atualizando versão Terralib..."), teError.c_str());
	    return;
    } 
  }
                              
  // 2) TerraMA²
  if(!sisOk)
  {
    dlg.setMsg(WaitDlg::tr("Atualizando versão TerraMA²..."));
    QCoreApplication::processEvents();
    if(!updateTerraMA2Version(db, config._driver, sisSchema, config._study, sisErr))
    {
	    dlg.hide();
	    QMessageBox::warning(NULL, WaitDlg::tr("Erro atualizando versão TerraMA²..."), sisErr);
	    return;
    } 
  }

  // Mostra mensagem de sucesso
  dlg.hide();
  QMessageBox::information(NULL, WaitDlg::tr("Atualização de esquema:"), QObject::tr("Base de dados atualizada com sucesso."));
}

//! Cria nova base de dados para uso com o TerraMA²
bool DbManager::createDatabase(const ConfigData::DataBaseServer& config)
{
  WaitDlg dlg(WaitDlg::tr("Criando nova base de dados..."));
  
  // Cria a base
  AutoDatabase db;
	if(!db.create(config))
	{
		AutoDatabase db(config);
		dlg.hide();

		if(db->isConnected())
			QMessageBox::warning(NULL, WaitDlg::tr("Erro criando base..."), QObject::tr("Base de dados já existe!"));
		else
			QMessageBox::warning(NULL, WaitDlg::tr("Erro criando base..."), db->errorMessage().c_str());
		
		return false;
	}
  
  // Cria tabelas do esquema TerraMA²
  dlg.setMsg(WaitDlg::tr("Criando tabelas..."));
  QCoreApplication::processEvents();
  
  QString dbPrefix = "PostgreSQL";
  if(config._driver == ConfigData::DRIVER_MySQL)
    dbPrefix = "MySQL";
  QFile schemaFile(":/sql/" + dbPrefix + "_database_script.sql");
  schemaFile.open(QFile::ReadOnly);
  QString schema = QString::fromAscii(schemaFile.readAll().constData());
 
  string err;
  if(!AutoDatabase::executeMultiStatements(db, schema.toStdString(), err))
  {
	  dlg.hide();
	  QMessageBox::warning(NULL, WaitDlg::tr("Erro criando tabelas da nova base..."), err.c_str());
	  return false;
  }

  //Se o banco é configurado para estudos, atualizar tabela com as configuracoes do BD
  if(config._study)
  {
	  if(!db->execute("UPDATE ws_dbco "
					  "SET dbcobo_studydb = TRUE;"))
	  {
		  dlg.hide();
		  QMessageBox::warning(NULL, WaitDlg::tr("Erro ao configurar banco de dados para estudos..."), db->errorMessage().c_str());
		  return false;
	  }
  }

  {
	  std::vector<std::string> imagens;
	  imagens.push_back("/images/none.png");
	  imagens.push_back("/images/blue.png");
	  imagens.push_back("/images/orange.png");
	  imagens.push_back("/images/yellow.png");
	  imagens.push_back("/images/red.png");

	  std::auto_ptr<TeDatabasePortal> portal(db->getPortal());
	  try_transaction(db)
	  {
		  for (int i = 0; i < imagens.size(); i++)
		  {
			  QString filename = QString("%1%2").arg(QCoreApplication::applicationDirPath()).arg(QString::fromStdString(imagens[i]));
			  if(!QFile::exists(filename))
				  continue;

			  if (!db->execute("INSERT INTO ws_imag (imagedata) VALUES (NULL);")) 
			  {
				  dlg.hide();
				  QMessageBox::warning(NULL, WaitDlg::tr("Erro ao adicionar uma imagem ao banco de dados..."), db->errorMessage().c_str());
				  return false;
			  }

			  int newImageID;

			  // Recuperar o ID do registro recém-criado:
			  if (!portal->query("SELECT MAX(image_id) FROM ws_imag") || !portal->fetchRow())
			  {
				  dlg.hide();
				  QMessageBox::warning(NULL, WaitDlg::tr("Erro ao adicionar uma imagem ao banco de dados..."), db->errorMessage().c_str());
				  return false;
			  }
			  else
				  // OK, temos o ID! Armazená-lo no valor de retorno:
				  newImageID = portal->getInt(0);

			  portal->freeResult();

			  // Converter imageData para unsigned char *
			  if(!db->insertBlob("ws_imag", "imagedata", "image_id = "+Te2String(newImageID),filename.toStdString()))
			  {
				  dlg.hide();
				  QMessageBox::warning(NULL, WaitDlg::tr("Erro ao adicionar uma imagem ao banco de dados..."), db->errorMessage().c_str());
				  return false;			  
			  }
		  }
	  }
	  catch_transaction(TransactionException & exc)
	  {
		  return false;
	  }	  
  }

  // Mostra mensagem de sucesso
  dlg.hide();
  QMessageBox::information(NULL, WaitDlg::tr("Nova base de dados:"), QObject::tr("Base de dados criada com sucesso."));
  return true;
}

//! Remove uma base de dados para uso com o TerraMA²
bool DbManager::dropDatabase(ConfigData::DataBaseServer& config)
{
	WaitDlg dlg(WaitDlg::tr("Removendo a base de dados..."));

	QString droppedDatabase =  config._dbName;
	if(config._driver == ConfigData::DRIVER_MySQL)
		config._dbName = QString("mysql");
	else if(config._driver == ConfigData::DRIVER_PostgreSQL)
		config._dbName = QString("postgres");
	else if(config._driver == ConfigData::DRIVER_PostGIS)
		config._dbName = QString("template_postgis");

	bool dropped = false;
	AutoDatabase db(config);
	if(db->isConnected())
	{
		dropped = db->dropDatabase(droppedDatabase.toStdString());
	}
	dlg.hide();
	QMessageBox::information(NULL, WaitDlg::tr("Removendo a base de dados:"), QObject::tr("Base de dados removida %1 sucesso.").arg(dropped ? "com" : "sem"));
	return dropped;
}


/*! Atualiza versão atual do esquema TerraMA² da base de dados
*/
bool DbManager::updateTerraMA2Version(TeDatabase* db, ConfigData::DatabaseDriver driver, int current_schema, bool isStudyDB, QString& err)
{
  assert(current_schema < wsVersion::Schema);  

  QString dbPrefix = "PostgreSQL";
  if(driver == ConfigData::DRIVER_MySQL)
    dbPrefix = "MySQL";
  
 	try_transaction(db)
	{
		if(current_schema == 0 && !db->tableExist("ws_anru"))
		{
			// O banco de dados nao possui tabelas do TerraMA².
			// Aplicar o script zero, que ira criar o banco completo
			QFile schemaFile(":/sql/" + dbPrefix + "_database_script.sql");
			schemaFile.open(QFile::ReadOnly);
			QString schema = QString::fromAscii(schemaFile.readAll().constData());

			string errString;
			if(!AutoDatabase::executeMultiStatementsInTransaction(db, schema.toStdString(), errString))
			{
				err = errString.c_str();
				return false;
			}

			//Se o banco é configurado para estudos, atualizar tabela com as configuracoes do BD
			if(isStudyDB)
			{
				if(!db->execute("UPDATE ws_dbco "
								"SET dbcobo_studydb = TRUE;"))
				{
					err = db->errorMessage().c_str();
					return false;
				}
			}
		}
		else
		{
			// Aplica todos os scripts necessarios para atualizar a versao atual da base (current_schema)
			// para a versão mais atual (wsVersion::Schema).
			for(int i=current_schema+1; i<=wsVersion::Schema; i++)
			{
				QString rsrc = QString(":/sql/%1_database_update_script_%2.sql").arg(dbPrefix).arg(i);
		   	  
 				QFile schemaFile(rsrc);
				schemaFile.open(QFile::ReadOnly);
				QString schema = QString::fromAscii(schemaFile.readAll().constData());
		    
				string updateError;
				if(!AutoDatabase::executeMultiStatementsInTransaction(db, schema.toStdString(), updateError))
				{
					err = updateError.c_str();
					return false;
				}
			}


		}
    }
 	catch_transaction (TransactionException & exc) 
	{
		err = QString("%1 :\n%2").arg(exc.what()).arg(exc.why());
		return false;
	}

	//Realiza outras atualizações
	for(int i=current_schema+1; i<=wsVersion::Schema; i++)
	{
		if(i == 5)
		{
			//inclui as colunas de unidade no anru_ID_warning_data
			if (!updateTerraMA2Version5(db,err))
			{
				return false;
			}
		}
	}
  
  return true;
}

/*! \brief Carrega lista de análises e instânicas a qual cada uma deleas 
           está associada 
*/
bool DbManager::loadAnaInstanceData(const ConfigData::DataBaseServer& config,
                                    QList<AnaData>& data)
{ 
  WaitDlg dlg(WaitDlg::tr("Conectando com a base de dados..."));
  
  // Estabelece conexão
  AutoDatabase db(config);
	if(!db->isConnected())
	{
	  dlg.hide();
	  QMessageBox::warning(NULL, WaitDlg::tr("Erro conectando à base..."), db->errorMessage().c_str());
	  return false;
  }

  // Obtem dados
  dlg.setMsg(WaitDlg::tr("Obtendo lista de análises..."));
  QCoreApplication::processEvents();

  data.clear();
  
	std::auto_ptr<TeDatabasePortal> portal(db->getPortal());

  if(!portal->query("SELECT anruse_id, anruvc_desc, anruin_instance FROM ws_anru ORDER BY anruvc_desc"))
  {
	  dlg.hide();
	  QMessageBox::warning(NULL, WaitDlg::tr("Erro ao consultar a base..."), portal->errorMessage().c_str());
	  return false;
  }

  while(portal->fetchRow())
  {
	  AnaData a;	
		a._id				= portal->getInt("anruse_id");
		a._name			= portal->getData("anruvc_desc");
		a._instance	= portal->getInt("anruin_instance");
		data.push_back(a);
  }

  // Fim!
  dlg.hide();
  return true;  
}                                  

/*! \brief Atualiza na base a instância a que as análises estão associadas 

Todas as análises contidas na lista serão atualizadas
*/
bool DbManager::saveAnaInstanceData(const ConfigData::DataBaseServer& config,
                                    const QList<AnaData>& data)
{
  WaitDlg dlg(WaitDlg::tr("Conectando com a base de dados..."));
  
  // Estabelece conexão
  AutoDatabase db(config);
	if(!db->isConnected())
	{
	  dlg.hide();
	  QMessageBox::warning(NULL, WaitDlg::tr("Erro conectando à base..."), db->errorMessage().c_str());
	  return false;
  }

  dlg.setMsg(WaitDlg::tr("Atualizando dados..."));
  QCoreApplication::processEvents();

  foreach(AnaData a, data)
  {
    std::string query = "UPDATE ws_anru SET "
                        " anruin_instance = " + Te2String(a._instance) +
                        ", anruts_last_modified = NOW() "
                        "WHERE anruse_id = " + Te2String(a._id);
    if(!db->execute(query))
    {
  	  dlg.hide();
	    QMessageBox::warning(NULL, WaitDlg::tr("Erro atualizando dados..."), db->errorMessage().c_str());
	    return false;
    }
  }

  dlg.hide();
  return true;
}                                

bool DbManager::updateTerraMA2Version5( TeDatabase* db, QString& err )
{
	//cria um novo portal
	TeDatabasePortal* portal = db->getPortal();
	if(!portal)
		return false;

	//realiza a consulta
	if(!portal->query("SELECT anruse_id, warning_theme_id FROM ws_anru"))
	{
		delete portal;
		return false;
	}

	std::vector<int> anruVector;
	std::vector<int> anruVectorTheme;
	while(portal->fetchRow())
	{
		//para cada analise verifica se a coluna existe
		int anruId = portal->getInt("anruse_id");
		int themeId = portal->getInt("warning_theme_id");
		anruVector.push_back(anruId);
		anruVectorTheme.push_back(themeId);
	}

	//para todas as analises
	while (anruVector.size())
	{
		//obtem o id da analise
		int anruId = anruVector[anruVector.size()-1];
		int themeId = anruVectorTheme[anruVectorTheme.size()-1];
		anruVector.pop_back();
		anruVectorTheme.pop_back();

		//se não existe a coluna, logo é necessário cria-la
		//"SELECT data_unit FROM anru_" + Te2String(anruId) + "_warning_data")
		TeAttribute attr;
		if(db->tableExist("anru_" + Te2String(anruId) + "_warning_data") && !db->columnExist("anru_" + Te2String(anruId) + "_warning_data", "data_unit", attr))
		{
			std::string sql = "ALTER TABLE anru_" + Te2String(anruId) + "_warning_data ADD COLUMN data_unit character varying(30)";
			if(!db->execute(sql))
			{
				delete portal;
				return false;
			}
		}

		std::string warningTableName = "anru_" + Te2String(anruId) + "_warning_level";
		if(!db->tableExist(warningTableName))
		{
			if(themeId < 1)
				continue;

			TeTheme warningTheme;
			warningTheme.id(themeId);

			if(!db->loadTheme(&warningTheme))
				return false;

			TeAttrTableVector& themeTables = warningTheme.attrTables();

			QString warningThemeTable      = QString::fromStdString(themeTables[0].name()).toLower();
			int     warningThemeTableId    = themeTables[0].id();
			QString warningThemeTableLink  = QString::fromStdString(themeTables[0].linkName()).toLower();

			TeAttributeList attributeList;
			{
				TeAttribute id;
				id.rep_.name_ = "id_w";
				id.rep_.type_ = TeINT;
				id.rep_.isPrimaryKey_ = true;
				id.rep_.isAutoNumber_ = true;
				id.rep_.null_ = false;
				attributeList.push_back(id);

				TeAttribute oid;
				oid.rep_.name_    = "object_id_w";
				oid.rep_.type_    = TeSTRING;
				oid.rep_.numChar_ = 128;
				oid.rep_.null_    = false;
				attributeList.push_back(oid);

				TeAttribute warningLevel;
				warningLevel.rep_.name_ = "warning_level";
				warningLevel.rep_.type_ = TeINT;
				warningLevel.rep_.null_ = false;
				attributeList.push_back(warningLevel);
			}

			// Cria tabela no banco
			if(!db->createTable(warningTableName, attributeList))
				return false;

			// Cria mesma tabela em memoria
			TeTable warningTable(warningTableName, attributeList, "id_w", "object_id_w", TeAttrExternal);
			warningTable.setTableType(TeAttrExternal, warningThemeTableId, warningThemeTableLink.toStdString());
			warningTable.relatedTableName(warningThemeTable.toStdString());	  

			// Cadastra a tabela como uma tabela externa      
			if(!db->insertTableInfo(-1, warningTable)) // -1 indica que é uma tab. externa
				return false;

			// Associa tabela externa com tema 
			if(!db->insertThemeTable(&warningTheme, warningTable) || !warningTheme.addThemeTable(warningTable))
				return false;

			if(!warningTheme.createCollectionAuxTable() || !warningTheme.populateCollectionAux())
				return false;
		}
	}

	delete portal;

	return true;
}