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
  \file terrama2/gui/config/SurfacesConfigDialog.cpp

  \brief Definition of Class SurfacesConfigDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  
*/

// TerraMA2
#include "SurfacesConfigDialog.hpp"
#include <WsUtils.hpp>
  
// QT
#include <QMenu>
#include <QListWidgetItem>
#include <QMessageBox>

// STL
#include <memory>


SurfacesConfigDialog::SurfacesConfigDialog(const QVector<struct wsPCD> &collectionPoints,
                                     const QVector<std::string> &collectionAttributes,
                                     const QVector<struct wsWeatherDataPcdSurface> &collectionSurfaces,
                                     const wsProjectionParams &projection,
                                     QWidget *parent, Qt::WFlags f) : QDialog(parent, f), _parent(parent),

  _collectionPoints(collectionPoints),
  _collectionAttributes(collectionAttributes),
  _collectionSurfaces(collectionSurfaces),
  _projection(projection),
  _attributeListIsLoaded(false),
  _baseBox(0, 0, 0, 0)

{
  setupUi(this);

  // métodos de interpolação
  interpCmb->addItem(tr("Vizinho mais próximo"));
  interpCmb->addItem(tr("K-vizinhos mais próximos"));
  interpCmb->addItem(tr("K-vizinhos mais próximos ponderados"));
#ifndef _WIN64  
  interpCmb->addItem(tr("Superfície de tendência"));
#endif

  // Menus associados a botões
  QMenu* menuMask = new QMenu(tr("Máscaras"), _parent);
  menuMask->addAction(tr("%a - ano com dois digitos"));
  menuMask->addAction(tr("%A - ano com quatro digitos"));
  menuMask->addAction(tr("%d - dia com dois digitos"));
  menuMask->addAction(tr("%M - mes com dois digitos"));
  menuMask->addAction(tr("%h - hora com dois digitos"));
  menuMask->addAction(tr("%m - minuto com dois digitos"));
  menuMask->addAction(tr("%s - segundo com dois digitos"));

  fileAnaGridMaskBtn->setMenu(menuMask);
  fileAnaGridMaskBtn->setPopupMode(QToolButton::InstantPopup);

  // Coneta os sinais
  connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(menuMaskClick(QAction*)));

  connect(surfaceList,    SIGNAL(currentRowChanged(int)),   SLOT(showSurfaceDetails(int)));
  connect(interpCmb,      SIGNAL(currentIndexChanged(int)), SLOT(interpolationChanged(int)));
  connect(resDegreeRadio, SIGNAL(toggled(bool)),            SLOT(resDegreeRadioToggled(bool)));

  connect(outputGridEdit,  SIGNAL(textEdited(const QString&)), SLOT(updateCurrentSurface()));
  connect(attributeCmb,    SIGNAL(activated(int)),             SLOT(updateCurrentSurface()));
  connect(interpCmb,       SIGNAL(activated(int)),             SLOT(updateCurrentSurface()));
  connect(neighborNumEdit, SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(powValueEdit,    SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(degreeCmb,       SIGNAL(activated(int)),             SLOT(updateCurrentSurface()));
  connect(x1Edit,          SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(y1Edit,          SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(x2Edit,          SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(y2Edit,          SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(resXEdit,        SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(resYEdit,        SIGNAL(editingFinished()),          SLOT(updateCurrentSurface()));
  connect(resDegreeRadio,  SIGNAL(clicked(bool)),              SLOT(updateCurrentSurface()));
  connect(resMeterRadio,   SIGNAL(clicked(bool)),              SLOT(updateCurrentSurface()));

  connect(outputGridEdit,  SIGNAL(textEdited(const QString&)), SLOT(updateSurfaceList()));
  connect(outputGridEdit,  SIGNAL(editingFinished()),          SLOT(trimGridOutputName()));

  connect(addAttributeBtn,    SIGNAL(clicked()), SLOT(addSurface()));
  connect(removeAttributeBtn, SIGNAL(clicked()), SLOT(removeSurface()));
  connect(okBtn,              SIGNAL(clicked()), SLOT(checkAndAccept()));
  connect(cancelBtn,          SIGNAL(clicked()), SLOT(checkAndReject()));

  neighborNumEdit->setValidator(new QIntValidator(0, 9999, neighborNumEdit));
  powValueEdit->setValidator(new QIntValidator(0, 9999, powValueEdit));

  x1Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, x1Edit));
  y1Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, y1Edit));
  x2Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, x2Edit));
  y2Edit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, y2Edit));

  resXEdit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, resXEdit));
  resYEdit->setValidator(new QDoubleValidator(-TeMAXFLOAT, TeMAXFLOAT, 20, resYEdit));

  // Carrega as superfícies já cadastradas
  if(!collectionSurfaces.isEmpty())
  {
    loadAttributeList();
    updateSurfaceList(0);
  }
  else // Nenhuma cadastrada, desabilita a parte direita da interface
    clearSurfaceDetails();

  // Calcula o box padrão
  {
    TePointSet pointSet;
    for(int i = 0; i < _collectionPoints.size(); i++)
    {
      if(_collectionPoints.at(i).isActive)
      {
        TeCoord2D aux = TeCoord2D(_collectionPoints.at(i).longitude, _collectionPoints.at(i).latitude);
        TePoint p(aux);
        pointSet.add(p);
      }
    }

    if(pointSet.size() > 0)
    {
      std::auto_ptr<TeProjection> projection(TeProjectionFactory::make(wsUtils::ProjectionParams::teFromWs(_projection)));

      for(unsigned int i = 0; i < pointSet.size(); ++i)
      {
        if(pointSet[i].srid() == -1)
          pointSet[i].srid(projection->epsgCode());
      }

      // Atualiza o box padrão
      _baseBox = pointSet.box();
    }
  }

  // Grava configuração inicial
  _startConfig = _collectionSurfaces;
}

