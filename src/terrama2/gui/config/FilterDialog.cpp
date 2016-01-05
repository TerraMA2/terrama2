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

#include "../../collector/Utils.hpp"

// Terralib
#include <terralib/geometry/Point.h>
#include <terralib/geometry/Polygon.h>
#include <terralib/geometry/LinearRing.h>

#include <terralib/datatype/TimeInstantTZ.h>

// boost
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <QIcon>
#include <QDialog>
#include <QLineEdit>
#include <QTimeZone>
#include <QDateTime>
#include <QMessageBox>
#include <QIntValidator>


struct terrama2::gui::config::FilterDialog::Impl
{
  Impl()
    : ui_(new Ui::FilterDialogForm),
      filterByDate_(false),
      filterByLayer_(false),
      filterByArea_(false),
      filterBypreAnalyse_(false),
      expressionType(terrama2::core::Filter::NONE_TYPE),
      utc_()
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
  terrama2::core::Filter::ExpressionType expressionType;
  QString utc_;
};

//! Construtor
terrama2::gui::config::FilterDialog::FilterDialog(FilterType type, const QString& timezone, QWidget* parent, Qt::WindowFlags flag)
  : QDialog(parent, flag), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  pimpl_->utc_ = timezone.toStdString().c_str();

  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), this, SLOT(onOkBtnClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

  connect(pimpl_->ui_->dateBeforeFilterCbx, SIGNAL(clicked()), this, SLOT(onFilteredByDate()));
  connect(pimpl_->ui_->dateAfterFilterCbx, SIGNAL(clicked()), this, SLOT(onFilteredByDate()));
  connect(pimpl_->ui_->bandFilterLed, SIGNAL(textEdited(QString)), this, SLOT(onFilteredByLayer()));
  connect(pimpl_->ui_->beforeTodayBtn, SIGNAL(clicked()), this, SLOT(onBeforeBtnClicked()));
  connect(pimpl_->ui_->afterTodayBtn, SIGNAL(clicked()), this, SLOT(onAfterBtnClicked()));

  // FilterByArea
  connect(pimpl_->ui_->noAreaFilterRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));
  connect(pimpl_->ui_->areaRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));
  connect(pimpl_->ui_->planeRdb, SIGNAL(clicked()), this, SLOT(onFilteredByArea()));

  // pre-filter
  connect(pimpl_->ui_->noPreAnalysisRdb, SIGNAL(clicked()), this, SLOT(onNoPreAnalyse()));
  connect(pimpl_->ui_->allSmallerThanRdb, SIGNAL(clicked()), this, SLOT(onFilterByLessThan()));
  connect(pimpl_->ui_->allLargerThanRdb, SIGNAL(clicked()), this, SLOT(onFilterByGreaterThan()));
  connect(pimpl_->ui_->belowAverageRdb, SIGNAL(clicked()), this, SLOT(onFilterByMeanLessThan()));
  connect(pimpl_->ui_->aboveAverageRdb, SIGNAL(clicked()), this, SLOT(onFilterByMeanGreaterThan()));

  // loading pixmap icon from theme and set it to label
  QPixmap pixmap = QIcon::fromTheme("filter-big").pixmap(80);
  pimpl_->ui_->labelFilterIcon->setPixmap(pixmap);

  switch(type)
  {
    case DATE:
      pimpl_->ui_->tabWidget->setTabEnabled(2,false);
      pimpl_->ui_->tabWidget->setTabEnabled(3,false);
      pimpl_->ui_->tabWidget->setTabEnabled(4,false);
      break;
    case BAND:
      pimpl_->ui_->bandTab->setEnabled(true);
      pimpl_->ui_->tabWidget->setTabEnabled(0,false); // date
      pimpl_->ui_->tabWidget->setTabEnabled(1,false); // pre
      pimpl_->ui_->tabWidget->setTabEnabled(2,false); // band
      pimpl_->ui_->tabWidget->setTabEnabled(4,false); // dummy
      break;
    case AREA:
      pimpl_->ui_->tabWidget->setTabEnabled(0,false);
      pimpl_->ui_->tabWidget->setTabEnabled(2,false);
      pimpl_->ui_->tabWidget->setTabEnabled(3,false);
      pimpl_->ui_->tabWidget->setTabEnabled(4,false);
      break;
    default:
      ;
  }

  pimpl_->ui_->planeRdb->setEnabled(false);
  pimpl_->ui_->preAnalysisThemeBox->setEnabled(false);
  pimpl_->ui_->dummyTab->setEnabled(false);

  pimpl_->ui_->allSmallerThanLed->setValidator(new QIntValidator(pimpl_->ui_->allSmallerThanLed));
  pimpl_->ui_->allLargerThanLed->setValidator(new QIntValidator(pimpl_->ui_->allLargerThanLed));
  pimpl_->ui_->belowAverageLed->setValidator(new QIntValidator(pimpl_->ui_->belowAverageLed));
  pimpl_->ui_->aboveAverageLed->setValidator(new QIntValidator(pimpl_->ui_->aboveAverageLed));

  std::string utc = "(UTC" + timezone.toStdString() + ")";
  pimpl_->ui_->labelDateNote->setText(utc.c_str());
}

