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
  \file terrama2/gui/config/IntersectionDialog.hpp

  \brief Definition of Class IntersectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _IntersectionDialog_H_
#define _IntersectionDialog_H_

#include "ui_IntersectionDialog.h"

// TerraMA2
#include "WeatherGridList.hpp"

// QT  
#include <QMap>
#include <QSet>

#include "soapH.h"

class IntersectionDialog : public QDialog, private Ui::IntersectionDialog
{
Q_OBJECT

public:
  IntersectionDialog(QWidget* parent = 0, Qt::WFlags f = 0 );
  ~IntersectionDialog();
  void setFields(const wsWeatherDataSourceIntersection& interParams, std::vector<struct wsTheme> themes, WeatherGridList* weatherGridList);
  void getFields(wsWeatherDataSourceIntersection *interParams, bool &changed);
  
private slots:
  void goToGridPageSlot();
  void goToThemePageSlot();
  void setIntersectionChanged();
  void changeRowTheme();
  void setBandIntersection();
  void enableStaticVectorIntersection(QTableWidgetItem*);
  void enableDynamicGridIntersection(QTableWidgetItem*);

private:
	bool _intersectionChanged;
	bool _ignoreChangeEvents;
	std::vector<struct wsTheme> _themes;
	std::map<int, wsWeatherDataSourceIntersectionRaster> _interRasterMap; //!< Mapeia o id de um tema raster a estrutura que define a sua forma de intersecao
	QMap< int, QSet<QString> > _interVectorMap; //!< Mapeia o id de um tema vetorial a um conjunto de atributos deste tema que serao usados na intersecao
	QSet<int> _interDataSource; //!< Armazena os IDs das fontes de dados que estao selecionadas para intersecao
	QPixmap _transparentPixmap; //!< Usado na criacao do icone que faz o alinhamento dos itens da lista de temas estaticos

	wsWeatherDataSourceIntersectionRaster findIntersectionRaster(std::vector<wsWeatherDataSourceIntersectionRaster> interRaster, int themeId);
	std::vector<std::string> findIntersectionVectorAttributes(std::vector<wsWeatherDataSourceIntersectionVector> interVector, int themeId);
};


#endif

