#ifndef __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__

// TerraMA2
#include "ConfigAppTab.hpp"

// Forward declaration
class QTreeWidgetItem;


class ConfigAppWeatherServer : public ConfigAppTab
{

  Q_OBJECT
  public:
    /*!
      @copydoc ConfigAppTab::ConfigAppTab(app, ui);
    */
    ConfigAppWeatherServer(ConfigApp* app, Ui::ConfigAppForm* ui);

    //! Destructor
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

    //! Slot triggered when connection type combobox index has been changed and it displays hidden widgets
    void onConnectionTypeChanged(int);

    /*!
      \brief Slot triggered when connection address button has been clicked. It opens FileDialog
             to select specific folder and fill connection address out
    */
    void onAddressFileBtnClicked();

  private:
    QString uri_; //!< Value used for store parsed uri value
};

#endif //__TERRAMA2_GUI_CONFIG_CONFIGAPPWEATHERSERVER_HPP__
