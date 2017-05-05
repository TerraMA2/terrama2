define([
  "TerraMA2WebApp/common/services/index",
  "TerraMA2WebApp/paging-tool/services/paging-tool",
  "TerraMA2WebApp/paging-tool/filters/paging-tool-limit",
  "TerraMA2WebApp/paging-tool/directives/paging-tool-controls"
], function(commonServiceApp, PagingToolService, PagingToolLimitFilter, PagingToolDirective) {
  "use strict";

  var moduleName = "terrama2.components.paging";

  angular
    .module(moduleName, [commonServiceApp])
    .service("PagingToolService", PagingToolService)
    .filter("pagingToolLimit", PagingToolLimitFilter)
    .directive("terrama2PagingTool", PagingToolDirective);

  return moduleName;
});