//! Preenche collectionSurfaces conforme valores da interface
void SurfacesConfigDialog::getFields(QVector<struct wsWeatherDataPcdSurface> &collectionSurfaces, bool &changed)
{ 
  collectionSurfaces = _collectionSurfaces;
  changed = configChanged();
}

//! Slot chamado quando o usuário clica no botao para adicionar uma nova superficie
void SurfacesConfigDialog::addSurface(){
  if(_collectionAttributes.isEmpty())
  {
    QMessageBox::information(_parent, tr("Nova superfície"), QObject::tr("O PCD ainda não realizou nenhuma coleta."));
    return;
  }
  loadAttributeList();

  wsWeatherDataPcdSurface surface;
  surface.attributeName       = _collectionAttributes.at(0);
  surface.gridOutputName      = "";
  surface.interpolationMethod = WS_SURFACE_INTERPOLATION_NN_METHOD;
  surface.numberOfNeighbors   = 1;
  surface.powValue            = 1;
  surface.x1                  = _baseBox.x1_;
  surface.y1                  = _baseBox.y1_;
  surface.x2                  = _baseBox.x2_;
  surface.y2                  = _baseBox.y2_;
  surface.coordUnit           = WS_COORD_DECIMAL_DEGREES;
  surface.resX                = 0.01;
  surface.resY                = 0.01;
  surface.surfaceId     = -1;

  _collectionSurfaces.append(surface);

  updateSurfaceList(_collectionSurfaces.count()-1);

  enableWidgets(true);

  // Nome temporario da nova superfície na lista
  surfaceList->currentItem()->setText("Nova superfície");
}

//! Slot chamado quando o usuário clica no botao para remover uma superficie
void SurfacesConfigDialog::removeSurface(){

  int row = surfaceList->currentRow();

  if(row == -1)
    return;

  _collectionSurfaces.remove(row);

  updateSurfaceList(--row);

  if(_collectionSurfaces.isEmpty())
    clearSurfaceDetails();
}

//! Slot chamado quando o usuário clica no botao de mascara
void SurfacesConfigDialog::menuMaskClick(QAction* actMenu)
{
  outputGridEdit->setText(outputGridEdit->text() + actMenu->text().left(2));
  updateCurrentSurface();
  updateSurfaceList();
}

