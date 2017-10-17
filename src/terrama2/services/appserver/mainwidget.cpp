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
  \file terrama2/services/appserver/mainwidget.cpp

  \brief Main Widget View.

  \author Carolina Galvão dos Santos
 */

// TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include "mainwidget.h"
#include "ui_mainwidget.h"

// STL
#include <iostream>
#include <string>
#include <vector>

// Qt
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QWidgetAction>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    uiSettings(new Ui::SettingsWidget)
{
    /* Widget Layout */
    ui->setupUi(this);
    ui->pushButtonSettings->setIcon(QIcon (terrama2::core::FindInTerraMA2Path("share/terrama2/icons/app/settings.png").c_str()));
    ui->pushButtonStart->setIcon(QIcon (terrama2::core::FindInTerraMA2Path("share/terrama2/icons/app/play.png").c_str()));
    ui->pushButtonStop->setIcon(QIcon (terrama2::core::FindInTerraMA2Path("share/terrama2/icons/app/stop.png").c_str()));
    ui->labelLogo->setPixmap(QPixmap (terrama2::core::FindInTerraMA2Path("webmonitor/public/images/terrama2_logo.png").c_str()));
    loadingAnimation = new QMovie(terrama2::core::FindInTerraMA2Path("share/terrama2/icons/app/loading.gif").c_str());
    ui->labelLoading->setMovie(loadingAnimation);

    /* Setups */
    setupTerraMaProcess();
    setupSettingsWidget();
    setupSystemTrayIcon();

    /* Service Status */
    updateServiceStatus();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateServiceStatus()));
    timer->start(5000); // update service status every 5 seconds
}

MainWidget::~MainWidget()
{
    terraMA_proc->close();
    delete ui;
}

/* Setup Actions */

void MainWidget::setupTerraMaProcess()
{
    terraMA_proc = new QProcess(this);
    connect(terraMA_proc, SIGNAL(readyReadStandardError()), this, SLOT(onErrorReceived()));
    connect(terraMA_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(onOutputReceived()));
    connect(terraMA_proc, SIGNAL(finished(int)), this, SLOT(onProcFinished(int)));
    connect(terraMA_proc, SIGNAL(started()), this, SLOT(onProcStarted()));

    /* Declare some system Paths to execute processes */
    scriptPath = terrama2::core::FindInTerraMA2Path("scripts").c_str();
    saveFilePath = scriptPath + "/paths.txt";

    std::ifstream savefile(saveFilePath.toStdString());
    if (!savefile.good())
    {
        nodePath = "/usr/local/Cellar/node/8.4.0/bin";
        psqlPath = "/Applications/Postgres.app/Contents/Versions/9.6/bin";
        save3rdpartyPaths();
    }

    recover3rdpartyPaths();

    path = "/bin:/usr/bin:/usr/local/bin:" + nodePath + ":" + psqlPath;
    if (!env.value("Path").isEmpty())
        path = path + ":" + env.value("Path");

    env = QProcessEnvironment::systemEnvironment();
    env.insert("TERRAMA2_APP_DIR", scriptPath + "/..");
    env.insert("TERRAMA2_STATUS", "");
    env.insert("PATH", path);
    terraMA_proc->setProcessEnvironment(env);
}

void MainWidget::setupSystemTrayIcon()
{
    QAction *m_show_action = new QAction("Settings",this);
    m_show_action->setIconVisibleInMenu(true);
    connect(m_show_action, SIGNAL(triggered()), this, SLOT(on_actionSettings_clicked()));
    QAction *m_quit_action = new QAction("Quit", this);
    m_quit_action->setIconVisibleInMenu(true);
    connect(m_quit_action, SIGNAL(triggered()), this, SLOT(on_actionQuit_clicked()));
    status = new QAction(this);
    status->setEnabled(false);

    trayIconMenu = new QMenu;
    trayIconMenu->addSection("TerraMA2");
    trayIconMenu->addAction(m_show_action);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(m_quit_action);
    trayIconMenu->addAction(status);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon (terrama2::core::FindInTerraMA2Path("webmonitor/public/images/terrama2_logo2.png").c_str()));
    trayIcon->setToolTip(QString("TerraMA2"));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void MainWidget::setupSettingsWidget()
{
    settingsWindow = new QWidget();
    uiSettings->setupUi(settingsWindow);
    uiSettings->lineEditNode->setText(nodePath);
    connect(uiSettings->pushButtonSave, SIGNAL(clicked()), this , SLOT(on_pushButtonSave_clicked()));
    uiSettings->lineEditPsql->setText(psqlPath);
    connect(uiSettings->pushButtonRemove, SIGNAL(clicked()), this , SLOT(on_pushButtonRemove_clicked()));
}

/* Push Button Actions */

void MainWidget::on_pushButtonStart_clicked()
{
    int check = checkServiceInstallation();

    if(check)
    {
        QString postinstPath = terrama2::core::FindInTerraMA2Path("scripts/postinst.sh").c_str();
        terraMA_proc->start(postinstPath);
        if (!terraMA_proc->waitForStarted())
                ui->textOutput->insertPlainText("Cannot connect to service.");
    }
    else
    {
        if(checkServiceStatus())
            startService();
    }
}

