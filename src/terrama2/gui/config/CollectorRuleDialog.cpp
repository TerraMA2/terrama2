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
  \file terrama2/gui/config/CollectorRuleDialog.cpp

  \brief Definition of methods in class CollectorRuleDialog.hpp

  \author Raphael Willian da Costa
  \author Pedro Luchini de Moraes
*/

// TerraMA2
#include "ui_CollectorRuleDialogForm.h"
#include "CollectorRuleDialog.hpp"
#include "LuaSyntaxHighlighter.hpp"

// QT
#include <QAction>
#include <QMenu>
#include <QMessageBox>

struct CollectorRuleDialog::Impl
{
  Impl()
    : ui_(new Ui::CollectorRuleForm)
  {
  }

  ~Impl()
  {
    delete ui_;
  }

  Ui::CollectorRuleForm* ui_;
};

CollectorRuleDialog::CollectorRuleDialog(QWidget* parent, Qt::WindowFlags f)
: QDialog(parent, f), pimpl_(new Impl), luaHighlighter_(nullptr)
{
  pimpl_->ui_->setupUi(this);

  pimpl_->ui_->okBtn->setEnabled(true);

  connect(pimpl_->ui_->okBtn, SIGNAL(clicked()), SLOT(onOkBtnClicked()));
  connect(pimpl_->ui_->cancelBtn, SIGNAL(clicked()), SLOT(reject()));

  luaHighlighter_.reset(new LuaSyntaxHighlighter(pimpl_->ui_->txtLuaScript));

  init();

}

CollectorRuleDialog::~CollectorRuleDialog()
{
  delete pimpl_;
}

void CollectorRuleDialog::fillGUI(const QString script)
{
  QString s(script);
  pimpl_->ui_->txtLuaScript->setText(s);
}

void CollectorRuleDialog::fillObject(QString &script)
{
  script = pimpl_->ui_->txtLuaScript->toPlainText();
}

void CollectorRuleDialog::init()
{
  QMenu* funcOpMenu = new QMenu(tr("Common Functions"), this);
  funcOpMenu->addAction(tr("print"))->setProperty("added_text", "print(_valores_)");
  funcOpMenu->addAction(tr("tonumber"))->setProperty("added_text", "tonumber(_valor_)");
  funcOpMenu->addAction(tr("tostring"))->setProperty("added_text", "tostring(_valor_)");
  funcOpMenu->addAction(tr("type"))->setProperty("added_text", "type(_valor_)");
  funcOpMenu->addSeparator();
  connect(funcOpMenu, SIGNAL(triggered(QAction*)), SLOT(onAddScript(QAction*)));

  QMenu* mathMenu = new QMenu(tr("Math Functions"), this);
  mathMenu->addAction(tr("abs"  ))->setProperty("added_text", "math.abs(_numero_)");
  mathMenu->addAction(tr("ceil" ))->setProperty("added_text", "math.ceil(_numero_)");
  mathMenu->addAction(tr("exp"  ))->setProperty("added_text", "math.exp(_numero_)");
  mathMenu->addAction(tr("floor"))->setProperty("added_text", "math.floor(_numero_)");
  mathMenu->addAction(tr("log"  ))->setProperty("added_text", "math.log(_numero_)");
  mathMenu->addAction(tr("log10"))->setProperty("added_text", "math.log10(_numero_)");
  mathMenu->addAction(tr("max"  ))->setProperty("added_text", "math.max(_lista_de_numeros_)");
  mathMenu->addAction(tr("min"  ))->setProperty("added_text", "math.min(_lista_de_numeros_)");
  mathMenu->addAction(tr("modf" ))->setProperty("added_text", "math.modf(_numero_)");
  mathMenu->addAction(tr("pow"  ))->setProperty("added_text", "math.pow(_numero_, _numero_)");
  mathMenu->addAction(tr("sqrt "))->setProperty("added_text", "math.sqrt(_numero_)");
  mathMenu->addSeparator();

  QMenu* trigMenu = new QMenu(tr("Trigonometric"), this);
  trigMenu->addAction(tr("acos" ))->setProperty("added_text", "math.acos(_numero_)");
  trigMenu->addAction(tr("asin" ))->setProperty("added_text", "math.asin(_numero_)");
  trigMenu->addAction(tr("atan" ))->setProperty("added_text", "math.atan(_numero_)");
  trigMenu->addAction(tr("atan2"))->setProperty("added_text", "math.atan2(_numero_, _numero_)");
  trigMenu->addAction(tr("cos"  ))->setProperty("added_text", "math.cos(_numero_em_radianos_)");
  trigMenu->addAction(tr("deg"  ))->setProperty("added_text", "math.deg(_numero_em_radianos_)");
  trigMenu->addAction(tr("pi"   ))->setProperty("added_text", "math.pi");
  trigMenu->addAction(tr("rad"  ))->setProperty("added_text", "math.rad(_numero_em_graus_)");
  trigMenu->addAction(tr("sin"  ))->setProperty("added_text", "math.sin(_numero_em_radianos_)");
  trigMenu->addAction(tr("tan"  ))->setProperty("added_text", "math.tan(_numero_em_radianos_)");

  QMenu* hiperMenu = new QMenu(tr("Hyperbolic"), this);
  hiperMenu->addAction(tr("cosh" ))->setProperty("added_text", "math.cosh(_numero_)");
  hiperMenu->addAction(tr("sinh" ))->setProperty("added_text", "math.sinh(_numero_)");
  hiperMenu->addAction(tr("tanh" ))->setProperty("added_text", "math.tanh(_numero_)");

  mathMenu->addMenu(trigMenu);
  mathMenu->addMenu(hiperMenu);
  funcOpMenu->addMenu(mathMenu);
  
  pimpl_->ui_->functionsBtn->setMenu(funcOpMenu);
  pimpl_->ui_->functionsBtn->setPopupMode(QToolButton::InstantPopup);

  QMenu* luaCommands = new QMenu(tr("Comandos"), this);
  luaCommands->addAction(tr("if then end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nend\n");
  luaCommands->addAction(tr("if then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  luaCommands->addAction(tr("if then elseif then else end"))->setProperty("added_text", "if _condicao_ then\n  _comandos_\nelseif _condicao_ then\n  _comandos_\nelse\n  _comandos_\nend\n");
  luaCommands->addSeparator();
  luaCommands->addAction(tr("and"))->setProperty("added_text", "(_condicao_ and _condicao_)");
  luaCommands->addAction(tr("or"))->setProperty("added_text", "(_condicao_ or _condicao_)");

  connect(luaCommands, SIGNAL(triggered(QAction*)), SLOT(onAddScript(QAction*)));

  pimpl_->ui_->commandsBtn->setMenu(luaCommands);
  pimpl_->ui_->commandsBtn->setPopupMode(QToolButton::InstantPopup);
}

void CollectorRuleDialog::onAddScript(QAction* action)
{
  QString value = action->property("added_text").toString();

  pimpl_->ui_->txtLuaScript->insertPlainText(value);
  pimpl_->ui_->txtLuaScript->setFocus();
}

void CollectorRuleDialog::onOkBtnClicked()
{
  if (pimpl_->ui_->txtLuaScript->toPlainText().trimmed().isEmpty())
  {
    QMessageBox::warning(this, tr("TerraMA2 Collector Rule Error"), tr("The lua script cannot be empty"));
    return;
  }
  accept();
}
