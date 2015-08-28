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
  \file terrama2/gui/config/ProjectionDialog.cpp

  \brief Definition of Class ProjectionDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa    
*/

// STL
#include <assert.h>

// TerraMA2
#include "ProjectionDialog.hpp"

static vdValueDefault valueDef = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  };
static vdValueDefault valueDefUTM = { TeCDR*-39.0, -45.00, 0.0, 0.0, 500000.00, 10000000.000, 0.9996  };

static pdProjection listProjections[] = {
		{ "NoProjection",           1, false, false, false, false, false, false, false, false, false, false, &valueDef },
		{ "Albers",                 1, true,  false, true,  true,  true,  true,  true,  true,  false, false, &valueDef },
		{ "LatLong",                2, true,  false, false, false, false, false, false, false, false, false, &valueDef },
		{ "LambertConformal",       1, true,  false, true,  true,  true,  true,  true,  true,  false, false, &valueDef },
		{ "Mercator",               1, true,  false, true,  true,  true,  false, true,  true,  false, false, &valueDef },
		{ "Miller",                 1, true,  false, false, true,  false, false, true,  true,  false, false, &valueDef },
		{ "UTM",                    1, true,  true,  false, true,  false, false, true,  true,  false, true,  &valueDefUTM },
		{ "Polyconic",              1, true,  false, true,  true,  false, false, true,  true,  false, false, &valueDef },
		{ "Sinusoidal",             1, true,  false, false, true,  false, false, true,  true,  false, false, &valueDef },
		{ "CylindricalEquidistant", 1, true,  false, false, true,  true,  false, true,  true,  false, false, &valueDef },
		{ "PolarStereographic",     1, true,  false, false, true,  false, false, true,  true,  false, true,  &valueDef },
		};

static QString listDATA_Names[] = { "Astro Chua", "Corrego Alegre", "Indian", "NAD27", "NAD83",
									"SAD69", "Spherical", "WGS84", "Aratu" };

static bool TeLongDMS2DD(char hem, short& dg, short& mn, float& sc, double& grauDec)
{
	short  	posat,lim = 180;

	double	dd,pi;			
	posat = 0;
	pi = 4.*atan((double)1.);

	// Longitude 
	if(dg < 0 || dg > lim)	
		return false;

	if(mn < 0 || mn > 60)
		return false;

	if(sc < 0. || sc > 60.)
		return false;

	if(hem == 'W' || hem == 'w'|| hem == 'O'|| hem == 'o')
	{
		if(dg > 0)
			dg = dg * -1;
		else if(mn > 0)
			mn = mn * -1;
		else
			sc = (float)(sc * -1.);
	}

	dd = (double)(abs(dg)) + ((double)abs(mn)/60.) + fabs(sc)/3600.;
	if (dg < 0 || mn < 0 || (int)sc < 0)
		dd = -dd;
	grauDec = dd;
	return true;
}


static bool longValidator(const QString str, double & val)
{
	QStringList myList;
	QRegExp exp1("^[oOwWeElL][ ]\\d\\d?\\d?[ ]?\\d\\d?[ ]?\\d?\\d?\\.?[0-9]?" );
	QRegExp exp2("^[oOwWeElL][;]\\d\\d?\\d?[;]?\\d\\d?[;]?\\d?\\d?\\.?[0-9]?" ); 
	QRegExp exp3("^[oOwWeElL][:]\\d\\d?\\d?[:]?\\d\\d?[:]?\\d?\\d?\\.?[0-9]?" ); 
	QRegExp exp4("^[+-]?\\d\\d?\\d?\\.?[0-9]*" ); 

	double grauDec;

    if ( exp1.exactMatch( str ) )
	{
		myList = str.split(" ");
	}
	else if ( exp2.exactMatch( str ) )
	{
		myList = str.split(";");
	}
	else if ( exp3.exactMatch( str ) )
	{
		myList = str.split(":");
	}
	else if ( exp4.exactMatch( str ) )
	{
		grauDec = str.toDouble();
		if ((grauDec >= -180.0 && grauDec <=180.0))
		{
			val = grauDec * TeCDR;
			return true;
		}
		else
			return false;
	}
	else
		return false;

	int nelementos = myList.count();
	short graus, minutos = 0;
	float segundos = 0.0;
	
	graus = myList[1].toShort();

	if (nelementos > 2)
		minutos = myList[2].toShort();
	
	if (nelementos > 3)
		segundos = myList[3].toFloat();

	if (TeLongDMS2DD(myList[0].at(0).toAscii(),graus, minutos, segundos, grauDec))
	{
		val = grauDec * TeCDR;
		return true;
	}
	else
		return false;
}