void MainWidget::on_pushButtonStop_clicked()
{
    if(!checkServiceStatus())
        stopService();
}

void MainWidget::on_pushButtonSettings_clicked()
{
    settingsWindow->show();
}

void MainWidget::on_pushButtonSave_clicked()
{
    nodePath = uiSettings->lineEditNode->text();
    psqlPath = uiSettings->lineEditPsql->text();
    save3rdpartyPaths();
}

void MainWidget::on_pushButtonRemove_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Remove TerraMA2", "Do you want to remove TerraMA2 service?",
        QMessageBox::Apply | QMessageBox::Cancel);

    if (reply == QMessageBox::Apply)
    {
        settingsWindow->close();
        QString prermPath = terrama2::core::FindInTerraMA2Path("scripts/prerm.sh").c_str();
        terraMA_proc->start(prermPath);
        if (!terraMA_proc->waitForStarted())
            ui->textOutput->insertPlainText("Cannot connect to service.");

    }
    else if(reply == QMessageBox::Cancel)
    {
    }
}

void MainWidget::on_actionSettings_clicked()
{
    settingsWindow->show();
}

void MainWidget::on_actionQuit_clicked()
{
    QApplication::exit();
}

/* Processes Functions */

void MainWidget::onOutputReceived()
{
    QString output = terraMA_proc->readAllStandardOutput();
    ui->textOutput->insertPlainText(output);
}

void MainWidget::onErrorReceived()
{
    QString output = terraMA_proc->readAllStandardError();
    ui->textOutput->insertPlainText(output);
}

void MainWidget::onProcStarted()
{
    loadingAnimation->start();
    ui->labelLoading->setVisible(true);
}

void MainWidget::onProcFinished(int code)
{
    loadingAnimation->stop();
    ui->labelLoading->setVisible(false);
    updateServiceStatus();
}

/* Service Verifications and Actions */

int MainWidget::checkServiceStatus()
{
    env.insert("TERRAMA2_STATUS", "check");
    terraMA_proc->setProcessEnvironment(env);
    return terraMA_proc->execute("sh",QStringList() << "-c" << "curl -s localhost:36000 &>/dev/null && exit 0 || exit 1");
}

int MainWidget::checkServiceInstallation()
{
    QString scriptPath = terrama2::core::FindInTerraMA2Path("scripts/check.sh").c_str();
    return terraMA_proc->execute(scriptPath);
}

void MainWidget::startService()
{
    env.insert("TERRAMA2_STATUS", "start");
    terraMA_proc->setProcessEnvironment(env);
    QString statusScriptPath = terrama2::core::FindInTerraMA2Path("scripts/check.sh").c_str();
    terraMA_proc->start(statusScriptPath);
    if (!terraMA_proc->waitForStarted())
            ui->textOutput->insertPlainText("Cannot connect to service.");
}

void MainWidget::stopService()
{
    env.insert("TERRAMA2_STATUS", "stop");
    terraMA_proc->setProcessEnvironment(env);
    QString statusScriptPath = terrama2::core::FindInTerraMA2Path("scripts/check.sh").c_str();
    terraMA_proc->start(statusScriptPath);
    if (!terraMA_proc->waitForStarted())
            ui->textOutput->insertPlainText("Cannot connect to service.");
}

void MainWidget::updateServiceStatus()
{
    QPixmap statusPixmap;
    QString statusText;

    if(checkServiceStatus())
    {
        if(checkServiceInstallation())
        {
            statusPixmap = QPixmap (terrama2::core::FindInTerraMA2Path("webmonitor/public/images/status/gray_icon.svg").c_str());
            statusText = "Play to Initialize";
        }
        else
        {
            statusPixmap = QPixmap (terrama2::core::FindInTerraMA2Path("webmonitor/public/images/status/red-gray.gif").c_str());
            statusText = "Not Running";
        }
        ui->pushButtonStart->setEnabled(true);
        ui->pushButtonStop->setEnabled(false);
    }
    else
    {
        statusPixmap = QPixmap (terrama2::core::FindInTerraMA2Path("webmonitor/public/images/status/green_icon.svg").c_str());
        statusText = "Running";
        ui->pushButtonStart->setEnabled(false);
        ui->pushButtonStop->setEnabled(true);
    }
    status->setIcon(statusPixmap);
    status->setText(statusText);
    ui->labelStatusIcon->setPixmap(statusPixmap);
    ui->labelStatusText->setText(statusText);
}

/* Extra functions */

void MainWidget::save3rdpartyPaths()
{
    std::ofstream output_file(saveFilePath.toStdString());
    if (output_file.is_open())
      {
        output_file << nodePath.toStdString();
        output_file << "\n";
        output_file << psqlPath.toStdString();
        output_file.close();
      }
}

void MainWidget::recover3rdpartyPaths()
{
    std::vector<std::string> pathsVector;
    std::ifstream input_file(saveFilePath.toStdString());
    std::string tempVar;
    while (input_file >> tempVar)
        pathsVector.push_back(tempVar);

    input_file.close();

    nodePath = QString::fromStdString(pathsVector.at(0));
    psqlPath = QString::fromStdString(pathsVector.at(1));
}