//! Slot chamado quando o usuário troca de algoritimo de interpolação
void SurfacesConfigDialog::interpolationChanged(int index)
{
  if((wsSurfaceInterpolationMethod)index == WS_SURFACE_INTERPOLATION_NN_METHOD)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(false);
    neighborNumEdit->setVisible(false);
    powValueLbl->setVisible(false);
    powValueEdit->setVisible(false);
  }
  else if((wsSurfaceInterpolationMethod)index == WS_SURFACE_INTERPOLATION_AVERAGE)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(true);
    neighborNumEdit->setVisible(true);
    powValueLbl->setVisible(false);
    powValueEdit->setVisible(false);
  }
  else if((wsSurfaceInterpolationMethod)index == WS_SURFACE_INTERPOLATION_WEIGHT_AVERAGE)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(true);
    neighborNumEdit->setVisible(true);
    powValueLbl->setVisible(true);
    powValueEdit->setVisible(true);
  }
  else if((wsSurfaceInterpolationMethod)index == WS_SURFACE_INTERPOLATION_POLYNOMIAL)
  {
    stackedWidget->setCurrentIndex(1);
  }
}

void SurfacesConfigDialog::resDegreeRadioToggled(bool checked)
{
  double resX = resXEdit->text().toDouble();
  double resY = resYEdit->text().toDouble();

  if(checked)
  {
    resX = TeConvertUnits(resX, TeMeters, TeDecimalDegress);
    resY = TeConvertUnits(resY, TeMeters, TeDecimalDegress);
  }
  else
  {
    resX = TeConvertUnits(resX, TeDecimalDegress, TeMeters);
    resY = TeConvertUnits(resY, TeDecimalDegress, TeMeters);
  }

  resXEdit->setText(QString::number(resX));
  resYEdit->setText(QString::number(resY));
}

//! Slot chamado quando a superfície atual foi alterada pelo usuário
void SurfacesConfigDialog::updateCurrentSurface()
{
  int row = surfaceList->currentRow();

  if(row == -1)
    return;

  wsWeatherDataPcdSurface &surface = _collectionSurfaces[row];

  QString powValueStr =
      surface.interpolationMethod == WS_SURFACE_INTERPOLATION_POLYNOMIAL ?
      degreeCmb->currentText() : powValueEdit->text();

  surface.attributeName       = attributeCmb->currentText().toStdString();
  surface.gridOutputName      = outputGridEdit->text().toStdString();
  surface.interpolationMethod = (wsSurfaceInterpolationMethod) interpCmb->currentIndex();
  surface.numberOfNeighbors   = neighborNumEdit->text().toInt();
  surface.powValue            = powValueStr.toInt();
  surface.x1                  = x1Edit->text().toDouble();
  surface.y1                  = y1Edit->text().toDouble();
  surface.x2                  = x2Edit->text().toDouble();
  surface.y2                  = y2Edit->text().toDouble();
  surface.coordUnit           = resDegreeRadio->isChecked() ? WS_COORD_DECIMAL_DEGREES : WS_COORD_METERS;
  surface.resX                = resXEdit->text().toDouble();
  surface.resY                = resYEdit->text().toDouble();
}

//! Slot chamado depois de editar o nome da grade de saída
void SurfacesConfigDialog::trimGridOutputName()
{
  int row = surfaceList->currentRow();

  if(row == -1)
    return;

  wsWeatherDataPcdSurface &surface = _collectionSurfaces[row];

  surface.gridOutputName = QString::fromStdString(surface.gridOutputName).trimmed().toStdString();
}

//! Slot chamado quando a lista de superfícies precisa ser atualizada
void SurfacesConfigDialog::updateSurfaceList()
{
  updateSurfaceList(surfaceList->currentRow());
}

//! Atualiza a superfície especificada
void SurfacesConfigDialog::updateSurfaceList(int row)
{
  surfaceList->clear();

  if(_collectionSurfaces.isEmpty())
    return;

  // Carrega a lista de atributos
  foreach(wsWeatherDataPcdSurface surface, _collectionSurfaces)
  {
    int row = surfaceList->count();
    surfaceList->addItem(QString::fromStdString(surface.gridOutputName));
    surfaceList->item(row)->setIcon(QIcon(":/data/icons/surface.png"));
  }

  if(row != -1)
    surfaceList->setCurrentRow(row);
  else
    surfaceList->setCurrentRow(0);
}

