define([
  "TerraMA2WebApp/ace/directive",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/views/controllers/view-list",
  "TerraMA2WebApp/views/controllers/view-register-update"
], function(aceModule, alertBoxModule, serviceModule, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.views.controllers";

  var deps = [alertBoxModule, serviceModule];
  try {
    angular.module(aceModule);
    deps.unshift(aceModule);
  } catch (err) {

  } finally {
    try {
      angular.module("color.picker");
      deps.push("color.picker");
    } catch (e) {

    }
  }

  angular.module(moduleName, deps)
    .controller("ListController", ListController)
    .controller("RegisterUpdateController", RegisterUpdateController);

  return moduleName;
})