terrama2::gui::config::FilterDialog::~FilterDialog()
{
  delete pimpl_;
}

bool terrama2::gui::config::FilterDialog::isFilterByDate() const
{
  return pimpl_->filterByDate_;
}

bool terrama2::gui::config::FilterDialog::isFilterByArea() const
{
  return pimpl_->filterByArea_;
}

bool terrama2::gui::config::FilterDialog::isAnyFilter() const
{
  return isFilterByArea() || isFilterByDate() || isFilterByLayer() || isFilterByPreAnalyse();
}

void terrama2::gui::config::FilterDialog::fillGUI(const terrama2::core::Filter& filter)
{
  // If there some geometry at filter, fill field limits
  if (filter.geometry() != nullptr)
  {
    pimpl_->ui_->areaRdb->setChecked(true);
    pimpl_->ui_->areaRdb->clicked();

    const te::gm::Polygon* geom = dynamic_cast<const te::gm::Polygon*>(filter.geometry());
    const te::gm::LinearRing* square = dynamic_cast<const te::gm::LinearRing*>(geom->getRingN(0));

    pimpl_->ui_->xMinLed->setText(QString::number(square->getPointN(0)->getX()));
    pimpl_->ui_->yMinLed->setText(QString::number(square->getPointN(0)->getY()));
    pimpl_->ui_->xMaxLed->setText(QString::number(square->getPointN(2)->getX()));
    pimpl_->ui_->yMaxLed->setText(QString::number(square->getPointN(2)->getY()));
  }

  if (filter.discardBefore())
  {
    const te::dt::TimeInstantTZ* dt = filter.discardBefore();

    std::string date_string;
    std::string time_string;
    std::string timezone_string;

    terrama2::collector::BoostLocalDateTime2DateTimeString(dt->getTimeInstantTZ(), date_string, time_string, timezone_string);

    pimpl_->ui_->dateBeforeFilterCbx->setChecked(true);

    QString format= "%1 %2";
    QString formated_datetime = format.arg(date_string.c_str()).arg(time_string.c_str());
    QDateTime dateTime = QDateTime::fromString(formated_datetime, "yyyy-MM-dd HH:mm:ss");

    pimpl_->ui_->datetimeBefore->setDateTime(dateTime);
    emit pimpl_->ui_->dateBeforeFilterCbx->clicked();
  }

  if (filter.discardAfter())
  {
    const te::dt::TimeInstantTZ* dt = filter.discardAfter();

    std::string date_string;
    std::string time_string;
    std::string timezone_string;

    terrama2::collector::BoostLocalDateTime2DateTimeString(dt->getTimeInstantTZ(), date_string, time_string, timezone_string);

    pimpl_->ui_->dateAfterFilterCbx->setChecked(true);

    QString format= "%1 %2";
    QString formated_datetime = format.arg(date_string.c_str()).arg(time_string.c_str());
    QDateTime dateTime = QDateTime::fromString(formated_datetime, "yyyy-MM-dd HH:mm:ss");

    pimpl_->ui_->datetimeAfter->setDateTime(dateTime);
    emit pimpl_->ui_->dateAfterFilterCbx->clicked();
  }

  if (!filter.bandFilter().empty())
  {
    pimpl_->ui_->bandFilterLed->setText(filter.bandFilter().c_str());
    pimpl_->filterByLayer_ = true;
  }

  if (filter.value() != nullptr)
  {
    pimpl_->expressionType = filter.expressionType();
    switch(pimpl_->expressionType)
    {
      case terrama2::core::Filter::LESS_THAN_TYPE:
        pimpl_->ui_->allSmallerThanRdb->setChecked(true);
        pimpl_->ui_->allSmallerThanLed->setText(QString::number(*filter.value()));
        break;
      case terrama2::core::Filter::GREATER_THAN_TYPE:
        pimpl_->ui_->allLargerThanRdb->setChecked(true);
        pimpl_->ui_->allLargerThanLed->setText(QString::number(*filter.value()));
        break;
      case terrama2::core::Filter::MEAN_LESS_THAN_TYPE:
        pimpl_->ui_->belowAverageRdb->setChecked(true);
        pimpl_->ui_->belowAverageLed->setText(QString::number(*filter.value()));
        break;
      case terrama2::core::Filter::MEAN_GREATER_THAN_TYPE:
        pimpl_->ui_->aboveAverageRdb->setChecked(true);
        pimpl_->ui_->aboveAverageLed->setText(QString::number(*filter.value()));
        break;
      default:
        pimpl_->ui_->noPreAnalysisRdb->setChecked(true);
    }
  }
}

