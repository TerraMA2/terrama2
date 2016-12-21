define([
  "TerraMA2WebApp/alert-box/services/common/index",
  "TerraMA2WebApp/alert-box/services/index"
], function(commonModule, servicesModule) {
  var moduleName = "terrama2.components.messagebox";

  angular.module(moduleName, [commonModule, servicesModule]);

  return moduleName;
});