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

  \brief Manages the filters typed by users

  \author Raphael Willian da Costa  
*/

// TerraMA2
#include "FilterDialog.hpp"

#include <QDialog>

struct FilterDialog::Impl
{
  Impl()
    : ui_(new Ui::FilterDialogForm),
      filterByDate_(false),
      filterByLayer_(false),
      filterByArea_(false),
      filterBypreAnalyse_(false)
  {
  }

  ~Impl()
  {
  }

  Ui::FilterDialogForm* ui_;
  bool filterByDate_;
  bool filterByLayer_;
  bool filterByArea_;
  bool filterBypreAnalyse_;
};

//! Construtor
FilterDialog::FilterDialog(FilterType type, QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), this, SLOT(accept()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

  // TODO: improve this and check with service???
  connect(pimpl_->ui_->dateBeforeFilterCbx, SIGNAL(clicked()), this, SLOT(onFilteredByDate()));
  connect(pimpl_->ui_->dateAfterFilterCbx, SIGNAL(clicked()), this, SLOT(onFilteredByDate()));
  connect(pimpl_->ui_->bandFilterLed, SIGNAL(textEdited(QString)), this, SLOT(onFilteredByLayer()));
  connect(pimpl_->ui_->beforeTodayBtn, SIGNAL(clicked()), this, SLOT(onBeforeBtnClicked()));
  connect(pimpl_->ui_->afterTodayBtn, SIGNAL(clicked()), this, SLOT(onAfterBtnClicked()));

  // FilterByArea
  connect(pimpl_->ui_->noAreaFilterRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));
  connect(pimpl_->ui_->areaRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));
  connect(pimpl_->ui_->planeRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));

  switch(type)
  {
    case DATE:
      pimpl_->ui_->tabWidget->setTabEnabled(1,false);
      pimpl_->ui_->tabWidget->setTabEnabled(2,false);
      pimpl_->ui_->tabWidget->setTabEnabled(3,false);
      pimpl_->ui_->tabWidget->setTabEnabled(4,false);
      break;
    case BAND:
      pimpl_->ui_->tabWidget->setTabEnabled(0,false); // date
      pimpl_->ui_->tabWidget->setTabEnabled(2,false); // pre
      pimpl_->ui_->tabWidget->setTabEnabled(3,false); // band
      pimpl_->ui_->tabWidget->setTabEnabled(4,false); // dummy
      break;
    default:
      ;
  }
}

FilterDialog::~FilterDialog()
{
  delete pimpl_;
}

bool FilterDialog::isFilterByDate() const
{
  return pimpl_->filterByDate_;
}

bool FilterDialog::isFilterByArea() const
{
  return pimpl_->filterByArea_;
}

bool FilterDialog::isAnyFilter() const
{
  return isFilterByArea() || isFilterByDate() || isFilterByLayer() || isFilterByPreAnalyse();
}

bool FilterDialog::isFilterByLayer() const
{
  return pimpl_->filterByLayer_;
}

bool FilterDialog::isFilterByPreAnalyse() const
{
  return pimpl_->filterBypreAnalyse_;
}

void FilterDialog::onFilteredByDate()
{
  pimpl_->filterByDate_ = pimpl_->ui_->dateBeforeFilterCbx->isChecked() || pimpl_->ui_->dateAfterFilterCbx->isChecked();
}

void FilterDialog::onFilteredByLayer()
{
  pimpl_->filterByLayer_ = !pimpl_->ui_->bandFilterLed->text().trimmed().isEmpty();
}

void FilterDialog::onFilteredByArea()
{
  // TODO: validate the filter limits
  if (pimpl_->ui_->noAreaFilterRdb->isChecked())
  {
    pimpl_->ui_->filterWidgetStack->setCurrentWidget(pimpl_->ui_->page_3); 
    pimpl_->filterByArea_ = false;
  }
  else if (pimpl_->ui_->areaRdb->isChecked())
  {
    pimpl_->ui_->filterWidgetStack->setCurrentWidget(pimpl_->ui_->page_2);
    pimpl_->filterByArea_ = true;
  }
  else
  {
    pimpl_->ui_->filterWidgetStack->setCurrentWidget(pimpl_->ui_->page);
    pimpl_->filterByArea_ = true;
  }
}

void FilterDialog::onBeforeBtnClicked()
{
  pimpl_->ui_->dateBeforeFilterDed->setDate(QDate::currentDate());
}

void FilterDialog::onAfterBtnClicked()
{
  pimpl_->ui_->dateAfterFilterDed->setDate(QDate::currentDate());
}