void terrama2::gui::config::FilterDialog::fillObject(terrama2::core::Filter &filter)
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
    square->setPoint(4, minX, minY); // lower right

    polygon->push_back(square);

    filter.setGeometry(std::move(polygon));
  }
  else
    filter.setGeometry(nullptr);

  if (pimpl_->filterByDate_)
  {
    if (pimpl_->ui_->dateBeforeFilterCbx->isChecked())
    {
      QDateTime beforeDate = pimpl_->ui_->datetimeBefore->dateTime();

      auto boostLocalTime = terrama2::collector::QDateTime2BoostLocalDateTime(beforeDate, pimpl_->utc_);

      //Build a te::dt::TimeInstantTZ
      std::unique_ptr<te::dt::TimeInstantTZ> datePtr (new te::dt::TimeInstantTZ(boostLocalTime));
      filter.setDiscardBefore(std::move(datePtr));
    }
    else
      filter.setDiscardBefore(nullptr);

    if (pimpl_->ui_->dateAfterFilterCbx->isChecked())
    {
      QDateTime afterDate = pimpl_->ui_->datetimeAfter->dateTime();

      auto boostLocalTime = terrama2::collector::QDateTime2BoostLocalDateTime(afterDate, pimpl_->utc_);

      //Build a te::dt::TimeInstantTZ
      std::unique_ptr<te::dt::TimeInstantTZ> datePtr (new te::dt::TimeInstantTZ(boostLocalTime));
      filter.setDiscardAfter(std::move(datePtr));
    }
    else
      filter.setDiscardAfter(nullptr);
  }
  else
  {
    filter.setDiscardBefore(nullptr);
    filter.setDiscardAfter(nullptr);
  }

  if (pimpl_->filterByLayer_)
  {
    filter.setBandFilter(pimpl_->ui_->bandFilterLed->text().toStdString());
  }
  else
    filter.setBandFilter("");

  if (pimpl_->filterBypreAnalyse_)
  {
    filter.setExpressionType(pimpl_->expressionType);
    double* value;
    switch(pimpl_->expressionType)
    {
      case terrama2::core::Filter::LESS_THAN_TYPE:
        value = new double(pimpl_->ui_->allSmallerThanLed->text().toDouble());
        break;
      case terrama2::core::Filter::GREATER_THAN_TYPE:
        value = new double(pimpl_->ui_->allLargerThanLed->text().toDouble());
        break;
      case terrama2::core::Filter::MEAN_LESS_THAN_TYPE:
        value = new double(pimpl_->ui_->belowAverageLed->text().toDouble());
        break;
      case terrama2::core::Filter::MEAN_GREATER_THAN_TYPE:
        value = new double(pimpl_->ui_->aboveAverageLed->text().toDouble());
        break;
      default:
        ;
    }

    filter.setValue(std::unique_ptr<double>(value));
  }
  else
    filter.setValue(nullptr);

}

