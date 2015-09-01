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
  \file terrama2/gui/config/AlertIconDialog.cpp

  \brief Definition of methods in class AlertIconDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Fred
*/

// TerraMA2 
#include "AlertIconDialog.hpp"
#include "alertIconSetDialog.hpp"
#include "AlertIconList.hpp"
#include "utils.h"
#include "Services.hpp"

// QT
#include <QMessageBox>

AlertIconDialog::AlertIconDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f), _parent(parent)
{
  setupUi(this);

  QPixmap pixmap(10, 10);

  // It has not pixelmap at level 0
  label_lvl_0_text->setText(Utils::warningLevelToString(0));

  pixmap.fill(QColor(0, 0, 255));
  label_lvl_1_icon->setPixmap(pixmap);
  label_lvl_1_text->setText(Utils::warningLevelToString(1));

  pixmap.fill(QColor(255, 255, 0));
  label_lvl_2_icon->setPixmap(pixmap);
  label_lvl_2_text->setText(Utils::warningLevelToString(2));

  pixmap.fill(QColor(255, 127, 0));
  label_lvl_3_icon->setPixmap(pixmap);
  label_lvl_3_text->setText(Utils::warningLevelToString(3));

  pixmap.fill(QColor(255, 0, 0));
  label_lvl_4_icon->setPixmap(pixmap);
  label_lvl_4_text->setText(Utils::warningLevelToString(4));

  connect(okBtn,          SIGNAL(clicked()), SLOT(accept()));
  connect(cancelBtn,      SIGNAL(clicked()), SLOT(checkAndReject()));

  connect(imageSetCfgBtn, SIGNAL(clicked()), SLOT(configImageSet()));

  connect(lvl_0_cmb,      SIGNAL(currentIndexChanged(int)), SLOT(updateAlertIDs()));
  connect(lvl_1_cmb,      SIGNAL(currentIndexChanged(int)), SLOT(updateAlertIDs()));
  connect(lvl_2_cmb,      SIGNAL(currentIndexChanged(int)), SLOT(updateAlertIDs()));
  connect(lvl_3_cmb,      SIGNAL(currentIndexChanged(int)), SLOT(updateAlertIDs()));
  connect(lvl_4_cmb,      SIGNAL(currentIndexChanged(int)), SLOT(updateAlertIDs()));
}

//! Initialize the dialog
void AlertIconDialog::setFields(AlertIconList *alertIconList, struct wsAlertIcons & alertIcons)
{
  _alertIconList = alertIconList;
  _alertIcons    = alertIcons;

  // Update the combos
  loadCombos();

  // It might to happen when an storaged icon has been removed from image set
  // In this way, we must to consolidate the data
  if(configChanged())
    alertIcons = _alertIcons;

  _startConfig = _alertIcons;
}

//! Retrieve the dialog result
void AlertIconDialog::getFields(struct wsAlertIcons & alertIcons, bool& changed)
{
  changed = configChanged();

  alertIcons = _alertIcons;
}

