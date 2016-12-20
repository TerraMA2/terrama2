define([
  "TerraMA2WebApp/common/directives/box",
  "TerraMA2WebApp/common/directives/button",
  "TerraMA2WebApp/common/directives/content",
  "TerraMA2WebApp/common/directives/datetime",
  "TerraMA2WebApp/common/directives/datetime-formatter",
  "TerraMA2WebApp/common/directives/fluid",
  "TerraMA2WebApp/common/directives/show-errors",
], function(terrama2Box, terrama2Button, terrama2Content, terrama2Datetime, terrama2DateTimeFormatter, terrama2Fluid, terrama2ShowErrors) {
  var moduleName = "terrama2.common.directives";
  angular.module(moduleName, [])
    .directive("terrama2Box", terrama2Box)
    .directive("terrama2Content", terrama2Content)
    .directive("terrama2Datetime", terrama2Datetime)
    .directive("formatDatetime", terrama2DateTimeFormatter)
    .directive("terrama2Fluid", terrama2Fluid)
    .directive("terrama2ShowErrors", terrama2ShowErrors)
    .run(["$templateCache", function($templateCache) {
      // TerraMA2 Box
      $templateCache.put('box.html',
      '<div class="col-md-12" title="{{ titleHeader }}">' +
        '<div class="box box-default {{ boxType }}" title="{{ titleHeader }}">' +
          '<div class="box-header with-border">' +
            '<h3 class="box-title">{{ titleHeader }}</h3>' +
            '<div class="box-tools pull-right">' +
              '<button type="button" class="btn btn-box-tool terrama2-circle-button" style="margin-right: 15px;" data-toggle="tooltip" data-placement="bottom" title="{{ helper }}"><i class="fa fa-question"></i></button>' +
              '<button type="button" class="btn btn-box-tool" data-widget="collapse"><i ng-if="!collapsed" class="fa fa-minus"></i></button> ' +
            '</div>' +
          '</div>' +
          '<div class="box-body" id="targetTransclude">' +
          '</div>' +
        '</div>' +
      '</div>');
    }]);
  
  return moduleName;
});