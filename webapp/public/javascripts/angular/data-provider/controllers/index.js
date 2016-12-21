define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/data-provider/controllers/list",
  "TerraMA2WebApp/data-provider/controllers/registration"
], function(commonServiceModule, alertBoxModule, ListController, RegisterController) {
  var moduleName = "terrama2.providers.controllers";

  angular.module(moduleName, [commonServiceModule, alertBoxModule])
    .controller("ListController", ListController)
    .controller("RegisterController", RegisterController);

  return moduleName;
});