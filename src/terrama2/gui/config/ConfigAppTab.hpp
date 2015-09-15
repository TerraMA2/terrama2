#ifndef __TERRAMA2_GUI_CONFIG_CONFIGTAB_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGTAB_HPP__

#include "ConfigApp.hpp"

// QT
#include <QString>

// Boost
#include <boost/noncopyable.hpp>


class Services
{

};

class ConfigAppTab : public QObject, private boost::noncopyable
{
  Q_OBJECT

  public:
    //! Constructor
    ConfigAppTab(ConfigApp* app, Ui::ConfigAppForm* ui);

    //! Destructor
    virtual ~ConfigAppTab();

    /*! \brief Loads the read data to interface.
      It always call when active configuration is changed
    */
    virtual void load() = 0;

    //! It shows if data were changed by user
    virtual bool dataChanged() = 0;

    //! Check if current data are valids and it may be saved
    //! This function is called by validateAndSaveChanges to validate display fields
    virtual bool validate(QString& err) = 0;

    //! It saves current data
    //! It tries to save the current data. Return true ou false showing the success of operation
    virtual void save() = 0;

    //! Calcel the editions made at current data
    virtual void discardChanges(bool restore_data) = 0;

    //! It calls the save method inside a try/catch block and check if there are any exception has found
    virtual void validateAndSaveChanges();

    //! It is used for enable/disable the save button along application runtime
    virtual bool verifyAndEnableChange(bool restore);

    virtual QString verifyAndEnableChangeMsg();

  public slots:

    //! Slot triggered on save button. It checks if there any change has made and then call "validateAndSaveChanges"
    virtual void onSaveRequested();

    //! Slot triggered on cancel button to check if the user wish cancel and save.
    virtual void onCancelRequested();

  protected:
    ConfigApp* app_;  //!< Main Window
    Ui::ConfigAppForm* ui_;
    Services* manager_; //!< Services Handler
};

#endif
