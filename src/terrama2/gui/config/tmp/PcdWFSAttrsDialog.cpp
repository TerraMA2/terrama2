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
  \file terrama2/gui/config/PcdWFSAttrsDialog.cpp

  \brief Definition of methods in class PcdWFSAttrsDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Gustavo Sampaio
*/

// TerraMA2
#include "PcdWFSAttrsDialog.hpp"
#include "Utils.hpp"

// QT  
#include <QMessageBox>
#include <QMenu>

PcdWFSAttrsDialog::PcdWFSAttrsDialog(std::vector<wsDBColumn> attributes, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
	setupUi(this);

	connect(okBtn,     SIGNAL(clicked()), SLOT(okBtnClicked()));
	connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

	for(int i = 0; i < attributes.size(); ++i)
	{
		wsDBColumn attr = attributes[i];
		QString attributeStr = QString::fromStdString(attr.name + "  (" + Utils::columnTypeToString(attr.type).toStdString() + ")");

		// Exibir o nome e o tipo do atributo, mas guardar referencia para o nome do atributo apenas
		wfsIdAttrCmb->addItem(attributeStr, attr.name.c_str());
		wfsTimestampAttrCmb->addItem(attributeStr, attr.name.c_str());
	}

	// Preencher o menu do botao que auxilia a edicao da mascara temporal.
	// As opcoes de mascara seguem o formato documentado no metodi QDateTime::fromString, 
	// pois eh ele que fara a conversao da string para um objeto QDateTime durante a importacao da PCD.
	QMenu* menuMask = new QMenu(tr("Máscaras"), this);
	menuMask->addAction(tr("d    - dia sem zero à esquerda (1 a 31)"));
	menuMask->addAction(tr("dd   - dia com zero à esquerda (01 a 31)"));
	menuMask->addAction(tr("M    - mês sem zero à esquerda (1 a 12)"));
	menuMask->addAction(tr("MM   - mês com zero à esquerda (01 a 12)"));
	menuMask->addAction(tr("yy   - ano com dois dígitos"));
	menuMask->addAction(tr("yyyy - ano com quatro dígitos"));
	menuMask->addAction(tr("H    - hora sem zero à esquerda (0 a 23)"));
	menuMask->addAction(tr("HH   - hora com zero à esquerda (00 a 23)"));
	menuMask->addAction(tr("m    - minuto sem zero à esquerda (0 a 59)"));
	menuMask->addAction(tr("mm   - minuto com zero à esquerda (00 a 59)"));
	menuMask->addAction(tr("s    - segundo sem zero à esquerda (0 a 59)"));
	menuMask->addAction(tr("ss   - segundo com zero à esquerda (00 a 59)"));

	wfsTimestampMaskToolBtn->setMenu(menuMask);
	wfsTimestampMaskToolBtn->setPopupMode(QToolButton::InstantPopup);

	// Conecta o clique do menu com a acao de incluir a mascara no edit
	connect(menuMask, SIGNAL(triggered(QAction*)), SLOT(menuMaskClicked(QAction*)));
}

PcdWFSAttrsDialog::~PcdWFSAttrsDialog()
{
}

void PcdWFSAttrsDialog::setFields(QString wfsPCDIdAttr, QString wfsPCDTimestampAttr, QString wfsPCDTimestampMask)
{
	setComboBoxIndex(wfsIdAttrCmb, wfsPCDIdAttr);
	setComboBoxIndex(wfsTimestampAttrCmb, wfsPCDTimestampAttr);

	wfsTimestampMaskLed->setText(wfsPCDTimestampMask);
}

void PcdWFSAttrsDialog::setComboBoxIndex(QComboBox* combo, QString value)
{
	if(!combo)
		return;

	for(int i = 0; i < combo->count(); ++i)
	{
		if(combo->itemData(i).toString() == value)
		{
			combo->setCurrentIndex(i);
			break;
		}
	}
}

void PcdWFSAttrsDialog::getFields(QString &wfsPCDIdAttr, QString &wfsPCDTimestampAttr, QString &wfsPCDTimestampMask)
{
	int currentIndex = wfsIdAttrCmb->currentIndex();
	if(wfsIdAttrCmb->currentIndex() >= 0)
		wfsPCDIdAttr = wfsIdAttrCmb->itemData(currentIndex).toString();
	
	currentIndex = wfsTimestampAttrCmb->currentIndex();
	if(wfsTimestampAttrCmb->currentIndex() >= 0)
		wfsPCDTimestampAttr = wfsTimestampAttrCmb->itemData(currentIndex).toString();
	
	wfsPCDTimestampMask = wfsTimestampMaskLed->text();
}

void PcdWFSAttrsDialog::okBtnClicked()
{
	if(wfsIdAttrCmb->currentIndex() == -1)
		QMessageBox::warning(this, tr("Erro..."), tr("Não há atributo (ID) selecionado."));
	else if(wfsTimestampAttrCmb->currentIndex() == -1)
		QMessageBox::warning(this, tr("Erro..."), tr("Não há atributo (Data/Hora) selecionado."));
	else if(wfsTimestampMaskLed->text().isEmpty())
		QMessageBox::warning(this, tr("Erro..."), tr("É necessário definir uma máscara."));
	else
		accept();
}

//! Slot chamado quando o usuario clica no botao de mascara
void PcdWFSAttrsDialog::menuMaskClicked(QAction* actMenu)
{
	// Obter o valor que deve ser adicionado a mascara
	QString selectedMaskText = actMenu->text();
	int separatorIndex = selectedMaskText.indexOf("-");
	QString maskItem = selectedMaskText.left(separatorIndex - 1).trimmed();

	// Adicionar o valor obtido
	wfsTimestampMaskLed->setText(wfsTimestampMaskLed->text() + maskItem);
}