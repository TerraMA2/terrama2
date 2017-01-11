define([
    "TerraMA2WebApp/countries/services/index",
    "TerraMA2WebApp/countries/directives/index"
  ],
  function(serviceModule, directivesModule) {
    var moduleName = "terrama2.countries";
    angular.module(moduleName, [serviceModule, directivesModule])
      .run(["TerraMA2Countries", function(TerraMA2Countries) {
        TerraMA2Countries.init();
      }]);

    return moduleName;
  }
)