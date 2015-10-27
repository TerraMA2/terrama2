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
  \file terrama2/gui/config/ProjectionDialog.cpp

  \brief Definition of Class ProjectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa    
*/

// TerraMA2
#include "ProjectionDialog.hpp"

// QT
#include <QDialog>


//todo: get it from terralib
const double  TeCDR = 0.01745329251994329576;   //!< Conversion factor: degrees to radians
const double  TeCRD = 57.29577951308232087679;  //!< Conversion factor: radians to degrees

static ProjectionValue valueDef = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static ProjectionValue valueDefUTM = { TeCDR*-39.0, -45.00, 0.0, 0.0, 500000.00, 10000000.000, 0.9996  };

static Projection projections[] = {
  {"NoProjection", 1, false, false, false, false, false, false, false, false, false, false, &valueDef},
  {"Albers", 1, true, false, true, true, true, true, true, true, false, false, &valueDef},
  {"LatLong", 2, true, false, false, false, false, false, false, false, false, false, &valueDef},
  {"LambertConformal", 1, true, false, true, true, true, true, true, true, false, false, &valueDef},
  {"Mercator", 1, true, false, true, true, true, false, true, true, false, false, &valueDef},
  {"Miller", 1, true, false, false, true, false, false, true, true, false, false, &valueDef},
  {"UTM", 1, true, true, false, true, false, false, true,  true, false, true, &valueDefUTM},
  {"Polyconic", 1, true, false, true, true, false, false, true, true, false, false, &valueDef},
  {"Sinusoidal", 1, true, false, false, true, false, false, true, true, false, false, &valueDef},
  {"CylindricalEquidistant", 1, true, false, false, true, true, false, true, true, false, false, &valueDef},
  {"PolarStereographic", 1, true, false, false, true, false, false, true,  true, false, true, &valueDef},
};

struct ProjectionDialog::Impl
{
  Impl()
    : ui_(new Ui::ProjectionDialogForm)
  {

  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::ProjectionDialogForm* ui_;
};


ProjectionDialog::ProjectionDialog(QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f), pimpl_(new Impl)
{
  pimpl_->ui_->setupUi(this);

  QStringList datumList({"Astro Chua", "Corrego Alegre", "Indian", "NAD27", "NAD83", "SAD69", "Spherical", "WGS84", "Aratu"});
  pimpl_->ui_->cboBoxDatum->addItems(datumList);

  for (int i = 1; i <= 60; i++)
    pimpl_->ui_->cboBoxZone->addItem("Zone "+QString::number(i));

  for(auto projection: projections)
    pimpl_->ui_->cboBoxProjection->addItem(projection.name);

  connect(pimpl_->ui_->cboBoxProjection, SIGNAL(currentIndexChanged(int)), SLOT(onProjectionChanged(int)));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(onCancelClicked()));
  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(accept()));

  connect(pimpl_->ui_->cboBoxZone, SIGNAL(activated(int)), SLOT(onComboboxZoneActivated(int)));

  QDoubleValidator* lineEdtLongValidator = new QDoubleValidator(pimpl_->ui_->lineEdtLongOrigem);
  lineEdtLongValidator->setDecimals(3);
  pimpl_->ui_->lineEdtLongOrigem->setValidator(lineEdtLongValidator);

  QDoubleValidator* lineEdtLatValidator = new QDoubleValidator(pimpl_->ui_->lineEdtLongOrigem);
  lineEdtLongValidator->setDecimals(3);
  pimpl_->ui_->lineEdtLatOrigem->setValidator(lineEdtLatValidator);

  QDoubleValidator* lineEdtParaleloPad1Validator = new QDoubleValidator(pimpl_->ui_->lineEdtParaleloPad1);
  lineEdtParaleloPad1Validator->setDecimals(3);
  pimpl_->ui_->lineEdtParaleloPad1->setValidator(lineEdtParaleloPad1Validator);

  QDoubleValidator* lineEdtParaleloPad2Validator = new QDoubleValidator(pimpl_->ui_->lineEdtParaleloPad2);
  lineEdtParaleloPad2Validator->setDecimals(3);
  pimpl_->ui_->lineEdtParaleloPad2->setValidator(lineEdtParaleloPad2Validator);

  QDoubleValidator* lineEdtOffsetXValidator = new QDoubleValidator(pimpl_->ui_->lineEdtOffsetX);
  lineEdtParaleloPad2Validator->setDecimals(3);
  pimpl_->ui_->lineEdtOffsetX->setValidator(lineEdtOffsetXValidator);

  QDoubleValidator* lineEdtOffsetYValidator = new QDoubleValidator(pimpl_->ui_->lineEdtOffsetY);
  lineEdtOffsetYValidator->setDecimals(3);
  pimpl_->ui_->lineEdtOffsetY->setValidator(lineEdtOffsetYValidator);

  QDoubleValidator* lineEdtEscalaValidator = new QDoubleValidator(pimpl_->ui_->lineEdtEscala);
  lineEdtEscalaValidator->setDecimals(3);
  pimpl_->ui_->lineEdtEscala->setValidator(lineEdtEscalaValidator);

  pimpl_->ui_->cboBoxProjection->setCurrentIndex(0);

}

