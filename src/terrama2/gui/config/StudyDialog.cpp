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
  \file terrama2/gui/config/StudyDialog.cpp

  \brief Definition of Class StudyDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

// STL
#include <assert.h>

// TerraMA2  
#include "Utils.hpp"
#include "StudyDialog.hpp"

// QT  
#include <QMessageBox>
#include <QCalendarWidget>

//! Construtor
StudyDialog::StudyDialog(Services* manager, wsTimestampInterval interval, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);
  
  _manager = manager;
  _page = 0;
  _currentAnalysisEvent = 0;
  _pauseRequested = false;

  connect(endBtn, SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(nextBtn, SIGNAL(clicked()), SLOT(nextRequested()));
  connect(previousBtn, SIGNAL(clicked()), SLOT(previousRequested()));

  connect(playBtn, SIGNAL(clicked()), SLOT(playClicked()));
  connect(pauseBtn, SIGNAL(clicked()), SLOT(pauseClicked()));

  connect(eventsTbl, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(enablePause(QTableWidgetItem*)));
  connect(toleranceCbx, SIGNAL(stateChanged(int)), SLOT(enableTolerance(int)));
  connect(showAnalysisOnlyCbx, SIGNAL(stateChanged(int)), SLOT(showAnalysisOnly(int)));

  connect(&_analysisTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeoutSlot()));

  toleranceLed->setValidator(new QIntValidator(toleranceLed));

  previousBtn->setEnabled(false);
  pauseBtn->setEnabled(false);

  //Preencher QComboBox com os niveis de alertas disponiveis
  for(int i=0; i<5; ++ i)
	warningLevelCmb->addItem(Utils::warningLevelToString(i));

  //Definir o intervalo de tempo valido para o estudo de acordo com os dados ja coletados
  QDate initialDate(interval.initialYear, interval.initialMonth, interval.initialDay);
  QTime initialTime(interval.initialHour, interval.initialMinutes, interval.initialSeconds);
  QDate finalDate(interval.finalYear, interval.finalMonth, interval.finalDay);
  QTime finalTime(interval.finalHour, interval.finalMinutes, interval.finalSeconds);

  initialTimestampTed->setMinimumDate(initialDate);
  initialTimestampTed->setMinimumTime(initialTime);
  initialTimestampTed->setMaximumDate(finalDate);
  initialTimestampTed->setMaximumTime(finalTime);
  
  finalTimestampTed->setMinimumDate(initialDate);
  finalTimestampTed->setMinimumTime(initialTime);
  finalTimestampTed->setMaximumDate(finalDate);
  finalTimestampTed->setMaximumTime(finalTime);

  initialTimestampTed->setDateTime(QDateTime(initialDate, initialTime));
  initialTimestampTed->calendarWidget()->setSelectedDate(initialDate);
  finalTimestampTed->setDateTime(QDateTime(finalDate, finalTime));
  finalTimestampTed->calendarWidget()->setSelectedDate(finalDate);

  eventsTbl->setColumnWidth(0,50);
  eventsTbl->setColumnWidth(1,70);
  eventsTbl->setColumnWidth(2,50);
  eventsTbl->setColumnWidth(3,130);
  eventsTbl->setColumnWidth(4,200);

  std::string imageDir; 
  _manager->getImageDirectory(imageDir);
  _imageAnalysisDlg = new ImageAnalysisDlg(imageDir.c_str(), this, Qt::Window);
}

//! Destrutor
StudyDialog::~StudyDialog()
{
	delete _imageAnalysisDlg;
}

void StudyDialog::nextRequested()
{
  if(_page >= 3)
	  return;

  if(_page == 0)
  {
	  if(initialTimestampTed->dateTime() >= finalTimestampTed->dateTime())
	  {
		  QMessageBox::critical(this, tr("Erro..."), tr("A data inicial deve ser menor que a data final!"));
		  return;
	  }

	  _manager->clearStudyDatabase();
  }

  _page++;
  pageStack->setCurrentIndex(_page);

  if(_page == 3)
  {
	  nextBtn->hide();
	  previousBtn->hide();
	  cancelBtn->hide();
  }
  else if(_page == 2)
  {
	  nextBtn->setText(tr("Terminar"));
	  previousBtn->hide();
	  fillStudyEventsTbl();
  }
  else if(_page == 1)
	  previousBtn->setEnabled(true);
}

