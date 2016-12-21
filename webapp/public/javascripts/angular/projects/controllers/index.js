define([
  "TerraMA2WebApp/table/table",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/projects/controllers/list",
  "TerraMA2WebApp/projects/controllers/registration"
], function(tableModule, alertBoxModule, ListControler, RegisterController) {
  var moduleName = "terrama2.projects.controllers";

  angular.module(moduleName, [alertBoxModule, tableModule])
    .controller("ListController", ListControler)
    .controller("RegisterController", RegisterController);

  return moduleName;
})