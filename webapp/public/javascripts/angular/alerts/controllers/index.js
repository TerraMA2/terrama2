define([
  "TerraMA2WebApp/alerts/controllers/alert-list",
  "TerraMA2WebApp/alerts/controllers/alert-register-update"
], function(ListController, RegisterUpdateController){
  var moduleName = "terrama2.alerts.controllers";

  angular.module(moduleName, [ListController, RegisterUpdateController])
    .controller("AlertListController", ListController)
    .controller("AlertRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});