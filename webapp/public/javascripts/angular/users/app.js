define([
  "TerraMA2WebApp/users/services",
  "TerraMA2WebApp/users/controllers"
], function(servicesApp, controllersApp) {
  var moduleName = "terrama2.users";

  angular.module(moduleName, [servicesApp, controllersApp]);

  return moduleName;
});