bool terrama2::gui::config::FilterDialog::isFilterByLayer() const
{
  return pimpl_->filterByLayer_;
}

bool terrama2::gui::config::FilterDialog::isFilterByPreAnalyse() const
{
  return pimpl_->filterBypreAnalyse_;
}

void terrama2::gui::config::FilterDialog::onFilteredByDate()
{
  pimpl_->filterByDate_ = pimpl_->ui_->dateBeforeFilterCbx->isChecked() || pimpl_->ui_->dateAfterFilterCbx->isChecked();
}

void terrama2::gui::config::FilterDialog::onFilteredByLayer()
{
  pimpl_->filterByLayer_ = !pimpl_->ui_->bandFilterLed->text().trimmed().isEmpty();
}

void terrama2::gui::config::FilterDialog::onFilteredByArea()
{
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

void terrama2::gui::config::FilterDialog::onBeforeBtnClicked()
{
  pimpl_->ui_->datetimeBefore->setDateTime(QDateTime::currentDateTime());
  pimpl_->ui_->dateBeforeFilterCbx->setChecked(true);
  emit pimpl_->ui_->dateBeforeFilterCbx->clicked();
}

void terrama2::gui::config::FilterDialog::onNoPreAnalyse()
{
  pimpl_->expressionType = terrama2::core::Filter::NONE_TYPE;
  setFilterByPreAnalyse();
}

void terrama2::gui::config::FilterDialog::onFilterByLessThan()
{
  pimpl_->expressionType = terrama2::core::Filter::LESS_THAN_TYPE;
  setFilterByPreAnalyse();
  pimpl_->ui_->allSmallerThanLed->setEnabled(true);
}

void terrama2::gui::config::FilterDialog::onFilterByGreaterThan()
{
  pimpl_->expressionType = terrama2::core::Filter::GREATER_THAN_TYPE;
  setFilterByPreAnalyse();
  pimpl_->ui_->allLargerThanLed->setEnabled(true);
}

void terrama2::gui::config::FilterDialog::onFilterByMeanLessThan()
{
  pimpl_->expressionType = terrama2::core::Filter::MEAN_LESS_THAN_TYPE;
  setFilterByPreAnalyse();
  pimpl_->ui_->belowAverageLed->setEnabled(true);
}

void terrama2::gui::config::FilterDialog::onFilterByMeanGreaterThan()
{
  pimpl_->expressionType = terrama2::core::Filter::MEAN_GREATER_THAN_TYPE;
  setFilterByPreAnalyse();
  pimpl_->ui_->aboveAverageLed->setEnabled(true);
}

void terrama2::gui::config::FilterDialog::onOkBtnClicked()
{
  if (pimpl_->ui_->dateBeforeFilterCbx->isChecked() && pimpl_->ui_->dateAfterFilterCbx->isChecked())
  {
    if (pimpl_->ui_->datetimeAfter->dateTime() > pimpl_->ui_->datetimeBefore->dateTime())
      accept();
    else
    {
      QMessageBox::warning(this, tr("TerraMa2"), tr("The after date should be equal or greater than before date"));
      return;
    }
  }
  accept();
}

void terrama2::gui::config::FilterDialog::setFilterByPreAnalyse()
{
  disablePreFields();
  pimpl_->filterBypreAnalyse_ = true;
}

void terrama2::gui::config::FilterDialog::disablePreFields()
{
  for(QLineEdit* widget: pimpl_->ui_->preTab->findChildren<QLineEdit*>())
    widget->setEnabled(false);
}

void terrama2::gui::config::FilterDialog::onAfterBtnClicked()
{
  pimpl_->ui_->datetimeAfter->setDateTime(QDateTime::currentDateTime());

  pimpl_->ui_->dateAfterFilterCbx->setChecked(true);
  emit pimpl_->ui_->dateAfterFilterCbx->clicked();
}
