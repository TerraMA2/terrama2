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
  \file terrama2/gui/config/ProjectionDialog.hpp

  \brief Definition of Class ProjectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

#ifndef __TERRAMA2_GUI_CONFIG_PROJECTIONDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_PROJECTIONDIALOG_HPP__

// TerraMA2
#include "ui_ProjectionDialogForm.h"

// Boost
#include <boost/noncopyable.hpp>
#include <QtCore/qobjectdefs.h>

// Forward Declaration
class QDialog;
class QWidget;
class QLineEdit;

struct ProjectionValue
{
  double valdefOriginLat;
  double valdefOriginLong;
  double valdefStandardPararel1;
  double valdefStandardPararel2;
  double valdefOffsetX;
  double valdefOffsetY;
  double valdefScale;
};

struct Projection
{
  QString name;
  int  unit;
  bool hasUnit;
  bool hasZone;
  bool hasOriginLat;
  bool hasOriginLong;
  bool hasStandardPararel1;
  bool hasStandardPararel2;
  bool hasOffsetX;
  bool hasOffsetY;
  bool hasScale;
  bool hasHemisphere;
  ProjectionValue *valueDef;
};

class ProjectionDialog : public QDialog, private boost::noncopyable
{
  Q_OBJECT

  public:
    ProjectionDialog(QWidget* parent = 0, Qt::WindowFlags f = 0 );
    ~ProjectionDialog();

  private:
    void setupProjection(int);
    void setEditValues(QLineEdit* ledtItem, bool enable, double value);

  private slots:
    void onProjectionChanged(int);
    void onCancelClicked();
    void onComboboxZoneActivated(int);

  private:
    struct Impl;
    Impl* pimpl_;
};


#endif // __TERRAMA2_GUI_CONFIG_PROJECTIONDIALOG_HPP__

