define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/collapser/directives",
  "TerraMA2WebApp/services/registration",
  "TerraMA2WebApp/services/service"
], function(commonServiceApp, servicesApp, messageboxApp, collapserApp, RegisterController, ListController) {
  var moduleName = "terrama2.adminstration.controllers";

  angular.module(moduleName, [
      commonServiceApp,
      servicesApp,
      messageboxApp,
      collapserApp
    ])
    .controller("ServiceRegisterUpdate", RegisterController)
    .controller("ServiceListController", ListController);

  return moduleName;
});