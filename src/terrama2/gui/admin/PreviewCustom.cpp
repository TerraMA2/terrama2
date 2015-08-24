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
  \file terrama2/gui/admin/PreviewCustom.cpp

  \brief Definition of PreviewCustom Class methods

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Raphael Meloni
*/


#include "PreviewCustom.hpp"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include "JlCompress.h"

//! Construtor
PreviewCustom::PreviewCustom(ConfigData::Preview preview, ConfigData::Animation animacao, ConfigData::DataBaseServer database)
{
	_preview   = preview;
	_animation = animacao;
	_database  = database;
}

//! Destrutor
PreviewCustom::~PreviewCustom()
{
}

bool PreviewCustom::createFolder(QDir& output, QString dirName, bool cd )
{
	if(!output.mkdir(dirName))
	{
		QMessageBox::warning(NULL, QObject::tr("Validação da Customização do módulo de visualização"), QObject::tr("Não foi possível criar o diretório %1, verifique sua permissão de escrita nesse diretório!").arg(dirName.toUpper()));
		return false;
	}
	if(cd)
		output.cd(dirName);

	return true;
}

void PreviewCustom::recursiveRemove(const QString d)
{
	QDir dir(d);
	if (dir.exists())
	{
		const QFileInfoList list = dir.entryInfoList();
		QFileInfo fi;
		for (int l = 0; l < list.size(); l++)
		{
			fi = list.at(l);
			if (fi.isDir() && fi.fileName() != "." && fi.fileName() != "..")
				recursiveRemove(fi.absoluteFilePath());
			else if (fi.isFile())
			{
				QFile f( fi.absoluteFilePath() );
				bool ret = f.remove();				
			}

		}
		dir.rmdir(d);
	}
}

bool PreviewCustom::generateFolders()
{
	QDir outputDir(_preview._outputPath);
	if(!outputDir.exists())
	{
		QMessageBox::warning(NULL, QObject::tr("Validação da Customização do módulo de visualização"), QObject::tr("Diretório de saída da customização do módulo de visualização não existe!"));
		return false;
	}

	if(outputDir.exists("AlertasWeb"))
	{
		QMessageBox::StandardButton answer;
		answer = QMessageBox::question(NULL, QObject::tr("Validação da Customização do módulo de visualização"),
			QObject::tr("Já existe uma customização no diretório escolhido, deseja remover a customização anterior?"),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes);

		// Se a operação foi cancelada, basta retornar false
		if(answer == QMessageBox::No)
			return false;		

		recursiveRemove(outputDir.absolutePath() + "/AlertasWeb");
	}

	if(!createFolder(outputDir,"AlertasWeb",true))
		return false;

	if(!createFolder(outputDir,"script",false))
		return false;

	if(!createFolder(outputDir,"image",false))
		return false;
	
	if(!createFolder(outputDir,"WEB-INF",true))
		return false;

	if(!createFolder(outputDir,"classes",true))
		return false;
	
	//if (_database._driver == ConfigData::DRIVER_MySQL)
	{
		//é necessário criar os arquivos xml do hibernate
		if(!createFolder(outputDir,"org",true))
			return false;

		if(!createFolder(outputDir,"alertasweb",true))
			return false;

		if(!createFolder(outputDir,"terrama2",true))
			return false;
	}

	return true;
}

//! Cria a customização do módulo de visualizacao
bool PreviewCustom::generate()
{
	//primeiro cria a estrutura de diretorio
	if(!generateFolders())
		return false;

	//segundo customiza o arquivo config.js
	processConfig();

	//terceiro customiza o arquivo hibernate.cfg.xml
	processHibernate();

	//quarto processa os arquivos de binding do hibernate
	//if (_database._driver == ConfigData::DRIVER_MySQL)
		processHibernateFiles();

	//quinto copia para logo.png
	processLogo();
	
	//sexto zipa o conteudo no arquivo
	zipPackage();
	return true;
}

void PreviewCustom::processConfig()
{
	QString config = readFile(":/webfiles/config");

#ifndef WIN32
	_preview._terraOGC.remove(".exe");
#endif	

	config = config.replace("Config.TerraMA2Name = \"TerraMA2\";","Config.TerraMA2Name = \""+ _preview._title +"\";");
	config = config.replace("Config.TerraOGCPath = \"http://localhost/teogc/terraogcwmscgi.exe\";","Config.TerraOGCPath = \""+ _preview._terraOGC +"\";");

	config = config.replace("Config.WarningCenterLongitude = -52.00928;","Config.WarningCenterLongitude = "+ _preview._longitude +";");
	config = config.replace("Config.WarningCenterLatitude = -24.053;","Config.WarningCenterLatitude = "+ _preview._latitude +";");

	config = config.replace("Config.WarningZoomLevel = 6;","Config.WarningZoomLevel = "+ _preview._zoomLevel +";");

	config = config.replace("Config.WarningRefreshInterval = 60000*10;","Config.WarningRefreshInterval = "+ _preview._intervalo +";");

	config = config.replace("Config.WMSLayerOpacity = 0.7;","Config.WMSLayerOpacity = "+ _preview._opacidade +";");

	if (!_preview._googleKey.isEmpty())
		config = config.replace("Config.GoogleKey = null;","Config.GoogleKey = \""+ _preview._googleKey +"\";");

	config = config.replace("Config.AnimationAddress = \"http://localhost:32103\";","Config.AnimationAddress = \"http://"+ _animation._address + ":" + QString("%1").arg(_animation._servicePort) +"\";");

	saveFile(_preview._outputPath + "/AlertasWeb/script/config.js", config);
}

