define([
  "TerraMA2WebApp/views/controllers/view-list",
  "TerraMA2WebApp/views/controllers/view-register-update"
], function(ListController, RegisterUpdateController) {
  var moduleName = "terrama2.views.controllers";

  angular.module(moduleName, [])
    .controller("ListController", ListController)
    .controller("RegisterUpdateController", RegisterUpdateController);

  return moduleName;
})