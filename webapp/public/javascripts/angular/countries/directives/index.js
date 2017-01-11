define([
  "TerraMA2WebApp/countries/services/index",
  "TerraMA2WebApp/countries/directives/countries-list"
], function(countriesServiceModule, countriesList) {
  var moduleName = "terrama2.countries.directive";

  angular.module(moduleName, [countriesServiceModule])
    .directive("terrama2CountriesList", countriesList);

  return moduleName;
})