void PreviewCustom::processHibernate()
{
	QString hibernate = readFile(":/webfiles/hibernate");

	hibernate = hibernate.replace("<property name=\"hibernate.connection.username\">postgres</property>","<property name=\"hibernate.connection.username\">"+_database._user+"</property>");
	hibernate = hibernate.replace("<property name=\"hibernate.connection.password\">k2123</property>","<property name=\"hibernate.connection.password\">"+_database._password+"</property>");

	if (_database._driver == ConfigData::DRIVER_MySQL)
	{
		hibernate = hibernate.replace("<property name=\"hibernate.dialect\">org.hibernate.dialect.PostgreSQLDialect</property>","<property name=\"hibernate.dialect\">org.hibernate.dialect.MySQLDialect</property>");
		hibernate = hibernate.replace("<property name=\"hibernate.connection.driver_class\">org.postgresql.Driver</property>","<property name=\"hibernate.connection.driver_class\">com.mysql.jdbc.Driver</property>");

		hibernate = hibernate.replace("<property name=\"hibernate.connection.url\">jdbc:postgresql://localhost:5432/tbgGIS</property>","<property name=\"hibernate.connection.url\">jdbc:mysql://"+ _database._hostName + ":" + QString("%1").arg(_database._port) + "/" + _database._dbName +"</property>");
	}
	else
	{
		hibernate = hibernate.replace("<property name=\"hibernate.connection.url\">jdbc:postgresql://localhost:5432/tbgGIS</property>","<property name=\"hibernate.connection.url\">jdbc:postgresql://"+ _database._hostName + ":" + QString("%1").arg(_database._port) + "/" + _database._dbName +"</property>");
	}

	saveFile(_preview._outputPath + "/AlertasWeb/WEB-INF/classes/hibernate.cfg.xml", hibernate);
}

void PreviewCustom::processHibernateFiles()
{
	QMap<QString, QString> fileMap;
	fileMap.insert(":/webfiles/tetheme","TeTheme.hbm.xml");
	fileMap.insert(":/webfiles/wsadma","WsAdma.hbm.xml");
	fileMap.insert(":/webfiles/wsanat","WsAnat.hbm.xml");
	fileMap.insert(":/webfiles/wsanlo","WsAnlo.hbm.xml");
	fileMap.insert(":/webfiles/wsanru","WsAnru.hbm.xml");
	fileMap.insert(":/webfiles/wsblge","WsBlge.hbm.xml");
	fileMap.insert(":/webfiles/wsbull","WsBull.hbm.xml");
	fileMap.insert(":/webfiles/wsdcon","WsDcon.hbm.xml");
	fileMap.insert(":/webfiles/wsdico","WsDico.hbm.xml");
	fileMap.insert(":/webfiles/wspins","WsPins.hbm.xml");
	fileMap.insert(":/webfiles/wsrima","WsRima.hbm.xml");
	fileMap.insert(":/webfiles/wsrmpr","WsRmpr.hbm.xml");
	fileMap.insert(":/webfiles/wsuser","WsUser.hbm.xml");
	fileMap.insert(":/webfiles/wsusvi","WsUsvi.hbm.xml");
	fileMap.insert(":/webfiles/wswalo","WsWalo.hbm.xml");	

	QMapIterator<QString, QString> i(fileMap);
	while (i.hasNext()) {
		i.next();
		processHibernateFile(i.key(), i.value());
	}	
}

void PreviewCustom::processHibernateFile(QString filename, QString outfilename)
{
	QString hibernate = readFile(filename);

	if (_database._driver == ConfigData::DRIVER_MySQL)
		hibernate = hibernate.replace("schema=\"public\"","schema=\""+ _database._dbName +"\"");

	saveFile(_preview._outputPath + "/AlertasWeb/WEB-INF/classes/org/alertasweb/terrama2/" + outfilename, hibernate);
}

void PreviewCustom::saveFile( QString fileName, QString content )
{
	QFile output(fileName);
	if (!output.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&output);
	out << content;

	output.close();
}

QString PreviewCustom::readFile( QString fileName )
{
	QFile input(fileName);
	input.open(QFile::ReadOnly | QFile::Text);
	QString content = input.readAll();
	input.close();

	return content;
}

void PreviewCustom::zipPackage()
{
	QString outputZip(_preview._outputPath + "/AlertasWeb.zip");
	QString outputDir(_preview._outputPath + "/AlertasWeb");
	JlCompress::compressDir(outputZip,outputDir);
}

void PreviewCustom::processLogo()
{
	if(!QFile::exists(_preview._logo))
		return;

	QImage img(_preview._logo);
	QString outputImage(_preview._outputPath + "/AlertasWeb/image/logo.png");
	img.save(outputImage,"PNG");
}
