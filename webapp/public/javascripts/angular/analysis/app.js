define([
  "TerraMA2WebApp/analysis/controllers/index",
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/analysis/directives",
  "TerraMA2WebApp/analysis/components/queryBuilder"
], function(controllersModule, servicesModule, directivesModule, queryBuilder) {
  var moduleName = "terrama2.analysis";

  angular.module(moduleName, [servicesModule, directivesModule, controllersModule])
    .component("queryBuilder", queryBuilder);

  return moduleName;
})