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
  \file terrama2/gui/config/ImageAnalysisDialog.hpp

  \brief Definition of Class ImageAnalysisDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

#ifndef _ImageAnalysisDialog_H_
#define _ImageAnalysisDialog_H_

#include "ui_ImageAnalysisDialog.h"


class ImageAnalysisDialog : public QDialog, private Ui::ImageAnalysisDialog
{
	Q_OBJECT

public:
	ImageAnalysisDialog(QString path, QWidget* parent = 0, Qt::WFlags f = 0);
	~ImageAnalysisDialog();
	void setTimestamp(int year, int month, int day, int hour, int minute, int second);
	void reloadAnalysisListAndImage();

private slots:
	void analysisSelected(int currentRow);
	
private:
	QString _path; //Diretorio em que estao as figuras geradas pelas analises
	QString _fileTimestamp; //Sufixo de todos os arquivos de imagem gerados em evento de analise. Ex.: "_200904231400.jpg"
	QString _timestampStr; //Timestamp que indica a hora em que o evento de analise occorreu. Formato: "dd/MM/yyyy hh:mm:ss"
	QList<QString> _imageFiles; //Contem os nomes dos arquivos gerados por um evento de analise
};


#endif
