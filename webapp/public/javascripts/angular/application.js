define([
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/countries/app"
], function(commonModule, countriesModule) {
  var terrama2Module = angular.module("terrama2", [
    commonModule,
    countriesModule
  ]);

  terrama2Module.init = function() {
    angular.bootstrap(document, ['terrama2']);
  };

  return terrama2Module;
})

require(["TerraMA2WebApp/application"], function(app) {
  app.init();
});