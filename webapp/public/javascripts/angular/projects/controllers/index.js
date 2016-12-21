define([
  "TerraMA2WebApp/projects/controllers/list",
  "TerraMA2WebApp/projects/controllers/registration"
], function(ListControler, RegisterController) {
  var moduleName = "terrama2.projects.controllers";

  angular.module(moduleName, [])
    .controller("ListControler", ListControler)
    .controller("RegisterController", RegisterController);

  return moduleName;
})