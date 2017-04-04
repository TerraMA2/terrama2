define([
  "TerraMA2WebApp/alerts/controllers/alert-list"
], function(ListController){
  var moduleName = "terrama2.alerts.controllers";

  angular.module(moduleName, [])
    .controller("AlertListController", ListController);

  return moduleName;
});