define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/countries/app",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/app",
  "TerraMA2WebApp/projects/app",
  "TerraMA2WebApp/status/status",
  "TerraMA2WebApp/data-provider/app",
  "TerraMA2WebApp/views/app",
], function(moduleLoader, commonModule, countriesModule, alertBoxModule, serviceModule, projectModule, statusModule, dataProviderModule, viewsModule) {
  var moduleName = "terrama2";

  var deps = [commonModule, countriesModule];

  moduleLoader(alertBoxModule, deps);
  moduleLoader(serviceModule, deps);
  moduleLoader(projectModule, deps);
  moduleLoader(statusModule, deps);
  moduleLoader(dataProviderModule, deps);
  moduleLoader(viewsModule, deps);

  var terrama2Module = angular.module(moduleName, deps);

  terrama2Module.init = function() {
    angular.bootstrap(document, [moduleName]);
  };

  return terrama2Module;
})

require(["TerraMA2WebApp/application"], function(app) {
  app.init();
});