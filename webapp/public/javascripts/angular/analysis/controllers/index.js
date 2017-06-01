define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/table/table",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/ace/directive",
  "TerraMA2WebApp/data-series/app",
  "TerraMA2WebApp/data-provider/app",
  "TerraMA2WebApp/datetimepicker/directive",
  "TerraMA2WebApp/geo/app",
  // controllers classes
  "TerraMA2WebApp/analysis/controllers/list",
  "TerraMA2WebApp/analysis/controllers/register-update",
  "TerraMA2WebApp/schema-form-plugin/mask-warn/directives/terrama2-mask-field"
], function(moduleLoader, commonApp, tableApp, messageBoxApp, servicesApp, analysisServicesApp, aceApp, dataSeriesApp, 
            dataProviderApp, datetimepickerApp, geoApp, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.analysis.controllers";
  // common dependencies both List and Register Update
  var deps = [commonApp, tableApp, servicesApp, messageBoxApp, analysisServicesApp];

  /** 
   * TODO: module loading dinamically pushing into "angular.module(moduleName).requires" array. @note It must be used before
   * directive bootstrapping. 
   */
  if (moduleLoader("schemaForm", deps) && 
      moduleLoader("treeControl", deps) && 
      moduleLoader("ui.select", deps) && 
      moduleLoader("ngSanitize", deps) &&
      moduleLoader("ngMaterial", deps)) {
    // register update dependencies
    deps.push(aceApp);
    deps.push(dataSeriesApp);
    deps.push(geoApp);
    deps.push(dataProviderApp);
    deps.push(datetimepickerApp);
  }

  angular.module(moduleName, deps)
    .controller("AnalysisListController", ListController)
    .controller("AnalysisRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});