//! Construtor
ProjectionDialog::ProjectionDialog(QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
  setupUi(this);

  _ignoreChangeEvents = true;
  _projectionChanged  = false;

  _EnableChangeValuesSetupProjection = true;

  Init();

  // Connect
  connect(cboBoxProjection,     SIGNAL(currentIndexChanged(int)), SLOT(changeComboProjection(int)));

  connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  // Conecta sinais para detectar dados alterados
  connect(cboBoxProjection,	   SIGNAL(currentIndexChanged(int)), SLOT(setProjectionChanged()));
  connect(cboBoxDatum,		   SIGNAL(currentIndexChanged(int)), SLOT(setProjectionChanged()));
  connect(cboBoxUnits,		   SIGNAL(currentIndexChanged(int)), SLOT(setProjectionChanged()));
  connect(cboBoxZone,		   SIGNAL(currentIndexChanged(int)), SLOT(setProjectionChanged()));
  connect(cboBoxZone,		   SIGNAL(activated(int)),           SLOT(cboBoxZone_activated(int)));


  connect(lineEdtLongOrigem,   SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtLongOrigem,   SIGNAL(textEdited(const QString&)), SLOT(lineEdtLongOrigem_textEdited()));
  connect(lineEdtLatOrigem,	   SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtParaleloPad1, SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtParaleloPad2, SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtOffsetX,	   SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtOffsetY,	   SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));
  connect(lineEdtEscala,	   SIGNAL(textEdited(const QString&)), SLOT(setProjectionChanged()));

  // Define os tipos para validação
  lineEdtLongOrigem->setValidator(new QDoubleValidator(lineEdtLongOrigem));
  lineEdtLatOrigem->setValidator(new QDoubleValidator(lineEdtLatOrigem));
  lineEdtParaleloPad1->setValidator(new QDoubleValidator(lineEdtParaleloPad2));
  lineEdtParaleloPad2->setValidator(new QDoubleValidator(lineEdtParaleloPad2));
  lineEdtOffsetX->setValidator(new QDoubleValidator(lineEdtOffsetX));
  lineEdtOffsetY->setValidator(new QDoubleValidator(lineEdtOffsetY));
  lineEdtEscala->setValidator(new QDoubleValidator(lineEdtEscala));

  // limita edicao em 3 casas decimais
  ((QDoubleValidator*)lineEdtLongOrigem->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtLatOrigem->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtParaleloPad1->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtParaleloPad2->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtOffsetX->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtOffsetY->validator())->setDecimals(3);
  ((QDoubleValidator*)lineEdtEscala->validator())->setDecimals(3);

  _ignoreChangeEvents = false;
}

//! Inicializa valores default da interface
void ProjectionDialog::Init()
{
  bool oldStat = _ignoreChangeEvents;

  QStringList strLst;
  unsigned  i;

  _ignoreChangeEvents = true;

  // Preenche Combo Zonas
  for (i=1; i<=60; i++)
	strLst.push_back("Zone "+QString::number(i));

  cboBoxZone->addItems(strLst);

  // Preenche combo Datum
  std::vector<QString> vetDatum(listDATA_Names, listDATA_Names + sizeof(listDATA_Names)/sizeof(QString));
  _vetDatum = vetDatum;

  strLst.clear();

  for (i=0; i < _vetDatum.size(); i++)
    strLst.push_back(_vetDatum[i]);

  cboBoxDatum->addItems(strLst);

  // Preenche combo Projecoes
  std::vector<pdProjection> vetProj(listProjections, listProjections + sizeof(listProjections)/sizeof(pdProjection));
  _vetProj = vetProj;

  strLst.clear();

  for (i=0; i < _vetProj.size(); i++)
    strLst.push_back(_vetProj[i].name);

  cboBoxProjection->addItems(strLst);

  cboBoxProjection->setCurrentIndex(0);
  setupProjection(cboBoxProjection->currentIndex());

  _ignoreChangeEvents = oldStat;
}

