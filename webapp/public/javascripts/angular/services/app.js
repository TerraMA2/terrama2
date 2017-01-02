define([
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/services/controllers"
], function(servicesApp, controllerApp) {
  var moduleName = "terrama2.administration.services";

  angular.module(moduleName, [servicesApp, controllerApp]);

  return moduleName;
})