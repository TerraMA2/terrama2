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
  \file terrama2/services/appserver/mainwidget.h

  \brief Main Widget View Header.

  \author Carolina Galvão dos Santos
 */

 #ifndef __TERRAMA2_SERVICES_APPSERVER_MAINWIDGET_H__
 #define __TERRAMA2_SERVICES_APPSERVER_MAINWIDGET_H__

// TerraMA2
#include "ui_settingswidget.h"

// Qt
#include <QAction>
#include <QWidget>
#include <QProcess>
#include <QMovie>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QLineEdit>
#include <QHostInfo>

namespace Ui {
    class MainWidget;
    class SettingsWidget;
} // end namespace ui

class QProcess;

/*!
 * \brief The MainWidget class is responsible for create the main view
 * that initialize all that the service needs to start or finish.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    //! Class constructor
    explicit MainWidget(QWidget *parent = 0);
    
    //! Class destructor
    ~MainWidget();

private slots:

   /*!
    * \brief Create the system tray icon that is on the system tab bar.
    */
    void setupSystemTrayIcon();

   /*!
    * \brief Create the terrama2 qprocess that run all the scripts and
    * commands needed to configure the service.
    */
    void setupTerraMaProcess();

   /*!
    * \brief Create the settings view that allow users to change some
    * thirdparty paths and completely remove the service.
    */
    void setupSettingsWidget();

   /*!
    * \brief Action when push start button.
    */
    void on_pushButtonStart_clicked();

   /*!
    * \brief Action when push stop button.
    */
    void on_pushButtonStop_clicked();

   /*!
    * \brief Action when push settings button.
    */
    void on_pushButtonSettings_clicked();

   /*!
    * \brief Action when push save button.
    */
    void on_pushButtonSave_clicked();

   /*!
    * \brief Action when push remove button.
    */
    void on_pushButtonRemove_clicked();

   /*!
    * \brief Action when push system tray settings button.
    */
    void on_actionSettings_clicked();

   /*!
    * \brief Action when push system tray quit button.
    */
    void on_actionQuit_clicked();

   /*!
    * \brief Show qprocess outputs in a qplaintext.
    */
    void onOutputReceived();

   /*!
    * \brief Show qprocess error outputs in a qplaintext.
    */
    void onErrorReceived();

   /*!
    * \brief Action after qprocess start a process.
    */
    void onProcStarted();

   /*!
    * \brief Action after qprocess finish a process.
    * \param An integer code that shows the process status.
    */
    void onProcFinished(int code);

    /*!
    * \brief Check TerraMA2 service status.
    * \return An integer, where 0 if running and 1 if not running.
    */
    int checkServiceStatus();

    /*!
    * \brief Check if TerraMA2 service was installed.
    * \return An integer, where 0 if installed and 1 if not installed.
    */
    int checkServiceInstallation();

   /*!
    * \brief Start TerraMA2 service via pm2.
    */
    void startService();

   /*!
    * \brief Stop TerraMA2 service via pm2.
    */
    void stopService();

   /*!
    * \brief Update TerraMA2 service status.
    */
    void updateServiceStatus();

   /*!
    * \brief Save the updates in the thirdparty paths available in the
    * settings view.
    */
    void save3rdpartyPaths();

   /*!
    * \brief Recover the updates in the thirdparty paths available in
    * the settings view.
    */
    void recover3rdpartyPaths();

private:
    Ui::MainWidget *ui;
    Ui::SettingsWidget *uiSettings;
    QWidget *settingsWindow;
    QProcess *terraMA_proc;
    QProcessEnvironment env;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QMovie *loadingAnimation;
    QAction *status;
    QString nodePath;
    QString psqlPath;
    QString scriptPath;
    QString saveFilePath;
    QString path;

};

#endif // __TERRAMA2_SERVICES_APPSERVER_MAINWIDGET_H__
