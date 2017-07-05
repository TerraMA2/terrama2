define([
  "TerraMA2WebApp/table/table",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/legends/services/index",
  "TerraMA2WebApp/projects/controllers/list",
  "TerraMA2WebApp/projects/controllers/registration"
], function(tableModule, alertBoxModule, analysisServices, legendsServices, ListControler, RegisterController) {
  var moduleName = "terrama2.projects.controllers";

  angular.module(moduleName, [alertBoxModule, tableModule, analysisServices, legendsServices])
    .controller("ProjectListController", ListControler)
    .controller("ProjectRegisterController", RegisterController);

  return moduleName;
})