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
  \file terrama2/gui/config/QtUtils.hpp

  \brief Definition of auxiliary functions for use with Qt

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _QT_UTILS_H_
#define _QT_UTILS_H_

#include <QObject>

class QTextEdit;

/*! Classe auxiliar contendo funções utilitárias para uso com o Qt
*/
class QtUtils : public QObject
{
  Q_OBJECT

public:
  /*! Seleciona uma linha em um TextEdit.  Esta linha fica selecionada 
      até o usuário efetuar alguma edição no texto
  */
  static void selectLine(QTextEdit* ted, int line);

private:
};


#endif