void StudyDialog::previousRequested()
{
  if(_page == 0)
    return;
  _page--;
  pageStack->setCurrentIndex(_page);

  if(_page == 2)
    nextBtn->setEnabled(true);
  else if(_page == 0)
    previousBtn->setEnabled(false);
}

void StudyDialog::enableTolerance(int enable)
{
	toleranceLed->setEnabled(enable);
}

void StudyDialog::showAnalysisOnly(int enable)
{
	if(enable)
	{
		for(int i=0; i<_studyEvents.size(); ++i)
			if(_studyEvents.at(i).eventType == WS_STUDYEVENT_COLLECTION)
				eventsTbl->hideRow(i);
	}
	else
	{
		for(int i=0; i<_studyEvents.size(); ++i)
			if(_studyEvents.at(i).eventType == WS_STUDYEVENT_COLLECTION)
				eventsTbl->showRow(i);
	}
}

void StudyDialog::enablePause(QTableWidgetItem* item)
{
	if(item != NULL)
	{
		int row = item->row();
		wsStudyEvent studyEvent = _studyEvents.at(row);

		if(studyEvent.eventType == WS_STUDYEVENT_ANALYSIS)
		{
			bool newState =	!studyEvent.pauseEnabled;
			_studyEvents.at(row).pauseEnabled = newState; //atualiza vetor de eventos

			if(newState)
				eventsTbl->item(row,0)->setIcon(QIcon(":/global/icons/ok.png"));
			else
				eventsTbl->item(row,0)->setIcon(QIcon());
		}	
	}
}

void StudyDialog::fillStudyEventsTbl()
{
	_studyEvents.clear();

	QDate initialDate = initialTimestampTed->date();
	QTime initialTime = initialTimestampTed->time();
	QDate finalDate = finalTimestampTed->date();
	QTime finalTime = finalTimestampTed->time();

	int tolerance = (toleranceCbx->isChecked() ? toleranceLed->text().toInt() : 0);

	_manager->getStudyEvents(initialDate.year(),
							 initialDate.month(),
							 initialDate.day(),
							 initialTime.hour(),
							 initialTime.minute(),
							 initialTime.second(),
							 finalDate.year(),
							 finalDate.month(),
							 finalDate.day(),
							 finalTime.hour(),
							 finalTime.minute(),
							 finalTime.second(),
							 tolerance,
							 _studyEvents);

	wsStudyEvent studyEvent;
	
	int rows = _studyEvents.size();
	eventsTbl->setRowCount(rows);

	for(int i=0; i<rows; ++i)
	{
		studyEvent = _studyEvents.at(i);
		
		char eventDateStr[11], eventTimeStr[6];
		sprintf(eventDateStr, "%d/%d/%d", studyEvent.day, studyEvent.month, studyEvent.year);
		sprintf(eventTimeStr, "%d:%d", studyEvent.hour, studyEvent.minutes);

		QString eventTimeQStr(eventTimeStr);

		//melhorar a exibicao da hora, adicionando '0' quando necessario
		QStringList eventTimeList = eventTimeQStr.split(":");
		if(eventTimeList.at(0).size() == 1)
			eventTimeQStr.prepend("0");
		if(eventTimeList.at(1).size() == 1)
			eventTimeQStr.insert(eventTimeQStr.indexOf(":") + 1, "0");

		QTableWidgetItem* itemPause = new QTableWidgetItem(); //inserir item para possibilitar a indiccao de que uma analise pode pausar o estudo
		QTableWidgetItem* itemDate = new QTableWidgetItem(eventDateStr);
		QTableWidgetItem* itemTime = new QTableWidgetItem(eventTimeQStr);
		QTableWidgetItem* itemDescription = new QTableWidgetItem(studyEvent.eventDescription.c_str());
		QTableWidgetItem* itemLayerName = new QTableWidgetItem(studyEvent.layerName.c_str());

		itemDate->setTextAlignment(Qt::AlignCenter);
		itemTime->setTextAlignment(Qt::AlignCenter);
		itemDescription->setTextAlignment(Qt::AlignCenter);
		itemLayerName->setTextAlignment(Qt::AlignCenter);

		eventsTbl->setItem(i, 0, itemPause);
		eventsTbl->setItem(i, 1, itemDate);
		eventsTbl->setItem(i, 2, itemTime);
		eventsTbl->setItem(i, 3, itemDescription);
		eventsTbl->setItem(i, 4, itemLayerName);
	}
}

