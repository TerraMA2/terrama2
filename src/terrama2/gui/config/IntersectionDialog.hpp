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

  \brief It defines the Intersection Form Dialog for Occurrence datasets

  \author Raphael Willian da Costa
*/

#ifndef __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__
#define __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__

// STL
#include <memory>
#include "../../core/Intersection.hpp"


// Forward declaration
struct Database;

namespace terrama2
{
  namespace core
  {
    class Intersection;
  }
}

namespace te
{
  namespace da
  {
    class DataSetType;
    class DataSourceTransactor;
  }
}

class IntersectionDialog : public QDialog
{
  Q_OBJECT

  public:
    IntersectionDialog(const terrama2::core::Intersection& intersection, Database* database, QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~IntersectionDialog();

    void fillAttributeTable(std::auto_ptr<te::da::DataSetType> dataset);
    void fillVectorialList();
    void fillRasterList();
    terrama2::core::Intersection getIntersection() const;

  private slots:
    void onOkBtnClicked();
    void onDatasetSelected();
    void onRasterSelected(int row, int column);
    void onAttributeSelected(int row, int column);
    void showVectorialPage();
    void showGridPage();

  private:
    struct Impl;
    Impl* pimpl_;

};


#endif // __TERRAMA2_GUI_CONFIG_INTERSECTIONDIALOG_HPP__

