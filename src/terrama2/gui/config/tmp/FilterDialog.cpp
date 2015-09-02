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
  \file terrama2/gui/config/FilterDialog.cpp

  \brief Definition of Class FilterDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// STL
#include <assert.h>

// TerraMA2
#include "FilterDialog.hpp"


//! Construtor
FilterDialog::FilterDialog(FilterDialogType fType, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _filterChanged = false;
  _ignoreChangeEvents = false;

  connect(areaRdb, SIGNAL(clicked()), SLOT(filterTypeToggled()));
  connect(planeRdb, SIGNAL(clicked()), SLOT(filterTypeToggled()));
  connect(noAreaFilterRdb, SIGNAL(clicked()), SLOT(filterTypeToggled()));
  connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(filterTypeToggled()));

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  connect(dateBeforeFilterCbx, SIGNAL(stateChanged(int)), SLOT(setFilterChanged()));
  connect(dateAfterFilterCbx, SIGNAL(stateChanged(int)), SLOT(setFilterChanged()));
  connect(dateBeforeFilterDed, SIGNAL(dateChanged(const QDate&)), SLOT(setFilterChanged()));
  connect(dateAfterFilterDed, SIGNAL(dateChanged(const QDate&)), SLOT(setFilterChanged()));
  
  connect(noAreaFilterRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(areaRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(planeRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(xMinLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(xMaxLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(yMinLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(yMaxLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(themeListWidget, SIGNAL(currentRowChanged(int)), SLOT(setFilterChanged()));

  connect(noPreAnalysisRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(allSmallerThanRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(allLargerThanRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(belowAverageRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(aboveAverageRdb, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  
  connect(allSmallerThanLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(allLargerThanLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(belowAverageLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));
  connect(aboveAverageLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));

  connect(noPreAnalysisRdb, SIGNAL(clicked(bool)), SLOT(disablePAThemeBox()));
  connect(allSmallerThanRdb, SIGNAL(clicked(bool)), SLOT(enablePAThemeBox()));
  connect(allLargerThanRdb, SIGNAL(clicked(bool)), SLOT(enablePAThemeBox()));
  connect(belowAverageRdb, SIGNAL(clicked(bool)), SLOT(enablePAThemeBox()));
  connect(aboveAverageRdb, SIGNAL(clicked(bool)), SLOT(enablePAThemeBox()));

  connect(preAnalysisThemeBox, SIGNAL(clicked(bool)), SLOT(setFilterChanged()));
  connect(preAnalysisThemeList, SIGNAL(currentRowChanged(int)), SLOT(setFilterChanged()));

  connect(bandFilterLed, SIGNAL(textChanged(const QString&)), SLOT(setFilterChanged()));

  connect(newDummyChk, SIGNAL(stateChanged(int)), SLOT(setFilterChanged()));
  connect(dummyValueSpb, SIGNAL(valueChanged(double)), SLOT(setFilterChanged()));

  connect(beforeTodayBtn, SIGNAL(clicked()), SLOT(setToday()));
  connect(afterTodayBtn, SIGNAL(clicked()), SLOT(setToday()));

  // Define os tipos para validação (filtro)
  xMinLed->setValidator(new QDoubleValidator(xMinLed));
  xMaxLed->setValidator(new QDoubleValidator(xMaxLed));
  yMinLed->setValidator(new QDoubleValidator(yMinLed));
  yMaxLed->setValidator(new QDoubleValidator(yMaxLed));

  allSmallerThanLed->setValidator(new QDoubleValidator(allSmallerThanLed));
  allLargerThanLed->setValidator(new QDoubleValidator(allLargerThanLed));
  belowAverageLed->setValidator(new QDoubleValidator(belowAverageLed));
  aboveAverageLed->setValidator(new QDoubleValidator(aboveAverageLed));

  QRegExp regExp("^[0-9]+(-[0-9]+)?(,[0-9]+(-[0-9]+)?)*$");
  bandFilterLed->setValidator(new QRegExpValidator(regExp, bandFilterLed));

  if (fType == DATEFILTER)
  {
	  tabWidget->setTabEnabled(1,false); // area
	  tabWidget->setTabEnabled(2,false); // pre
	  tabWidget->setTabEnabled(3,false); // band
	  tabWidget->setTabEnabled(4,false); // dummy
  }
  else if (fType == AREAFILTER)
  {
	  tabWidget->setTabEnabled(0,false); // date
	  tabWidget->setTabEnabled(2,false); // pre
	  tabWidget->setTabEnabled(3,false); // band
	  tabWidget->setTabEnabled(4,false); // dummy
  }
  else if (fType == NO_BANDFILTER)
  {
	  tabWidget->setTabEnabled(3,false); // band
  }

  preAnalysisThemeBox->setDisabled(true);
  okBtn->setEnabled(false);
}

//! Destrutor
FilterDialog::~FilterDialog()
{
}

void FilterDialog::fillThemeList(std::vector<struct wsTheme> themes)
{
	for(int i=0,size=themes.size(); i<size; ++i)
	{
		wsTheme theme = themes.at(i);
		QListWidgetItem* item = new QListWidgetItem(QString((theme.name).c_str()));
		item->setData(Qt::UserRole, theme.id);
		themeListWidget->addItem(item);

		QListWidgetItem* preAnalysisListItem = new QListWidgetItem(QString((theme.name).c_str()));
		preAnalysisListItem->setData(Qt::UserRole, theme.id);
		preAnalysisThemeList->addItem(preAnalysisListItem);
	}
}

void FilterDialog::filterTypeToggled()
{
  int index = 0;
  if(areaRdb->isChecked())
    index = 1;
  else if(planeRdb->isChecked())
	index = 2;
  filterWidgetStack->setCurrentIndex(index);  
}

void FilterDialog::setFields(const wsWeatherDataSourceFilter& filterParams, std::vector<struct wsTheme> themes)
{
	bool lOldStat = _ignoreChangeEvents;
	_ignoreChangeEvents = true;

	//Preenche as listas contendo os temas disponiveis para filtro por area e pre-analise
	fillThemeList(themes);

	//Filtro por data
	if(filterParams.dateBeforeEnabled)
	{
		dateBeforeFilterCbx->setCheckState(Qt::Checked);
		dateBeforeFilterDed->setDate(QDate(filterParams.dateBeforeYear, 
									   filterParams.dateBeforeMonth, 
									   filterParams.dateBeforeDay));
	}

	if(filterParams.dateAfterEnabled)
	{
		dateAfterFilterCbx->setCheckState(Qt::Checked);
		dateAfterFilterDed->setDate(QDate(filterParams.dateAfterYear,
										 filterParams.dateAfterMonth,
										 filterParams.dateAfterDay));
	}

	//Filtro por area
	if(filterParams.areaFilterType == WS_AreaFilter)
	{
		areaRdb->setChecked(true);
		xMinLed->setText(QString::number(filterParams.xMin, 'f'));
		xMaxLed->setText(QString::number(filterParams.xMax, 'f'));
		yMinLed->setText(QString::number(filterParams.yMin, 'f'));
		yMaxLed->setText(QString::number(filterParams.yMax, 'f'));
	}
	else if(filterParams.areaFilterType == WS_PlaneFilter)
	{
		planeRdb->setChecked(true);
		QListWidgetItem *item;
		int i=0;
		for(; i<themeListWidget->count(); ++i)
		{
			item = themeListWidget->item(i);
			if(item->data(Qt::UserRole).toInt() == filterParams.themeID)
				break;
		}
		themeListWidget->setCurrentRow(i);
	}
	else
		noAreaFilterRdb->setChecked(true);

	//Filtro por pre-analise
	if(filterParams.preAnalysisType == WS_PATYPE_NoPreAnalysis)
		noPreAnalysisRdb->setChecked(true);
	else 
	{
		preAnalysisThemeBox->setDisabled(false);

		if(filterParams.preAnalysisType == WS_PATYPE_AllSmallerThan)
		{
			allSmallerThanRdb->setChecked(true);
			allSmallerThanLed->setText(QString::number(filterParams.preAnalysisValue, 'f'));
		}
		else if(filterParams.preAnalysisType == WS_PATYPE_AllLargerThan)
		{
			allLargerThanRdb->setChecked(true);
			allLargerThanLed->setText(QString::number(filterParams.preAnalysisValue, 'f'));
		}
		else if(filterParams.preAnalysisType == WS_PATYPE_BelowAverage)
		{
			belowAverageRdb->setChecked(true);
			belowAverageLed->setText(QString::number(filterParams.preAnalysisValue, 'f'));
		}
		else if(filterParams.preAnalysisType == WS_PATYPE_AboveAverage)
		{
			aboveAverageRdb->setChecked(true);
			aboveAverageLed->setText(QString::number(filterParams.preAnalysisValue, 'f'));
		}

		if(filterParams.preAnalysisThemeEnabled)
		{
			preAnalysisThemeBox->setChecked(true);

			QListWidgetItem *item;
			int i = 0;
			for(; i<preAnalysisThemeList->count(); ++i)
			{
				item = preAnalysisThemeList->item(i);
				if(item->data(Qt::UserRole).toInt() == filterParams.preAnalysisTheme)
					break;
			}
			preAnalysisThemeList->setCurrentRow(i);
		}
	}

	//Filtro por banda
	bandFilterLed->setText(QString(filterParams.bandFilterString.c_str()));

	dummyValueSpb->setValue(filterParams.newDummy);
	newDummyChk->setCheckState(filterParams.useNewDummy ? Qt::Checked : Qt::Unchecked);

	_ignoreChangeEvents = lOldStat;
}

void FilterDialog::getFields(wsWeatherDataSourceFilter *filterParams, bool& changed)
{
	//Filtro por data
	filterParams->dateBeforeEnabled = dateBeforeFilterCbx->isChecked();
	filterParams->dateBeforeYear = dateBeforeFilterDed->date().year();
	filterParams->dateBeforeMonth = dateBeforeFilterDed->date().month();
	filterParams->dateBeforeDay = dateBeforeFilterDed->date().day();

	filterParams->dateAfterEnabled = dateAfterFilterCbx->isChecked();
	filterParams->dateAfterYear = dateAfterFilterDed->date().year();
	filterParams->dateAfterMonth = dateAfterFilterDed->date().month();
	filterParams->dateAfterDay = dateAfterFilterDed->date().day();

	//Filtro por area
	if(areaRdb->isChecked())
	{
		filterParams->areaFilterType = WS_AreaFilter;
		filterParams->xMin = xMinLed->text().trimmed().toDouble();
		filterParams->xMax = xMaxLed->text().trimmed().toDouble();
		filterParams->yMin = yMinLed->text().trimmed().toDouble();
		filterParams->yMax = yMaxLed->text().trimmed().toDouble();
	}
	else if(planeRdb->isChecked() && themeListWidget->currentRow() >= 0)
	{
		filterParams->areaFilterType = WS_PlaneFilter;
		filterParams->themeID = themeListWidget->currentItem()->data(Qt::UserRole).toInt();
	}
	else
		filterParams->areaFilterType = WS_NoFilter;

	//Filtro por pre-analise
	if(noPreAnalysisRdb->isChecked())
	{
		filterParams->preAnalysisType = WS_PATYPE_NoPreAnalysis;
		filterParams->preAnalysisThemeEnabled = false;
	}
	else 
	{
		//Armazenar o tipo de pre-analise
		if(allSmallerThanRdb->isChecked())
		{
			filterParams->preAnalysisType = WS_PATYPE_AllSmallerThan;
			filterParams->preAnalysisValue = allSmallerThanLed->text().trimmed().toDouble();
		}
		else if(allLargerThanRdb->isChecked())
		{
			filterParams->preAnalysisType = WS_PATYPE_AllLargerThan;
			filterParams->preAnalysisValue = allLargerThanLed->text().trimmed().toDouble();
		}
		else if(belowAverageRdb->isChecked())
		{
			filterParams->preAnalysisType = WS_PATYPE_BelowAverage;
			filterParams->preAnalysisValue = belowAverageLed->text().trimmed().toDouble();
		}
		else if(aboveAverageRdb->isChecked())
		{
			filterParams->preAnalysisType = WS_PATYPE_AboveAverage;
			filterParams->preAnalysisValue = aboveAverageLed->text().trimmed().toDouble();
		}

		//Armazenar o plano definido para a pre-analise, se informado
		if(preAnalysisThemeBox->isChecked() && preAnalysisThemeList->currentRow() >= 0)
		{
			filterParams->preAnalysisThemeEnabled = true;
			filterParams->preAnalysisTheme = preAnalysisThemeList->currentItem()->data(Qt::UserRole).toInt();
		}
		else
			filterParams->preAnalysisThemeEnabled = false;
	}

	//Filtro por banda
	filterParams->bandFilterString = bandFilterLed->text().toStdString();

	filterParams->useNewDummy = (newDummyChk->checkState() == Qt::Checked);
	if (filterParams->useNewDummy)
		filterParams->newDummy = dummyValueSpb->value();
	else
		filterParams->newDummy = -10.;

	changed = _filterChanged;
}

void FilterDialog::setFilterChanged()
{
  if (_ignoreChangeEvents)
	  return;

  _filterChanged = true;
  okBtn->setEnabled(true);
}

void FilterDialog::enablePAThemeBox()
{
	preAnalysisThemeBox->setDisabled(false);
}

void FilterDialog::disablePAThemeBox()
{
	preAnalysisThemeBox->setDisabled(true);
}

void FilterDialog::setToday()
{
	QToolButton* btnSender = (QToolButton*)sender();
	QDate today = QDate::currentDate();

	if(btnSender == beforeTodayBtn)
	{
		dateBeforeFilterDed->setDate(today);
	}

	if (btnSender == afterTodayBtn)
	{
		dateAfterFilterDed->setDate(today);
	}
	
}
