define([
  "TerraMA2WebApp/common/directives/box",
  "TerraMA2WebApp/common/directives/button",
  "TerraMA2WebApp/common/directives/content",
  "TerraMA2WebApp/common/directives/datetime",
  "TerraMA2WebApp/common/directives/datetime-formatter",
  "TerraMA2WebApp/common/directives/fluid",
  "TerraMA2WebApp/common/directives/show-errors",
  "TerraMA2WebApp/common/directives/compare-to"
], function(terrama2Box, terrama2Button, terrama2Content, terrama2Datetime, terrama2DateTimeFormatter, terrama2Fluid, terrama2ShowErrors, terrama2CompareTo) {
  var moduleName = "terrama2.common.directives";
  angular.module(moduleName, [])
    .directive("terrama2Box", terrama2Box)
    .directive("terrama2Content", terrama2Content)
    .directive("terrama2Datetime", terrama2Datetime)
    .directive("formatDatetime", terrama2DateTimeFormatter)
    .directive("terrama2Fluid", terrama2Fluid)
    .directive("terrama2ShowErrors", terrama2ShowErrors)
    .directive("terrama2CompareTo", terrama2CompareTo)
    .run(["$templateCache", function($templateCache) {
      // TerraMA2 Box
      $templateCache.put('box.html',
      '<div class="col-md-12" title="{{ titleHeader }}">' +
        '<div class="box box-default {{ boxType }}" title="{{ titleHeader }}">' +
          '<div class="box-header with-border" ng-class="{disabled: extra.disabled}">' +
           '<span ng-if="handleButtons" style="position: absolute;">' +
              '<span ng-if="handleButtons.circle.show()"' +
                    'ng-class="terrama2-font-black"' +
                    'class="button pull-right enable-button"' +
                    'ng-click="handleButtons.circle.click()"' +
                    'data-toggle="tooltip" title="{{handleButtons.circle.message}}">' +
                '<i class="fa fa-plus-circle"></i>' +
              '</span>'+
              '<span ng-if="handleButtons.minus.show()"' +
                    'ng-class="terrama2-font-black"' +
                    'class="button pull-right enable-button"' +
                    'ng-click="handleButtons.minus.click()"'+
                    'data-toggle="tooltip" title="{{handleButtons.minus.message}}">'+
                '<i class="fa fa-minus-circle"></i>'+
              '</span>' +
            '</span>' +
            '<h3 class="box-title" style="margin-left: {{ handleButtons ? \'17px\' : \'0\'}};">' +
              '{{ titleHeader }}' +
            '</h3>' +
            '<div class="box-tools pull-right">' +
              '<button type="button" class="btn btn-box-tool terrama2-circle-button" style="margin-right: 15px;" data-toggle="tooltip" data-placement="bottom" title="{{ helper }}"><i class="fa fa-question"></i></button>' +
              '<button type="button" ng-if="extra.disabled === undefined" class="btn btn-box-tool collapserBtn" data-widget="collapse"><i ng-if="!collapsed" class="fa fa-minus"></i></button> ' +
            '</div>' +
          '</div>' +
          '<div class="box-body" ng-class="{\'terrama2-hidden\': extra.disabled}" id="targetTransclude">' +
          '</div>' +
        '</div>' +
      '</div>');
    }]);
  
  return moduleName;
});