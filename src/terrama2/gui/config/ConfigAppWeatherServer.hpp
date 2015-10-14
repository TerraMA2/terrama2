#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// Forward declaration
class QTreeWidgetItem;

class ConfigAppWeatherServer : public ConfigAppTab {
  Q_OBJECT
  public:
    ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui);
    ~ConfigAppWeatherServer();

    //! It is used for connect QTextEdit to allow datachanged
    void load();

    //! Save dataprovider in database
    void save();

    //! Discard changes and restore the widget to the initial state
    void discardChanges(bool restore);

    //! Validates the fields in server insertion
    bool validate();

    //! Sets current dataprovider to the variable
    void setDataProviderSelected(const QString&);

  private:
    //! It validates the connection type
    void validateConnection();

  private slots:
    //! Slot triggered when the user clicks in insertServerbtn to display ServerPage
    void onServerTabRequested();

    //! Slot triggered when some widget is changed.
    void onServerEdited();

    //! Slot triggered for detect if the QTextEdit is changed
    void onTextEditChanged();

    //! Slot for handling if it is valid connection. TODO: ftp
    void onCheckConnectionClicked();

  private:
    QString dataProviderSelected_; //!< Value used for detects whether it is new dataprovider or not
};

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
