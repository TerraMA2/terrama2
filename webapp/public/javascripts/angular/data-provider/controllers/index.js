define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/data-provider/controllers/list",
  "TerraMA2WebApp/data-provider/controllers/registration"
], function(commonServiceModule, alertBoxModule, ListController, RegisterController) {
  var moduleName = "terrama2.providers.controllers";

  var deps = [commonServiceModule, alertBoxModule];

  // checking schema form dependency
  try {
    angular.module("schemaForm");
    deps.unshift("schemaForm");
  } catch (e) {
    // schemaForm not found
  }

  angular.module(moduleName, deps)
    .controller("DataProviderListController", ListController)
    .controller("DataProviderRegisterController", RegisterController);

  return moduleName;
});