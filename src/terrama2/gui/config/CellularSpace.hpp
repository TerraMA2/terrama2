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
  \file terrama2/gui/config/CellularSpace.hpp

  \brief Definition of methods in class CellularSpace.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Gustavo Sampaio
*/


#ifndef _CELLULARSPACE_H_
#define _CELLULARSPACE_H_


// QT
#include <QString>
#include <QDate>

#include "soapServPlanosProxy.h"

//! Classe respons�vel por representar um espa�o celular
class CellularSpace
{
public:
	CellularSpace();
	CellularSpace(const wsCellularSpace& data);
	~CellularSpace();

	//! Retorna o identificador do espa�o celular
	int id() const { return _data.id; }

	void setId(int id);  

	//! Retorna true se o espa�o celular for novo e ainda n�o salvo na base de dados
	bool isNew() const { return _data.id == -1; }

	//! Retorna o nome do espa�o celular
	QString name() const { return QString::fromStdString(_data.name); }

	//! Altera o nome do espa�o celular
	void setName(QString name) { _data.name = name.toStdString(); }

	//! Retorna o nome do autor do espa�o celular
	QString author() const { return QString::fromStdString(_data.author); }

	//! Altera o nome do autor do espa�o celular
	void setAuthor(QString author) { _data.author = author.toStdString(); }

	//! Retorna o nome da institui��o a que o autor est� associado
	QString institution() const { return QString::fromStdString(_data.institution); }

	//! Altera o nome da institui��o a que o autor est� associado
	void setInstitution(QString name) { _data.institution = name.toStdString(); }

	//! Retorna uma descri��o do espa�o celular
	QString description() const { return QString::fromStdString(_data.details); }

	//! Altera a descri��o do espa�o celular
	void setDescription(QString description) { _data.details = description.toStdString(); }

	//! Retorna a data de cria��o do espa�o celular
	QDate creationDate() const { return QDate(_data.creationYear, _data.creationMonth, _data.creationDay); }

	void setCreationDate(QDate date);

	//! Retorna a data de expira��o do espa�o celular
	QDate expirationDate() const { return QDate(_data. expirationYear, _data. expirationMonth, _data. expirationDay); }

	void setExpirationDate(QDate date);

	//! Retorna o n�mero de atributos do espa�o celular
	int numAttributes() const { return _data.baseTheme.attributes.size(); }

	//! Retorna o nome do atributo de indice index
	QString attributeName(int index) const { return QString::fromStdString(_data.baseTheme.attributes.at(index).name); }

	//! Retorna o tipo do atributo de indice index
	wsColumnType attributeType(int index) const { return _data.baseTheme.attributes.at(index).type; }

	//! Retorna a estrutura que empacota os dados do espa�o celular
	const wsCellularSpace& data() const { return _data; }

	//! Retorna a estrutura que empacota o tema do espa�o celular
	const wsTheme& theme() const { return _data.baseTheme; }

	//! Altera o Tema
	void setThemeStructure(wsTheme theme) { _data.baseTheme = theme; }

private:
	wsCellularSpace _data;  //! Dados do espa�o celular empacotados na estrutura lida / recebida do servi�o de planos
};


#endif