//! Slot chamado quando os detalhes da superfície publicada precisam ser atualizados
void SurfacesConfigDialog::showSurfaceDetails(int row)
{
  if(row == -1)
    return;

  wsWeatherDataPcdSurface surface = _collectionSurfaces.at(row);

  outputGridEdit->setText(QString::fromStdString(surface.gridOutputName));
  int attributeIndex = attributeCmb->findText(QString::fromStdString(surface.attributeName));
  if(attributeIndex != -1)
  {
    attributeCmb->setEditable(false);
    attributeCmb->setCurrentIndex(attributeIndex);
    attributeCmb->setEnabled(true);
  }
  else
  {
    attributeCmb->setEditable(true);
    attributeCmb->setEditText(QString::fromStdString(surface.attributeName));
    attributeCmb->setEnabled(false);
  }
  interpCmb->setCurrentIndex((int)surface.interpolationMethod);
  neighborNumEdit->setText(QString::number(surface.numberOfNeighbors));
  powValueEdit->setText(QString::number(surface.powValue));
  if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_NN_METHOD)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(false);
    neighborNumEdit->setVisible(false);
    powValueLbl->setVisible(false);
    powValueEdit->setVisible(false);
  }
  else if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_AVERAGE)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(true);
    neighborNumEdit->setVisible(true);
    powValueLbl->setVisible(false);
    powValueEdit->setVisible(false);
  }
  else if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_WEIGHT_AVERAGE)
  {
    stackedWidget->setCurrentIndex(0);

    neighborNumLbl->setVisible(true);
    neighborNumEdit->setVisible(true);
    powValueLbl->setVisible(true);
    powValueEdit->setVisible(true);
  }
  else if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_POLYNOMIAL)
  {
    stackedWidget->setCurrentIndex(1);

    degreeCmb->setCurrentIndex(degreeCmb->findText(QString::number(surface.powValue)));
  }

  x1Edit->setText(QString::number(surface.x1));
  y1Edit->setText(QString::number(surface.y1));
  x2Edit->setText(QString::number(surface.x2));
  y2Edit->setText(QString::number(surface.y2));
  resMeterRadio->setChecked(surface.coordUnit == WS_COORD_METERS);
  resDegreeRadio->setChecked(surface.coordUnit == WS_COORD_DECIMAL_DEGREES);
  resXEdit->setText(QString::number(surface.resX));
  resYEdit->setText(QString::number(surface.resY));
}

//! Carrega a lista dos atributos disponiveis
void SurfacesConfigDialog::loadAttributeList()
{
  if(_attributeListIsLoaded)
    return;

  // Insere os atributos disponíveis no combobox
  attributeCmb->clear();
  foreach(std::string attribute, _collectionAttributes)
    attributeCmb->addItem(QString::fromStdString(attribute));

  _attributeListIsLoaded = true;
}

//! Nenhuma superfície a ser mostrada
void SurfacesConfigDialog::clearSurfaceDetails()
{
  outputGridEdit->clear();
  attributeCmb->clear();
  interpCmb->setCurrentIndex(0);
  stackedWidget->setCurrentIndex(0);
  neighborNumLbl->setVisible(false);
  neighborNumEdit->setVisible(false);
  powValueLbl->setVisible(false);
  powValueEdit->setVisible(false);
  x1Edit->clear();
  y1Edit->clear();
  x2Edit->clear();
  y2Edit->clear();
  resMeterRadio->setChecked(false);
  resDegreeRadio->setChecked(false);
  resXEdit->clear();
  resYEdit->clear();

  enableWidgets(false);

  _attributeListIsLoaded = false;
}

//! Abilitar ou desabilitar os widgets
void SurfacesConfigDialog::enableWidgets(bool enable)
{
  outputGridEdit->setEnabled(enable);
  fileAnaGridMaskBtn->setEnabled(enable);
  attributeCmb->setEnabled(enable);
  interpCmb->setEnabled(enable);
  neighborNumEdit->setEnabled(enable);
  powValueEdit->setEnabled(enable);
  x1Edit->setEnabled(enable);
  y1Edit->setEnabled(enable);
  x2Edit->setEnabled(enable);
  y2Edit->setEnabled(enable);
  resMeterRadio->setEnabled(enable);
  resDegreeRadio->setEnabled(enable);
  resXEdit->setEnabled(enable);
  resYEdit->setEnabled(enable);
}

//! Verificar consistencia das superfícies cadastradas
bool SurfacesConfigDialog::verifyDados(QString & errMsg)
{
  for(int row=0; row < _collectionSurfaces.count(); row++)
  {
    if(!verifyDadosAt(row, errMsg))
    {
      surfaceList->item(row)->setIcon(QIcon(":/data/icons/surface_error.png"));
      surfaceList->setCurrentRow(row);
      return false;
    }
  }
  return true;
}

