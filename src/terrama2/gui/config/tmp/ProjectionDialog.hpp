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

#ifndef _ProjectionDialog_H_
#define _ProjectionDialog_H_

// TerraMA2
#include "ui_ProjectionDialog.h"
#include "soapServPlanosProxy.h"
#include <TeDefines.h>

struct vdValueDefault
{
	long double valdefOriginLat;
	long double valdefOriginLong;
	long double valdefStandardPararel1;
	long double valdefStandardPararel2;
	long double valdefOffsetX;
	long double valdefOffsetY;
	long double valdefScale;
};

struct pdProjection
{
	QString name;
	int  unit;
	bool haveUnit;
	bool haveZone;
	bool haveOriginLat;
	bool haveOriginLong;
	bool haveStandardPararel1;
	bool haveStandardPararel2;
	bool haveOffsetX;
	bool haveOffsetY;
	bool haveScale;
	bool haveHemisphere;
	struct vdValueDefault *valueDef;
};

class ProjectionDialog : public QDialog, private Ui::ProjectionDialog
{
Q_OBJECT

public:
  ProjectionDialog(QWidget* parent = 0, Qt::WFlags f = 0 );
  ~ProjectionDialog();

  void setupProjection(int projectionIndex);

  void setFields(const wsProjectionParams& projectionParams);
  void getFields(wsProjectionParams* projectionParams, bool& changed);
  void setEnabledFields(bool mode);

private slots:
  void changeComboProjection(int index);
  void setProjectionChanged();
  void lineEdtLongOrigem_textEdited();
  void cboBoxZone_activated(int i);
  
private:
  std::vector<pdProjection> _vetProj;
  std::vector<QString> _vetDatum;

  bool _EnableChangeValuesSetupProjection;

  bool  _ignoreChangeEvents; //!< Indica que o slot que monitora mudanças está temporariamente desligado
  bool _projectionChanged; //!< Indica que algum dos dados apresentados foi alterado pelo usuário

  void Init();
  void setEditValues(QLineEdit* ledtItem, bool enable, double value);

};


#endif