//! Update the combos
void AlertIconDialog::loadCombos()
{
  lvl_0_cmb->blockSignals(true);
  lvl_1_cmb->blockSignals(true);
  lvl_2_cmb->blockSignals(true);
  lvl_3_cmb->blockSignals(true);
  lvl_4_cmb->blockSignals(true);

  lvl_0_cmb->clear();
  lvl_1_cmb->clear();
  lvl_2_cmb->clear();
  lvl_3_cmb->clear();
  lvl_4_cmb->clear();

  lvl_0_cmb->addItem(tr("Nenhum"));
  lvl_1_cmb->addItem(tr("Nenhum"));
  lvl_2_cmb->addItem(tr("Nenhum"));
  lvl_3_cmb->addItem(tr("Nenhum"));
  lvl_4_cmb->addItem(tr("Nenhum"));

  foreach(AlertIcon *icon, (QList<AlertIcon *>) *_alertIconList)
  {
    lvl_0_cmb->addItem(QIcon(icon->pixmap()), "");
    lvl_1_cmb->addItem(QIcon(icon->pixmap()), "");
    lvl_2_cmb->addItem(QIcon(icon->pixmap()), "");
    lvl_3_cmb->addItem(QIcon(icon->pixmap()), "");
    lvl_4_cmb->addItem(QIcon(icon->pixmap()), "");
  }

  // show alert icons in widgets
  lvl_0_cmb->setCurrentIndex(_alertIconList->findIconIndex(_alertIcons.imageLevel0)+1);
  lvl_1_cmb->setCurrentIndex(_alertIconList->findIconIndex(_alertIcons.imageLevel1)+1);
  lvl_2_cmb->setCurrentIndex(_alertIconList->findIconIndex(_alertIcons.imageLevel2)+1);
  lvl_3_cmb->setCurrentIndex(_alertIconList->findIconIndex(_alertIcons.imageLevel3)+1);
  lvl_4_cmb->setCurrentIndex(_alertIconList->findIconIndex(_alertIcons.imageLevel4)+1);

  lvl_0_cmb->blockSignals(false);
  lvl_1_cmb->blockSignals(false);
  lvl_2_cmb->blockSignals(false);
  lvl_3_cmb->blockSignals(false);
  lvl_4_cmb->blockSignals(false);

  //Se o indice do combo for 0 e o campo de_alertIcons for diferente de 0,
  //significa que a imagem foi apagada. O botao cancelar deve ser desativado, e os dados
  //de alerta consolidados na proxima vez que sera executado setFields.
  cancelBtn->setEnabled((lvl_0_cmb->currentIndex() > 0 || _alertIcons.imageLevel0 == 0) &&
                        (lvl_1_cmb->currentIndex() > 0 || _alertIcons.imageLevel1 == 0) &&
                        (lvl_2_cmb->currentIndex() > 0 || _alertIcons.imageLevel2 == 0) &&
                        (lvl_3_cmb->currentIndex() > 0 || _alertIcons.imageLevel3 == 0) &&
                        (lvl_4_cmb->currentIndex() > 0 || _alertIcons.imageLevel4 == 0));

  _alertIcons.imageLevel0 = lvl_0_cmb->currentIndex() > 0 ? _alertIcons.imageLevel0 : 0;
  _alertIcons.imageLevel1 = lvl_1_cmb->currentIndex() > 0 ? _alertIcons.imageLevel1 : 0;
  _alertIcons.imageLevel2 = lvl_2_cmb->currentIndex() > 0 ? _alertIcons.imageLevel2 : 0;
  _alertIcons.imageLevel3 = lvl_3_cmb->currentIndex() > 0 ? _alertIcons.imageLevel3 : 0;
  _alertIcons.imageLevel4 = lvl_4_cmb->currentIndex() > 0 ? _alertIcons.imageLevel4 : 0;
}

//! Update the combos
void AlertIconDialog::updateAlertIDs()
{
  int index;

  _alertIcons.imageLevel0 = (index = lvl_0_cmb->currentIndex()) > 0 ? _alertIconList->at(index-1)->id() : 0;
  _alertIcons.imageLevel1 = (index = lvl_1_cmb->currentIndex()) > 0 ? _alertIconList->at(index-1)->id() : 0;
  _alertIcons.imageLevel2 = (index = lvl_2_cmb->currentIndex()) > 0 ? _alertIconList->at(index-1)->id() : 0;
  _alertIcons.imageLevel3 = (index = lvl_3_cmb->currentIndex()) > 0 ? _alertIconList->at(index-1)->id() : 0;
  _alertIcons.imageLevel4 = (index = lvl_4_cmb->currentIndex()) > 0 ? _alertIconList->at(index-1)->id() : 0;
}

//! Check if had changes in alert icons
bool AlertIconDialog::configChanged()
{
  return
      (_alertIcons.imageLevel0 != _startConfig.imageLevel0) ||
      (_alertIcons.imageLevel1 != _startConfig.imageLevel1) ||
      (_alertIcons.imageLevel2 != _startConfig.imageLevel2) ||
      (_alertIcons.imageLevel3 != _startConfig.imageLevel3) ||
      (_alertIcons.imageLevel4 != _startConfig.imageLevel4);
}

void AlertIconDialog::configImageSet()
{
  AlertIconSetDlg dlg;

  dlg.setFields(_alertIconList);
  if (dlg.exec()== QDialog::Accepted)		//	QDialog::Rejected
  {
    bool changed = false;

    dlg.getFields(changed);

    if (changed)
      loadCombos();
  }
}

//! Close the dialog notifying the user if have any changes not saved
void AlertIconDialog::checkAndReject()
{
  if(configChanged())
  {
    // There are modifications. Ask to user if would like to save modifications, leave
    // or just cancel the new selection role

    QMessageBox::StandardButton answer;
    answer = QMessageBox::question(_parent, tr("Modificações não salvas!"),
                                   "As alterações efetuadas na tela de configuração "
                                   "ainda não foram salvas. Deseja salvar as alterações?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                   QMessageBox::Save);

    if(answer == QMessageBox::Cancel)
      return;

    if(answer == QMessageBox::Discard)
      reject();

    else // save
      accept();
  }
  else
    reject();
}
