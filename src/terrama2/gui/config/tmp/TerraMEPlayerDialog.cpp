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
  \file terrama2/gui/config/TerraMEPlayerDialog.cpp

  \brief Definition of Class TerraMEPlayerDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

// TerraMA2
#include "TerraMEPlayerDialog.hpp"
#include "AnalysisList.hpp"

// QT
#include <QDir>

///////////////////////////////////////////////////////////////////////////////
// 
// TerraMEAnalysisResult
// 
///////////////////////////////////////////////////////////////////////////////

//! Construtor da classe auxiliar TerraMEAnalysisResult
TerraMEPlayerDialog::TerraMEAnalysisResult::TerraMEAnalysisResult(QDateTime dateTime, QFileInfoList imageFiles)
{
	_dateTime = dateTime;
	_imageFiles = imageFiles; 
}

//! Operador usado na ordenacao de objetos TerraMEAnalysisResult
const bool TerraMEPlayerDialog::TerraMEAnalysisResult::operator<(const TerraMEPlayerDialog::TerraMEAnalysisResult & res) const
{
	return (this->_dateTime < res.getDateTime());
}

///////////////////////////////////////////////////////////////////////////////
// 
// TerraMEPlayerDialog
// 
///////////////////////////////////////////////////////////////////////////////

//! Construtor
TerraMEPlayerDialog::TerraMEPlayerDialog(Services* manager, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
	setupUi(this);
	_manager = manager;
	_currentAnalysisImageIndex = 0;
	_imageCountLabel->setText("");

	_imageFileFilter.append("terrama2_*.png"); //Este filtro deve ser igual ao usado para gerar as imagens das analises TerraME (ver AnalysisRule.cpp)
	_imageCountText = tr("Imagem: %1 / %2");

	connect(_terraMEAnalysisListWidget, SIGNAL(currentRowChanged(int)), SLOT(analysisSelected(int)));
	connect(_datetimeListWidget, SIGNAL(currentRowChanged(int)), SLOT(datetimeSelected(int)));

	connect(_previousBtn, SIGNAL(clicked()), SLOT(showPreviousImage()));
	connect(_nextBtn, SIGNAL(clicked()), SLOT(showNextImage()));
	connect(_rewindBtn, SIGNAL(clicked()), SLOT(rewindClicked()));
	connect(_forwardBtn, SIGNAL(clicked()), SLOT(forwardClicked()));
	connect(_pauseBtn, SIGNAL(clicked()), SLOT(pauseClicked()));

	connect(&_changeResultTimer, SIGNAL(timeout()), this, SLOT(changeResultSlot()));
	_changeResultTimer.setSingleShot(false);

	reloadInterface();
}

//! Destrutor
TerraMEPlayerDialog::~TerraMEPlayerDialog()
{
}

//! Recarrega a lista de analises disponiveis e os arquivos associados a cada uma delas
void TerraMEPlayerDialog::reloadInterface()
{
	// Limpar interface e objetos que guardam em memoria os resultados das analises TerraME
	_terraMEAnalysisListWidget->clear();
	_datetimeListWidget->clear();
	_terraMEImageAnalysisWidget->setImageAnalysis("");
	_terraMEResult.clear();
	_currentAnalysis.clear();
	_currentAnalysisResult.clear();
	_currentAnalysisImageIndex = 0;
	_imageCountLabel->setText("");
	_changeResultTimer.stop();

	// Obter lista de analises TerraME que estao na base de dados e atualizar a interface
	AnalysisList* analysisList = _manager->analysisList();
	for(int i = 0; analysisList && i < analysisList->count(); ++i)
	{
		Analysis* ana = analysisList->at(i);
		if(ana->getAnalysisType() == WS_ANALYSISTYPE_TERRAME)
		{
			QListWidgetItem* item = new QListWidgetItem(ana->name());
			item->setData(Qt::UserRole, ana->id());

			_terraMEAnalysisListWidget->addItem(item);
		}	
	}

	// Carregar os logs das analises TerraME
	std::vector<wsAnalysisLog> terraMEAnalysisLogs;
	_manager->getAllTerraMEAnalysisLogs(terraMEAnalysisLogs);

	// Montar objeto que vai guardar, para cada analise TerraME, um conjunto de timestamps que 
	// representa os momentos em que cada analise foi executada. E para cada timestamp estara associado
	// um conjunto de imagens geradas.
	for(int i = 0; i < terraMEAnalysisLogs.size(); ++i)
	{
		wsAnalysisLog log = terraMEAnalysisLogs[i];

		// Acessar diretorio de imagens desta analise
		QDir dir(log.terraMEImagePath.c_str());
		if(dir.exists())
		{
			// Obter lista de imagens, ordenadas pela hora da modificacao do arquivo.
			QFileInfoList imageFileNameList = dir.entryInfoList(_imageFileFilter, QDir::Files, QDir::Name);

			// Se foram encontradas imagens, guardar referencia para elas
			if(!imageFileNameList.isEmpty())
			{
				QDateTime dateTime(QDate(log.dateStartYear, log.dateStartMonth, log.dateStartDay),
								   QTime(log.timeStartHour, log.timeStartMinutes, log.timeStartSeconds));

				TerraMEAnalysisResult analysisResult(dateTime, imageFileNameList);
				_terraMEResult[log.analysisId].push_back(analysisResult);
			}
		}
	}
	
	// Ordenar resultados pela data/hora da execucao da analise
	QMapIterator<int, QList<TerraMEAnalysisResult> > it(_terraMEResult);
	while(it.hasNext())
	{
		it.next();
		qSort(_terraMEResult[it.key()].begin(), _terraMEResult[it.key()].end());
	}

	// Se alguma analise TerraME foi encontrada, selecionar a primeira
	if(_terraMEAnalysisListWidget->count() > 0)
	{
		_terraMEAnalysisListWidget->setCurrentRow(0);
	}
}

