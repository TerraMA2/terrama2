define([
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/countries/app",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/projects/app",
  "TerraMA2WebApp/status/status",
  "TerraMA2WebApp/data-provider/app",
  "TerraMA2WebApp/views/app",
], function(commonModule, countriesModule, alertBoxModule, projectModule, statusModule, dataProviderModule, viewsModule) {
  var terrama2Module = angular.module("terrama2", [
    commonModule,
    countriesModule,
    alertBoxModule,
    projectModule,
    statusModule,
    dataProviderModule,
    viewsModule
  ]);

  terrama2Module.init = function() {
    angular.bootstrap(document, ['terrama2']);
  };

  return terrama2Module;
})

require(["TerraMA2WebApp/application"], function(app) {
  app.init();
});