//! Destrutor
ProjectionDialog::~ProjectionDialog()
{
}


//! Trata alteração da Combo de Projeção. Ao ser alterada, deve atualizar os valores default da interface
void ProjectionDialog::changeComboProjection(int index)
{
	setupProjection(index);
}

//! Atualiza valores da interface de acordo com a projeção selecionada.
void ProjectionDialog::setupProjection(int projectionIndex)
{
  bool lOldStat = _ignoreChangeEvents;
  _ignoreChangeEvents = true;

  cboBoxUnits->setCurrentIndex(_vetProj[projectionIndex].unit -1);
  cboBoxUnits->setEnabled(false);

  cboBoxZone->setEnabled(_vetProj[projectionIndex].haveZone);
  if (_vetProj[projectionIndex].name == "UTM") 
  {
	  cboBoxZone->setCurrentIndex(22);
	  cboBoxZone_activated(22);
  }
  else
	  setEditValues(lineEdtLongOrigem, _vetProj[projectionIndex].haveOriginLong, _vetProj[projectionIndex].valueDef->valdefOriginLong*TeCRD);

  bool oldEnableChange = _EnableChangeValuesSetupProjection;
  _EnableChangeValuesSetupProjection = true;

  setEditValues(lineEdtLatOrigem, _vetProj[projectionIndex].haveOriginLat, _vetProj[projectionIndex].valueDef->valdefOriginLat*TeCRD);

  setEditValues(lineEdtParaleloPad1, _vetProj[projectionIndex].haveStandardPararel1, _vetProj[projectionIndex].valueDef->valdefStandardPararel1*TeCRD);
  setEditValues(lineEdtParaleloPad2, _vetProj[projectionIndex].haveStandardPararel2, _vetProj[projectionIndex].valueDef->valdefStandardPararel2*TeCRD);

  setEditValues(lineEdtOffsetX, _vetProj[projectionIndex].haveOffsetX, _vetProj[projectionIndex].valueDef->valdefOffsetX);
  setEditValues(lineEdtOffsetY, _vetProj[projectionIndex].haveOffsetY, _vetProj[projectionIndex].valueDef->valdefOffsetY);

  setEditValues(lineEdtEscala, _vetProj[projectionIndex].haveScale, _vetProj[projectionIndex].valueDef->valdefScale);
  
  grpBoxHemisferio->setEnabled(_vetProj[projectionIndex].haveHemisphere);
  
  _EnableChangeValuesSetupProjection = oldEnableChange;
  _ignoreChangeEvents = lOldStat;
}

//! Rotina auxiliar para tratar lineEdits (habilitar/desabilitar e atribuir valores)
void ProjectionDialog::setEditValues(QLineEdit* ledtItem, bool enable, double value)
{
  ledtItem->setEnabled(enable);

  if (_EnableChangeValuesSetupProjection)
  {
    if (enable)
	  ledtItem->setText(QString::number(value, 'f',3));
    else
  	  ledtItem->clear();
  }
}

