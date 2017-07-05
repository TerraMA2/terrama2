define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/legends/controllers/legend-register-update"
], function(moduleLoader, commonApp, messageBoxApp, serviceApp, RegisterUpdateController){
  var moduleName = "terrama2.legends.controllers";

  var deps = [messageBoxApp];
  moduleLoader("ui.select", deps);
  moduleLoader(serviceApp, deps);

  angular.module(moduleName, deps).controller("LegendRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});