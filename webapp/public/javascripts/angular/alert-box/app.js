define([
  "TerraMA2WebApp/alert-box/common/index",
  "TerraMA2WebApp/alert-box/directives/index",
  "TerraMA2WebApp/alert-box/services/index"
], function(commonModule, directivesModule, servicesModule) {
  var moduleName = "terrama2.components.messagebox";

  angular.module(moduleName, [commonModule, directivesModule, servicesModule]);

  return moduleName;
});