ProjectionDialog::~ProjectionDialog()
{
  delete pimpl_;
}

void ProjectionDialog::setupProjection(int index)
{
  pimpl_->ui_->cboBoxUnits->setCurrentIndex(projections[index].unit -1);
  pimpl_->ui_->cboBoxUnits->setEnabled(false);

  pimpl_->ui_->cboBoxZone->setEnabled(projections[index].hasZone);
  if (projections[index].name == "UTM")
  {
    pimpl_->ui_->cboBoxZone->setCurrentIndex(22);
    onComboboxZoneActivated(22);
  }
  else
    setEditValues(pimpl_->ui_->lineEdtLongOrigem, projections[index].hasOriginLong, projections[index].valueDef->valdefOriginLong*TeCRD);

  setEditValues(pimpl_->ui_->lineEdtLatOrigem, projections[index].hasOriginLat, projections[index].valueDef->valdefOriginLat*TeCRD);
  setEditValues(pimpl_->ui_->lineEdtParaleloPad1, projections[index].hasStandardPararel1, projections[index].valueDef->valdefStandardPararel1*TeCRD);
  setEditValues(pimpl_->ui_->lineEdtParaleloPad2, projections[index].hasStandardPararel2, projections[index].valueDef->valdefStandardPararel2*TeCRD);
  setEditValues(pimpl_->ui_->lineEdtOffsetX, projections[index].hasOffsetX, projections[index].valueDef->valdefOffsetX);
  setEditValues(pimpl_->ui_->lineEdtOffsetY, projections[index].hasOffsetY, projections[index].valueDef->valdefOffsetY);
  setEditValues(pimpl_->ui_->lineEdtEscala, projections[index].hasScale, projections[index].valueDef->valdefScale);

  pimpl_->ui_->grpBoxHemisferio->setEnabled(projections[index].hasHemisphere);

}

void ProjectionDialog::onProjectionChanged(int i)
{
  pimpl_->ui_->cboBoxUnits->setCurrentIndex(projections[i].unit - 1);
  pimpl_->ui_->cboBoxUnits->setEnabled(false);

  pimpl_->ui_->cboBoxZone->setEnabled(projections[i].hasZone);

  if (projections[i].name == "UTM")
  {
    pimpl_->ui_->cboBoxZone->setCurrentIndex(22);
  }

  setEditValues(pimpl_->ui_->lineEdtLatOrigem,
                projections[i].hasOriginLat,
                projections[i].valueDef->valdefOriginLat);

  setEditValues(pimpl_->ui_->lineEdtParaleloPad1,
                projections[i].hasStandardPararel1,
                projections[i].valueDef->valdefStandardPararel1);

  setEditValues(pimpl_->ui_->lineEdtParaleloPad2,
                projections[i].hasStandardPararel2,
                projections[i].valueDef->valdefStandardPararel2);

  setEditValues(pimpl_->ui_->lineEdtOffsetX,
                projections[i].hasOffsetX,
                projections[i].valueDef->valdefOffsetX);

  setEditValues(pimpl_->ui_->lineEdtOffsetY,
                projections[i].hasOffsetY,
                projections[i].valueDef->valdefOffsetY);

  pimpl_->ui_->grpBoxHemisferio->setEnabled(projections[i].hasHemisphere);
}

void ProjectionDialog::onCancelClicked()
{
  close();
}

void ProjectionDialog::setEditValues(QLineEdit* edit, bool enable, double value)
{
  if (enable)
    edit->setText(QString::number(value, 'f', 3));
  else
    edit->clear();
}

void ProjectionDialog::onComboboxZoneActivated(int i)
{
  pimpl_->ui_->lineEdtLongOrigem->clear();
  float lo = -183. + 6.*(i+1);
  QString aux;
  aux.setNum(lo,'f',3);
  pimpl_->ui_->lineEdtLongOrigem->setText(aux);
  pimpl_->ui_->lineEdtLongOrigem->setEnabled(true);
}
