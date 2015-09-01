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
  \file terrama2/gui/config/AnalysisCreateLuaDialog.hpp

  \brief Definition of class AnalysisCreateLuaDialog to create lua file for utilization with TerraME

  \author Evandro Delatin
  \author Raphael Willian da Costa
*/


#ifndef _CREATELUADLG_H_
#define _CREATELUADLG_H_

// TerraMA2
#include "ComboBoxDelegate.hpp"
#include "Services.hpp"

// Generated by QT
#include "ui_AnalysisCreateLuaDialog.h"

// QT
#include <QStandardItemModel>


enum SELECT_CELLULAR_SPACE{
  DECLIV = 0,
  LDD = 1,
  ACUM = 2,
  VIS = 3
};

/*!
\brief Class responsible for metric dialog of lua file for TerraME
*/
class AnalysisCreateLuaDialog : public QDialog, private Ui::AnalysisCreateLuaDialog
{
Q_OBJECT

public:
  AnalysisCreateLuaDialog(Services* manager, int cellularSpaceID, const std::vector<std::string> columnCelularSpaceList, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~AnalysisCreateLuaDialog();

  QString getLuaFileLinesString();


private: 
  QString readFile( QString fileName );
  bool fillDescricaoAtributosEspacoCelular();
  bool fillCodificacaoClassesVIS();
  bool fillParametrosEntradaModelo();
  void addNewLineClassCodeTableWidget(int position, QString classCodeName, QString value);
  void addNewLineRainTableWidget(int position, QString value);
  void addNewLineHidrodynamicModelColumnsTableWidget(int position, QString value);

private slots:

  void analysisCreateLuaAddClassCodeItem();
  void analysisCreateLuaRemoveClassCodeItem();
  void reloadClassCodesBtnSlot();
  void classCodeTableWidgetCellClicked( int row, int column);
  void classCodeTableWidgetCellChanged ( int row, int column);

  void analysisCreateLuaAddRainItem();
  void analysisCreateLuaRemoveRainItem();
  void rainTableWidgetCellChanged ( int row, int column);
  void rainTableWidgetCellClicked ( int row , int column);
  void openFileRainPushButtonSlot();

  void hidrodynamicModelColumnsTableWidgetCellChanged ( int row, int colunm);
  void hidrodynamicModelColumnsTableWidgetCellClicked( int row, int col);

  void okPressed();

private:
  QString _modelLua;
  QString _oldRainValue;
  QString _oldClassCodeCNValue;
  QString _oldClassCodeNameValue;
  QString _oldHidrodynamicModelColumns;
  std::vector<std::string> _columnCelularSpaceList;
  Services* _manager;
  int _cellularSpaceID;
};


#endif

