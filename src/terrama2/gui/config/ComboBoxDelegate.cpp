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
  \file terrama2/gui/config/ComBoxDelegate.cpp

  \brief Definition of Class ComBoxDelegate.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  
*/

// TerraMA2
#include "ComboBoxDelegate.hpp"

// QT  
#include <QComboBox>
#include <QStandardItem>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
: QItemDelegate(parent)
{
} 

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
									  const QStyleOptionViewItem &/* option */,
									  const QModelIndex &/* index */) const
{
	QComboBox *editor = new QComboBox(parent);
	for (int i = 0; i < _items.size(); i++)
	{
		if(_items[i]->icon().isNull())
			editor->addItem(_items[i]->text());
		else
			editor->addItem(_items[i]->icon(), _items[i]->text());
	}
	return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
									const QModelIndex &index) const
{
	int comboIndex = index.model()->data(index, Qt::UserRole).toInt();

	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	comboBox->setCurrentIndex(comboIndex);
} 

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
								   const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	QVariant value = comboBox->currentIndex();
	QString display = comboBox->currentText();
	QIcon icon = comboBox->itemIcon(comboBox->currentIndex());

	model->setData(index, value, Qt::UserRole);
	model->setData(index, display, Qt::EditRole);
	model->setData(index, icon, Qt::DecorationRole);
} 

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
											const QStyleOptionViewItem &option, 
											const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
} 

void ComboBoxDelegate::insertItem(QString itemName)
{
	_items.push_back(new QStandardItem(itemName));
}

void ComboBoxDelegate::insertItem(QPixmap itemPixmap, QString itemName)
{
	_items.push_back(new QStandardItem(itemPixmap, itemName));
}