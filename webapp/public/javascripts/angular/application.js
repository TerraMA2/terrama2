define([
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/countries/app",
  "TerraMA2WebApp/alert-box/app"
], function(commonModule, countriesModule, alertBoxModule) {
  var terrama2Module = angular.module("terrama2", [
    commonModule,
    countriesModule,
    alertBoxModule
  ]);

  terrama2Module.init = function() {
    angular.bootstrap(document, ['terrama2']);
  };

  return terrama2Module;
})

require(["TerraMA2WebApp/application"], function(app) {
  app.init();
});