#ifndef __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPCOLLECTTAB_HPP__
#define __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPCOLLECTTAB_HPP__

// TerraMA2
#include "AdminAppTab.hpp"
 
class AdminAppCollectTab: public AdminAppTab
{
 public:

//! Default constructor
  AdminAppCollectTab(AdminApp* app, Ui::AdminAppForm* ui);

 //! Destructor
  ~AdminAppCollectTab();

  bool validate();
  bool load();
  QMap<QString, QJsonObject> toJson();

};

#endif __TERRAMA2_INTERNAL_GUI_ADMIN_ADMINAPPCOLLECTTAB_HPP__