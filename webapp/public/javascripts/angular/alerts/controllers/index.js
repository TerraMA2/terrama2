define([
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/alerts/controllers/alert-list",
  "TerraMA2WebApp/alerts/controllers/alert-register-update"
], function(){
  var moduleName = "terrama2.alerts.controllers";

  angular.module(moduleName, [commonApp, ListController, RegisterUpdateController])
    .controller("AlertListController", ListController)
    .controller("AlertRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});