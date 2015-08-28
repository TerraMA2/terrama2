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
  \file terrama2/gui/config/SurfacesConfigDialog.hpp

  \brief Definition of Class SurfacesConfigDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

#ifndef _GRIDCONFIGDLG_H_
#define _GRIDCONFIGDLG_H_

// TerraMA2
#include <soapH.h>
#include "ui_SurfacesConfigDialog.h"
#include <TeBox.h>

class WeatherGridList;
class Services;

class SurfacesConfigDialog : public QDialog, private Ui::SurfacesConfigDialog
{
  Q_OBJECT
  
public:
	SurfacesConfigDialog(const QVector<wsPCD> &collectionPoints,
										const QVector<std::string> &collectionAttributes,
										const QVector<wsWeatherDataPcdSurface> &collectionSurfaces,
										const wsProjectionParams &projection,
										QWidget *parent = 0, Qt::WFlags f = 0);

  ~SurfacesConfigDialog() {}
  
	void getFields(QVector<wsWeatherDataPcdSurface> &collectionSurfaces, bool &changed);

private slots:
	void addSurface();
	void removeSurface();
	void menuMaskClick(QAction* actMenu);
	void interpolationChanged(int index);
	void resDegreeRadioToggled(bool checked);
	void updateCurrentSurface();
	void trimGridOutputName();
	void updateSurfaceList();
	void showSurfaceDetails(int row);
	void checkAndAccept();
	void checkAndReject();

private:
	void updateSurfaceList(int row);
	void loadAttributeList();
	void clearSurfaceDetails();
	void enableWidgets(bool enable);
	bool verifyDados(QString &errMsg);
	bool verifyDadosAt(int row, QString & errMsg);
	bool configChanged();

	QWidget                                *_parent;
	QVector<struct wsWeatherDataPcdSurface> _startConfig;
	QVector<struct wsPCD>                   _collectionPoints;
	QVector<struct wsWeatherDataPcdSurface> _collectionSurfaces;
	QVector<std::string>                    _collectionAttributes;
	wsProjectionParams                      _projection;
	bool                                    _attributeListIsLoaded;
	TeBox                                   _baseBox;
};

#endif
