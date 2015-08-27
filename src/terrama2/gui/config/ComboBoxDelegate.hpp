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
  \file terrama2/gui/config/ComBoxDelegate.hpp

  \brief Definition of Class ComBoxDelegate.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  
*/

#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

// QT
#include <QItemDelegate>
#include <QStandardItem>

class ComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT

	public:
	ComboBoxDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void insertItem(QPixmap itemPixmap, QString itemName);
	void insertItem(QString itemName);

	private:
		QList<QStandardItem*> _items;
		QStringList _options;

	signals:
		void dataChanged();

}; 
#endif

