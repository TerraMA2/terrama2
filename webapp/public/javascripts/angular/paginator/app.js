define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/paginator/services/paginator",
  "TerraMA2WebApp/paginator/filters/paginator-limit",
  "TerraMA2WebApp/paginator/directives/paginator-controls"
], function(commonServiceApp, PaginatorService, PaginatorLimitFilter, PaginatorDirective) {
  "use strict";

  var moduleName = "terrama2.components.paginator";

  angular
    .module(moduleName, [commonServiceApp])
    .service("PaginatorService", PaginatorService)
    .filter("paginatorLimit", PaginatorLimitFilter)
    .directive("terrama2Paginator", PaginatorDirective);

  return moduleName;
});