//! Verificar consistencia da superfície especificada
bool SurfacesConfigDialog::verifyDadosAt(int row, QString & errMsg)
{
  wsWeatherDataPcdSurface surface = _collectionSurfaces.at(row);

  if(QString::fromStdString(surface.gridOutputName).isEmpty())
  {
    errMsg = tr("Informar a grade de saída");
    return false;
  }
  else if((surface.interpolationMethod == WS_SURFACE_INTERPOLATION_AVERAGE ||
           surface.interpolationMethod == WS_SURFACE_INTERPOLATION_WEIGHT_AVERAGE) &&
           surface.numberOfNeighbors < 1)
  {
    errMsg = tr("Numero de vizinhos inválido [1-9999]");
    return false;
  }
  else if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_WEIGHT_AVERAGE &&
          surface.powValue < 1)
  {
    errMsg = tr("Exponenciação inválida [1-9999]");
    return false;
  }
  else if(surface.interpolationMethod == WS_SURFACE_INTERPOLATION_POLYNOMIAL &&
          surface.powValue < 2)
  {
    errMsg = tr("Grau do polinômio inválido");
    return false;
  }
  else if(surface.x2 - surface.x1 < TeMINFLOAT)
  {
    errMsg = tr("x2 deve ser estritamente maior que x1");
    return false;
  }
  else if(surface.y2 - surface.y1 < TeMINFLOAT)
  {
    errMsg = tr("y2 deve ser estritamente maior que y1");
    return false;
  }
  else if(surface.resX < TeMINFLOAT || surface.resY < TeMINFLOAT)
  {
    errMsg = tr("Resoluções devem ser estritamente positivas");
    return false;
  }
  return true;
}

//! Salvar os dados e fechar o dialogo se os dados foram verificados com sucesso
void SurfacesConfigDialog::checkAndAccept()
{
  QString errMsg;
  if(!verifyDados(errMsg))
  {
    QMessageBox::warning(this, tr("Erro validando dados..."), errMsg);
    return;
  }

  // Dados validos
  accept();
}

//! Fecha o dialogo avisando o usuario das eventuais modificações não salvas
void SurfacesConfigDialog::checkAndReject()
{
  if(configChanged())
  {
    // Dados modificados.  Pergunta ao usuário se deseja salvar
    // alterações, abandonar alterações ou cancelar a seleção de
    // nova regra
    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(_parent, tr("Modificações não salvas!"),
                                   "As alterações efetuadas na tela de configuração de superfícies "
                                   "ainda não foram salvas. Deseja salvar as alterações?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                   QMessageBox::Save);

    if(answer == QMessageBox::Cancel)
      return;

    if(answer == QMessageBox::Discard)
      reject();

    else // save
      checkAndAccept();
  }
  else
    reject();
}

//! Verifica se houve alterações nas superfícies cadastradas
bool SurfacesConfigDialog::configChanged()
{
  if(_collectionSurfaces.size() !=_startConfig.size())
    return true;

  for(int row = 0; row < _collectionSurfaces.count(); row++)
  {
    wsWeatherDataPcdSurface collectionSurface  = _collectionSurfaces.at(row);
    wsWeatherDataPcdSurface startConfigSurface = _startConfig.at(row);

    bool changed =
        startConfigSurface.attributeName       != collectionSurface.attributeName ||
        startConfigSurface.gridOutputName      != collectionSurface.gridOutputName ||
        startConfigSurface.interpolationMethod != collectionSurface.interpolationMethod ||
        startConfigSurface.numberOfNeighbors   != collectionSurface.numberOfNeighbors ||
        startConfigSurface.powValue            != collectionSurface.powValue ||
        startConfigSurface.x1                  != collectionSurface.x1 ||
        startConfigSurface.y1                  != collectionSurface.y1 ||
        startConfigSurface.x2                  != collectionSurface.x2 ||
        startConfigSurface.y2                  != collectionSurface.y2 ||
        startConfigSurface.coordUnit           != collectionSurface.coordUnit ||
        startConfigSurface.resX                != collectionSurface.resX ||
        startConfigSurface.resY                != collectionSurface.resY;

    if(changed)
      return true;
  }

  return false;
}
