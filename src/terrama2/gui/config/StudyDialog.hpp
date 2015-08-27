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
  \file terrama2/gui/config/StudyDialog.hpp

  \brief Definition of Class StudyDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

#ifndef _StudyDialog_H_
#define _StudyDialog_H_

// TerraMA2
#include "Services.hpp"
#include "ImageAnalysisDialog.hpp"

// QT  
#include <QTimer>
  #include "ui_StudyDialog.h"

class StudyDialog : public QDialog, private Ui::StudyDialog
{
Q_OBJECT

public:
  StudyDialog(Services* manager, wsTimestampInterval interval, QWidget* parent = 0, Qt::WFlags f = 0 );
  ~StudyDialog();

private slots:
  void nextRequested();
  void previousRequested();
  void enablePause(QTableWidgetItem*);
  void enableTolerance(int);
  void showAnalysisOnly(int);
  void playClicked();
  void pauseClicked();
  void showNextAnalysisEvent(int oldAnalysisEvent); //Colore na tabela qual evento de analise esta sendo executado e descolore o evento anterior
  void timeoutSlot();

private:
  int _page;
  Services* _manager;
  std::vector<struct wsStudyEvent> _studyEvents;
  void fillStudyEventsTbl();
  void enableExecutionControlButtons(bool enable);
  
  void executeNextAnalysisEvent();
  int _currentAnalysisEvent; //Armazena o indice do evento de analise que esta sendo executado
  bool _pauseRequested;
  QTimer _analysisTimeoutTimer;

  ImageAnalysisDlg* _imageAnalysisDlg;
};


#endif

