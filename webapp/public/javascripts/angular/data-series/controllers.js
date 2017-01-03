define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/datetimepicker/directive",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/geo/app",

  // controllers
  "TerraMA2WebApp/data-series/data-series",
  "TerraMA2WebApp/data-series/registration"
], function(moduleLoader, commonServiceApp, messageboxApp, datetimepickerApp, dataSeriesServicesApp, scheduleApp, geoApp, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.dataseries.controllers";
  var deps = [commonServiceApp, messageboxApp];

  // checking externals dependencies
  if (moduleLoader("schemaForm", deps) && 
      moduleLoader("xeditable", deps) && 
      moduleLoader("treeControl", deps) &&
      moduleLoader("ui.router", deps) && 
      moduleLoader("mgo-angular-wizard")) {
    deps.push(commonServiceApp);
    deps.push(dataSeriesServicesApp);
    deps.push(scheduleApp);
    deps.push(datetimepickerApp);
    deps.push(geoApp);
  }

  angular.module(moduleName, deps)
    .controller("DataSeriesListController", ListController)
    .controller("DataSeriesRegisterUpdateController", RegisterUpdateController);

  return moduleName;
});