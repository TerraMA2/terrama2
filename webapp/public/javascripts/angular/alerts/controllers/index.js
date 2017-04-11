define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/alerts/controllers/alert-list",
  "TerraMA2WebApp/alerts/controllers/alert-register-update"
], function(moduleLoader, commonApp, messageBoxApp, dataSeriesServiceApp, serviceApp, ListController, RegisterUpdateController){
  var moduleName = "terrama2.alerts.controllers";

  var deps = [messageBoxApp];
  moduleLoader("ui.select", deps);
  moduleLoader(dataSeriesServiceApp, deps);
  moduleLoader(serviceApp, deps);

  angular.module(moduleName, deps)
    .controller("AlertListController", ListController)
    .controller("AlertRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});