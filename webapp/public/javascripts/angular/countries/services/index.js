define(["TerraMA2WebApp/countries/services/countries"],
  function(TerraMA2Countries) {
    var moduleName = "terrama2.countries.services";
    angular.module(moduleName, [])
      .factory("TerraMA2Countries", TerraMA2Countries);
    return moduleName;
  }
)