void StudyDialog::enableExecutionControlButtons(bool enable)
{
	executeAllRdb->setEnabled(enable);
	pauseAllRdb->setEnabled(enable);
	warningLevelRdb->setEnabled(enable);
	warningLevelCmb->setEnabled(enable);
	tableRdb->setEnabled(enable);
}

void StudyDialog::playClicked()
{
	_pauseRequested = false;
	playBtn->setEnabled(false);
	pauseBtn->setEnabled(true);

	enableExecutionControlButtons(false);

	executeNextAnalysisEvent();
}

void StudyDialog::pauseClicked()
{
	_pauseRequested = true;
	pauseBtn->setEnabled(false);
}

void StudyDialog::executeNextAnalysisEvent()
{
	wsStudyEvent studyEvent;

	int oldAnalysisEvent = _currentAnalysisEvent;
	while(++_currentAnalysisEvent)
	{
		if(_currentAnalysisEvent >= _studyEvents.size()) //Nao ha mais eventos para este estudo
		{
			QMessageBox::information(this, "Resultado", "Não há mais eventos para serem executados neste estudo.", QMessageBox::Ok);
			playBtn->hide();
			pauseBtn->hide();
			_imageAnalysisDlg->close();
			return;
		}

		studyEvent = _studyEvents.at(_currentAnalysisEvent);
		if(studyEvent.eventType == WS_STUDYEVENT_ANALYSIS)
		{
			//Deve ser solicitada uma pausa na execucao dos eventos dependendo do tipo de controle de execucao escolhido
			if( pauseAllRdb->isChecked() || (studyEvent.pauseEnabled && tableRdb->isChecked()) ) 
				pauseClicked();
			break;
		}
	}

	showNextAnalysisEvent(oldAnalysisEvent);

	//Executa as analises disponiveis em um determinado timestamp.
	//Só serao consideradas as coletas realizadas ate este timestamp.
	_manager->runAllAnalyses(false,
							 studyEvent.year,
							 studyEvent.month,
							 studyEvent.day,
							 studyEvent.hour,
							 studyEvent.minutes,
							 studyEvent.seconds);

	//Informa para a janela de visualizacao dos alertas a nova hora considerada
	_imageAnalysisDlg->setTimestamp(studyEvent.year,
									studyEvent.month,
									studyEvent.day,
									studyEvent.hour,
									studyEvent.minutes,
									studyEvent.seconds);

	_analysisTimeoutTimer.setSingleShot(false);
	_analysisTimeoutTimer.start(3000);
}

void StudyDialog::showNextAnalysisEvent(int oldAnalysisEvent)
{
	for(int i=0; i<eventsTbl->columnCount(); ++i)
	{
		eventsTbl->item(oldAnalysisEvent,i)->setBackground(QBrush(QColor(255,255,255)));
		eventsTbl->item(_currentAnalysisEvent, i)->setBackground(QBrush(QColor(255,150,100)));
	}

	QTableWidgetItem* item = eventsTbl->item(_currentAnalysisEvent, 0);
	if(item)
		eventsTbl->scrollToItem(item);
}

void StudyDialog::timeoutSlot()
{
	bool thereIsAnalysisRunning;
	_manager->getAnalysisRunning(thereIsAnalysisRunning);

	if(!thereIsAnalysisRunning) 
	{
		_analysisTimeoutTimer.stop();

		if(warningLevelRdb->isChecked())
		{
			//Testar se o maior alerta retornado pelo ultimo evento de analises ira gerar uma pausa no estudo
			int warning;
			_manager->getLastStudyAnalysesMaxWarning(warning);

			if(warning >= warningLevelCmb->currentIndex())
				pauseClicked();
		}

		//atualizar janela de exibição do jpg
		_imageAnalysisDlg->reloadAnalysisListAndImage();
		_imageAnalysisDlg->show();
		_imageAnalysisDlg->update();

		if(!_pauseRequested)
			executeNextAnalysisEvent();
		else
		{
			enableExecutionControlButtons(true);
			playBtn->setEnabled(true);
		}
	}
}