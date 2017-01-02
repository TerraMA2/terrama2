define([
  "TerraMA2WebApp/common/loader",
  "TerraMA2WebApp/common/app",
  "TerraMA2WebApp/data-series/schedule",
  "TerraMA2WebApp/data-series/services",
  "TerraMA2WebApp/views/services/index",
  "TerraMA2WebApp/datetimepicker/directive",
  "TerraMA2WebApp/ace/directive",
  "TerraMA2WebApp/alert-box/app",
  "TerraMA2WebApp/services/services",

  "TerraMA2WebApp/views/controllers/view-list",
  "TerraMA2WebApp/views/controllers/view-register-update"
], function(moduleLoader, commonApp, scheduleApp, dataSeriesServiceApp, viewsService, datetimepicker, 
            aceApp, messageBoxApp, serviceApp, ListController, RegisterUpdateController) {
  var moduleName = "terrama2.views.controllers";

  var deps = [messageBoxApp, viewsService];

  moduleLoader(aceApp, deps);
  // controller RegisterUpdate dependencies
  moduleLoader("schemaForm", deps, function() {
    moduleLoader("color.picker", deps);
    moduleLoader(datetimepicker, deps);
    moduleLoader(serviceApp, deps);
    moduleLoader(scheduleApp, deps);
  });

  angular.module(moduleName, deps)
    .controller("ListController", ListController)
    .controller("RegisterUpdateController", RegisterUpdateController);

  return moduleName;
})