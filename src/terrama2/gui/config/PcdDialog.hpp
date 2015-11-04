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
  \file terrama2/gui/config/PcdDialog.hpp

  \brief Class responsible to handle the PCD insertion/modification file

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


#ifndef __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__

// TerraMA2
#include "ui_PcdForm.h"

// Boost
#include <boost/noncopyable.hpp>

// temp struct for stores the pcd meta
struct PCD
{
  QString file;
  QString latitude;
  QString longitude;
  bool active;
};

class PcdDialog : public QDialog, private boost::noncopyable
{
  Q_OBJECT

  public:
    PcdDialog(QWidget* parent = 0, Qt::WindowFlags f = 0 );
    ~PcdDialog();

    void fill(const PCD& pcd);
    void fillObject(PCD& pcd);

  private slots:
    void onPcdChanged();
    void onConfirmClicked();

  private:
    struct Impl;
    Impl* pimpl_;
};


#endif // __TERRAMA2_GUI_CONFIG_PCDDIALOG_HPP__

