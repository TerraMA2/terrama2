define([
  "TerraMA2WebApp/analysis/controllers/index",
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/analysis/directives",
  "TerraMA2WebApp/analysis/components/queryBuilder",
  "TerraMA2WebApp/analysis/enum",
  "TerraMA2WebApp/analysis/components/vectorProcessing",
  "TerraMA2WebApp/analysis/components/queryBuilderWrapper",
], function(controllersModule, servicesModule, directivesModule, queryBuilder, enumAnalysis, vectorProcessing, queryBuilderWrapper) {
  var moduleName = "terrama2.analysis";

  angular.module(moduleName, [servicesModule, directivesModule, controllersModule])
    .component("queryBuilder", queryBuilder)
    .component("queryBuilderWrapper", queryBuilderWrapper)
    .component("vectorProcessing", vectorProcessing)
    .constant("SpatialOperations", enumAnalysis.SpatialOperations)

  return moduleName;
})