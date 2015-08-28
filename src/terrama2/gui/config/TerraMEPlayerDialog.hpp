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
  \file terrama2/gui/config/TerraMEPlayerDialog.hpp

  \brief Definition of Class TerraMEPlayerDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Gustavo Sampaio
*/


/*!
* 
*
* \brief Definicao da classe TerraMEPlayerDialog, que representa a interface onde o
*		 usuario visualiza os resultados das analises TerraME.
*
*
*/

#ifndef _TerraMEPlayerDialog_H_
#define _TerraMEPlayerDialog_H_

// TerraMA2
#include "Services.hpp"

// QT
#include <QFileInfoList>
#include <QDateTime>
#include <QTimer>
#include "ui_TerraMEPlayerDialog.h"

class TerraMEPlayerDialog : public QDialog, private Ui::TerraMEPlayerDialog
{
	Q_OBJECT

public:
	TerraMEPlayerDialog(Services* manager, QWidget* parent = 0, Qt::WFlags f = 0);
	~TerraMEPlayerDialog();

private slots:
	void reloadInterface();
	void analysisSelected(int currentRow);
	void datetimeSelected(int currentRow);
	void showPreviousImage();
	void showNextImage();
	void rewindClicked();
	void forwardClicked();
	void pauseClicked();
	void changeResultSlot();

private:

	/**
	 * Classe que auxilia a organizacao dos resultados de uma analise TerraME, que posteriormente serao exibidos.
	 * Cada objeto armazena uma data/hora e uma lista de imagens geradas naquele momento, para uma determinada analise.
	 */
	class TerraMEAnalysisResult
	{
	public:
		TerraMEAnalysisResult(QDateTime dateTime, QFileInfoList imageFiles);
		~TerraMEAnalysisResult() {}
		const bool operator<(const TerraMEAnalysisResult & ) const;
		QDateTime getDateTime() const { return  _dateTime; }
		QFileInfoList getFileInfoList() { return _imageFiles; }
	private:
		QDateTime _dateTime;
		QFileInfoList _imageFiles;
	};

	void showFirstImage();

	Services* _manager;
	QMap<int, QList<TerraMEAnalysisResult> > _terraMEResult; //!< A chave eh o id da analise e o valor eh uma lista de objetos que representam cada execucao da analise
	QList<TerraMEAnalysisResult> _currentAnalysis; //!< Representa a analise que esta selecionada na interface
	QFileInfoList _currentAnalysisResult; //!< Lista contendo os caminhos das imagens geradas na data/hora que estiver selecionada na interface
	int _currentAnalysisImageIndex; //!< Inteiro usado para controlar qual imagem deve ser exibida
	QStringList _imageFileFilter; //!< Filtro usado para selecionar os arquivos que sao as imagens geradas por uma analise TerraME
	QString _imageCountText; //!< Texto exibido na interface. Mostra o contador de imagens
	QTimer _changeResultTimer; //!< Timer usado para atualizar a imagem durante uma animacao
	bool _forward; //!< Possui valor true se a animacao deve ser feita a partir da primeira imagem gerada para a ultima
};

#endif