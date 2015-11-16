#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// Forward declaration
class QTreeWidgetItem;
class QNetworkAccessManager;

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

  private:
    //! It validates the connection type and stores the URI value in uri_ member
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

    void onConnectionTypeChanged(int);

    void onAddressFileBtnClicked();

  private:
    QString dataProviderSelected_; //!< Value used for detects whether it is new dataprovider or not
    QString uri_; //!< Value used for store uri parsed value

    QNetworkAccessManager* manager_;
};

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
