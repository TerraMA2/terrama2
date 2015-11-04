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
#include "../../core/Filter.hpp"

// Terralib
#include <terralib/geometry/Polygon.h>
#include <terralib/geometry/LinearRing.h>

#include <terralib/datatype/Date.h>

// boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"

#include <QDialog>
#include <QIcon>


#include <iostream>

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

  // loading pixmap icon from theme and set it to label
  QPixmap pixmap = QIcon::fromTheme("filter-big").pixmap(80);
  pimpl_->ui_->labelFilterIcon->setPixmap(pixmap);

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

void FilterDialog::fillGUI(const terrama2::core::Filter& filter)
{
  // If there some geometry at filter, fill field limits
  if (filter.geometry() != nullptr)
  {
    pimpl_->ui_->areaRdb->setChecked(true);
    pimpl_->ui_->areaRdb->clicked();
  }

  //TODO: improve date convertion. This code just uses boost to convert te::Date
  if (filter.discardBefore())
  {
    const te::dt::Date* dt = static_cast<const te::dt::Date*>(filter.discardBefore());
    pimpl_->ui_->dateBeforeFilterCbx->setChecked(true);
    QDate date = QDate::fromString(boost::gregorian::to_iso_string(dt->getDate()).c_str(), "yyyyMMdd");
    pimpl_->ui_->dateBeforeFilterDed->setDate(date);
  }

  if (filter.discardAfter())
  {
    const te::dt::Date* dt = static_cast<const te::dt::Date*>(filter.discardAfter());
    pimpl_->ui_->dateAfterFilterCbx->setChecked(true);
    QDate date = QDate::fromString(boost::gregorian::to_iso_string(dt->getDate()).c_str(), "yyyyMMdd");
    pimpl_->ui_->dateAfterFilterDed->setDate(date);
  }
}

void FilterDialog::fillObject(terrama2::core::Filter &filter)
{
  if (pimpl_->filterByArea_)
  {
    std::unique_ptr<te::gm::Polygon> polygon(new te::gm::Polygon(0, te::gm::PolygonType));

    double minX = pimpl_->ui_->xMinLed->text().toDouble();
    double minY = pimpl_->ui_->yMinLed->text().toDouble();
    double maxX = pimpl_->ui_->xMaxLed->text().toDouble();
    double maxY = pimpl_->ui_->yMaxLed->text().toDouble();

    te::gm::LinearRing* square = new te::gm::LinearRing(5, te::gm::LineStringType);

    square->setPoint(0, minX, minY); // lower left
    square->setPoint(1, minX, maxY); // upper left
    square->setPoint(2, maxX, maxY); // upper right
    square->setPoint(3, maxX, minY); // lower right

    polygon->push_back(square);

    filter.setGeometry(std::move(polygon));
  }

  // TODO: is there another way fill up with before/after date?.
  if (pimpl_->filterByDate_)
  {
    if (pimpl_->ui_->dateBeforeFilterCbx->isChecked())
    {
      QDate beforeDate = pimpl_->ui_->dateBeforeFilterDed->date();
      std::unique_ptr<te::dt::Date> datePtr (new te::dt::Date(
                                          beforeDate.year(), beforeDate.month(), beforeDate.day()));
      filter.setDiscardBefore(std::move(datePtr));
    }

    if (pimpl_->ui_->dateAfterFilterCbx->isChecked())
    {
      QDate afterDate = pimpl_->ui_->dateAfterFilterDed->date();
      std::unique_ptr<te::dt::Date> datePtr (new te::dt::Date(
                                          afterDate.year(), afterDate.month(), afterDate.day()));
      filter.setDiscardAfter(std::move(datePtr));
    }

  }

  // TODO: filter by layer
  if (pimpl_->filterByLayer_)
  {

  }

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
    pimpl_->ui_->filterWidgetStack->setCurrentWidget(pimpl_->ui_->page);
    pimpl_->filterByArea_ = true;
  }
  else
  {
    pimpl_->ui_->filterWidgetStack->setCurrentWidget(pimpl_->ui_->page_2);
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