//! Slot chamado quando o usuario altera a analise selecionada para visualizacao
void TerraMEPlayerDialog::analysisSelected(int currentRow)
{
	_datetimeListWidget->clear();
	_terraMEImageAnalysisWidget->setImageAnalysis("");
	_currentAnalysis.clear();
	_currentAnalysisResult.clear();
	_currentAnalysisImageIndex = 0;
	_imageCountLabel->setText("");
	_changeResultTimer.stop();
	
	QListWidgetItem* item = _terraMEAnalysisListWidget->item(currentRow);
	int anaID = item ? item->data(Qt::UserRole).toInt() : -1;

	// Verificar se a analise ja foi executada
	if(anaID >= 0 && _terraMEResult.contains(anaID))
	{
		_currentAnalysis = _terraMEResult[anaID];

		// Preencher a lista com a data/hora de cada execucao da analise
		for(int i = 0; i < _currentAnalysis.size(); ++i)
		{
			QString timestampStr = _currentAnalysis[i].getDateTime().toString("dd/MM/yyyy HH:mm:ss");
			_datetimeListWidget->addItem(new QListWidgetItem(timestampStr));
		}

		// Selecionar o primeiro elemento da lista
		if(_datetimeListWidget->count() > 0)
		{
			_datetimeListWidget->setCurrentRow(0);
		}
	}
}

//! Slot chamado quando o usuario altera a data/hora para um valor especifico, exibindo os resultados da analise corrente para aquele momento
void TerraMEPlayerDialog::datetimeSelected(int currentRow)
{
	_terraMEImageAnalysisWidget->setImageAnalysis("");
	_currentAnalysisResult.clear();
	_currentAnalysisImageIndex = 0;
	_imageCountLabel->setText("");
	_changeResultTimer.stop();

	// O elemento de indice i do objeto _datetimeListWidget deve representar o elemento de mesmo indice na lista _currentAnalysis
	if(currentRow >= 0 && currentRow < _currentAnalysis.size())
	{
		_currentAnalysisResult = _currentAnalysis[currentRow].getFileInfoList();

		// Mostrar a primeira imagem da lista de resultados da analise
		showFirstImage();
	}
}

//! Mostra, se possivel, a primeira imagem da lista de resultados de uma analise na data/hora corrente
void TerraMEPlayerDialog::showFirstImage()
{
	if(_currentAnalysisResult.size() > 0)
	{
		_currentAnalysisImageIndex = 0;
		_terraMEImageAnalysisWidget->setImageAnalysis(_currentAnalysisResult[0].absoluteFilePath());

		_imageCountLabel->setText(_imageCountText.arg(1).arg(_currentAnalysisResult.size()));
	}
}

//! Mostra, se possivel, a imagem anterior a que estiver sendo exibida
void TerraMEPlayerDialog::showPreviousImage()
{
	if(_currentAnalysisImageIndex >= 1 && _currentAnalysisImageIndex - 1 < _currentAnalysisResult.size())
	{
		--_currentAnalysisImageIndex;
		_terraMEImageAnalysisWidget->setImageAnalysis(_currentAnalysisResult[_currentAnalysisImageIndex].absoluteFilePath());

		_imageCountLabel->setText(_imageCountText.arg(_currentAnalysisImageIndex + 1).arg(_currentAnalysisResult.size()));
	}
}

//! Mostra, se possivel, a proxima imagem
void TerraMEPlayerDialog::showNextImage()
{
	if(_currentAnalysisImageIndex >= 0 && _currentAnalysisImageIndex + 1 < _currentAnalysisResult.size())
	{
		++_currentAnalysisImageIndex;
		_terraMEImageAnalysisWidget->setImageAnalysis(_currentAnalysisResult[_currentAnalysisImageIndex].absoluteFilePath());

		_imageCountLabel->setText(_imageCountText.arg(_currentAnalysisImageIndex + 1).arg(_currentAnalysisResult.size()));
	}
}

//! Slot chamado quando o usuario solicita a animacao dos resultados, exibindo a imagem anterior em cada passo
void TerraMEPlayerDialog::rewindClicked()
{
	_forward = false;
	_changeResultTimer.start(1000);
}

//! Slot chamado quando o usuario solicita a animacao dos resultados, exibindo a proxima imagem em cada passo
void TerraMEPlayerDialog::forwardClicked()
{
	_forward = true;
	_changeResultTimer.start(1000);
}

//! Slot chamado quando o usuario deseja pausar uma animacao
void TerraMEPlayerDialog::pauseClicked()
{
	_changeResultTimer.stop();
}

//! Slot chamado durante uma animacao. Atualiza a imagem que esta sendo exibida
void TerraMEPlayerDialog::changeResultSlot()
{
	if(_forward)
		showNextImage();
	else
		showPreviousImage();

	// Verificar se chegou ao inicio ou ao fim da lista de imagens a serem exibidas
	if( _currentAnalysisImageIndex == 0 || _currentAnalysisImageIndex == (_currentAnalysisResult.size() - 1) )
		_changeResultTimer.stop();
}