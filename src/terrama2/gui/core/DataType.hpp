#include <QString>

namespace terrama2
{
  namespace gui
  {
    namespace core
    {
      /*!
        \brief A struct for stores common values like service params.
      */
      struct CommonData
      {
          int servicePort_; //!< A int value to store specific TerraMA2 service port.
          QString address_; //!< A string value to define remote connection address.
          QString logFile_; //!< A string value to define where is the service log file.
          QString cmd_;     //!< A string value to define a command to execute TerraMA2 service.
          QString params_;  //!< A string value to store custom service parameters.
      };

      /*!
       \brief A struct for handling database connections parameters

        It is used to store TerraMA2 administration database tab values.
      */
      struct Database
      {
          QString host_;     //!< A string value defining database host connection.
          int port_;         //!< A int value to store a databse port connection.
          QString user_;     //!< A string value to store user database connection.
          QString password_; //!< A string value to define the user password.
          QString dbName_;   //!< A string value to define database name to be created.
          QString driver_;   //!< A string value to define which database connection driver.
          QString study_;    //!< A string value to store if the database is study. It is used to save "True" or "False" in json file.
          QString name_;     //!< A string value to store TerraMA2 configuration name
          QString version_;  //!< A string value to store TerraMA2 version
      };

      /*!
        \brief A struct for handling Collection values.
 
        It is used to store TerraMA2 administration collection tab values.
      */
      struct Collection: public CommonData
      {
          int timeout_;     //!< A int value for handling response service timeout retry
          QString dirPath_; //!< A string value to define where is to store the collected files
      };
    }
  }
}