//! Preenche valores da interface conforme estrutura da projeção
void ProjectionDialog::setFields(const wsProjectionParams& projectionParams)
{
  bool lOldStat = _ignoreChangeEvents;
  _ignoreChangeEvents = true;

  if (projectionParams.name==WS_PROJNAME_NoProjection)
	  _EnableChangeValuesSetupProjection = true;
  else
	  _EnableChangeValuesSetupProjection = false;

  cboBoxProjection->setCurrentIndex(projectionParams.name);
  cboBoxDatum->setCurrentIndex(projectionParams.datum);
  cboBoxUnits->findText(QString::fromStdString(projectionParams.units));
  
  cboBoxZone->setCurrentIndex((int)((projectionParams.lon0*TeCRD+183.0)/6.0)-1);
  lineEdtLongOrigem->setText(QString::number(projectionParams.lon0*TeCRD, 'f',3));
  
  lineEdtLatOrigem->setText(QString::number(projectionParams.lat0*TeCRD, 'f',3));
  lineEdtParaleloPad1->setText(QString::number(projectionParams.stlat1*TeCRD, 'f',3));
  lineEdtParaleloPad2->setText(QString::number(projectionParams.stlat2*TeCRD, 'f',3));
  lineEdtOffsetX->setText(QString::number(projectionParams.offx, 'f',3));
  lineEdtOffsetY->setText(QString::number(projectionParams.offy, 'f',3));
  lineEdtEscala->setText(QString::number(projectionParams.scale, 'f',3));

  rbNorthBtn->setChecked(projectionParams.hemisphere == WS_HEMISPHERE_NORTH);
  rbSouthBtn->setChecked( projectionParams.hemisphere == WS_HEMISPHERE_SOUTH);

  _ignoreChangeEvents = lOldStat;
}

//! Preenche estrutura da projeção conforme valores da interface
void ProjectionDialog::getFields(wsProjectionParams* projectionParams, bool& changed)
{
  projectionParams->name = (wsProjectionName) cboBoxProjection->currentIndex();
  projectionParams->datum = (wsDatum) cboBoxDatum->currentIndex();
  projectionParams->units = cboBoxUnits->currentText().toStdString();

  QString auxStr;

  projectionParams->lon0 = TeCDR*lineEdtLongOrigem->text().toDouble();
  projectionParams->lat0 = TeCDR*lineEdtLatOrigem->text().toDouble();
  projectionParams->stlat1 = TeCDR*lineEdtParaleloPad1->text().toDouble();
  projectionParams->stlat2 = TeCDR*lineEdtParaleloPad2->text().toDouble();
  projectionParams->offx = lineEdtOffsetX->text().toDouble();
  projectionParams->offy = lineEdtOffsetY->text().toDouble();
  projectionParams->scale = lineEdtEscala->text().toDouble();

  projectionParams->hemisphere = rbNorthBtn->isChecked() ? WS_HEMISPHERE_NORTH : WS_HEMISPHERE_SOUTH;
  changed = _projectionChanged;
}

//! Habilita/desabilita campos da interface
void ProjectionDialog::setEnabledFields(bool mode)
{
  cboBoxProjection->setEnabled(mode);
  cboBoxDatum->setEnabled(mode);
  cboBoxUnits->setEnabled(mode);
  cboBoxZone->setEnabled(mode);
  lineEdtLongOrigem->setEnabled(mode);
  lineEdtLatOrigem->setEnabled(mode);
  lineEdtParaleloPad1->setEnabled(mode);
  lineEdtParaleloPad2->setEnabled(mode);
  lineEdtOffsetX->setEnabled(mode);
  lineEdtOffsetY->setEnabled(mode);
  lineEdtEscala->setEnabled(mode);
  grpBoxHemisferio->setEnabled(mode);
//  cancelBtn->setEnabled(mode);
  if (mode)
  {
    okBtn->setEnabled(false);
    cancelBtn->show();
  }
  else
  {
    okBtn->setEnabled(true);
    cancelBtn->hide();
  }

  if (mode)
    setupProjection(cboBoxProjection->currentIndex());
}

//! Marca flag indicando que houve alteração em algum campo
void ProjectionDialog::setProjectionChanged()
{
  if (_ignoreChangeEvents)
	  return;

  _projectionChanged = true;
  okBtn->setEnabled(true);
}


void ProjectionDialog::lineEdtLongOrigem_textEdited()
{
 	double val;
	if (longValidator(lineEdtLongOrigem->text(), val))
		cboBoxZone->setCurrentIndex(static_cast<int>(((val*TeCRD+183.0)/6.0)-1));
}



void ProjectionDialog::cboBoxZone_activated(int z)
{
	lineEdtLongOrigem->clear();
	float lo = -183. + 6.*(z+1);
	QString aux;
	aux.setNum(lo,'f',3);
	lineEdtLongOrigem->setText(aux);
	lineEdtLongOrigem->setEnabled(true);
}