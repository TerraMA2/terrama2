define([
  "TerraMA2WebApp/alerts/services/alert"
], function(AlertService) {
  var moduleName = "terrama2.alerts.services";

  angular.module(moduleName, [])
    .service("AlertService", AlertService);
  
  return moduleName;
});