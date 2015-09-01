/************************************************************************************
TerraMA² is a development platform of Monitoring, Analysis and Alert for
geoenvironmental extremes. Applied to natural disasters, quality of water / air / atmosphere,
tailings basins, epidemics and others.Copyright © 2012 INPE.

This code is part of the TerraMA² framework.
This framework is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose.
The library provided hereunder is on an "as is" basis, and the authors have no
obligation to provide maintenance, support, updates, enhancements,
or modifications.

In no event shall INPE or K2 SISTEMAS be held liable to any party for direct, indirect,
special, incidental, or consequential damages arising out of the use of this
library and its documentation.
*************************************************************************************/

#include <qapplication.h>

#include "MainDialog.hpp"
// #include "Language.h"

int main( int argc, char** argv )
{
  QApplication app( argc, argv );

  //Carrega a linguagem do sistema e os translators.
  // loadLanguage("config");

  // Alteracao para contornar um problema que ocorria no Mandriva.
  // TerraLib usa sprintf para gerar alguns comandos SQL INSERT.
  // Dependendo do idioma do sistema, era usado virgula nos numeros reais, o que atrapalhava a execucao do INSERT.
#ifndef WIN32
  setlocale(LC_NUMERIC,"en_US.UTF-8");
#endif
    
  // Abre janela principal
  MainDialog mainwindow;

  mainwindow.show();
  return app.exec();
}

