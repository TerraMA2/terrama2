#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"


class ConfigAppWeatherServer : public ConfigAppTab {
  Q_OBJECT
  public:
    ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui);
    ~ConfigAppWeatherServer();

    void load();
    void save();
    void discardChanges(bool restore);
    bool validate();
    bool dataChanged();

  private:
    void validateConnection();

  private slots:
    void onServerTabRequested();
    void onServerEdited();
    //! Slot for handling if it is valid connection. TODO: ftp
    void onCheckConnectionClicked();
};

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
