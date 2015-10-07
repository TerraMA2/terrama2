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


static ProjectionValue valueDef = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
static ProjectionValue valueDefUTM = { -39.0, -45.00, 0.0, 0.0, 500000.00, 10000000.000, 0.9996};
//static ProjectionValue valueDefUTM = { TeCDR*-39.0, -45.00, 0.0, 0.0, 500000.00, 10000000.000, 0.9996  };

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
}

ProjectionDialog::~ProjectionDialog()
{
  delete pimpl_;
}

void ProjectionDialog::onProjectionChanged(int i)
{
  pimpl_->ui_->okBtn->setEnabled(true);

  pimpl_->ui_->cboBoxUnits->setCurrentIndex(projections[i].unit - 1);
  pimpl_->ui_->cboBoxUnits->setEnabled(false);

  pimpl_->ui_->cboBoxZone->setEnabled(projections[i].hasZone);

  if (projections[i].name == "UTM")
  {
    pimpl_->ui_->cboBoxZone->setCurrentIndex(22);
//    cboBoxZone_activated(22);
  }
}
