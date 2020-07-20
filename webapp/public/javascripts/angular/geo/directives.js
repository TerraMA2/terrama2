define([], function() {

"use strict";

var moduleName = "terrama2.components.geo.directives";
var app = angular.module(moduleName, []);

app.run(["$templateCache", function($templateCache) {
  $templateCache.put("bounded-box.html",
  "<form name='filterForm.boundedForm'>" +
    "<div class=\"col-md-12 terrama2-nopadding-box\">" +
      "<div class=\"col-md-12 terrama2-nopadding-box\">" +
        "<div class=\"col-md-6\">" +
          "<div class=\"form-group\" terrama2-show-errors>" +
            "<label>{{ i18n.__('X min') }}:</label>" +
            "<input class=\"form-control\" id=\"minX\" name=\"minX\" ng-model=\"model.minX\" type=\"number\" max=\"{{model.maxX}}\" ng-required=\"isRequired\">" +
            "<span class='help-block' ng-show='filterForm.boundedForm.minX.$dirty && filterForm.boundedForm.minX.$error.required'>" +
            "{{ i18n.__('MinX is required') }}"+
            "</span> " +
            "<span class='help-block' ng-show='filterForm.boundedForm.minX.$dirty && filterForm.boundedForm.minX.$error.max'>" +
            "{{ i18n.__('MinX is invalid') }}"+
            "</span> " +
          "</div>" +
        "</div>" +

        "<div class=\"col-md-6\">" +
          "<div class=\"form-group\" terrama2-show-errors>" +
            "<label>X max:</label>" +
            "<input class=\"form-control\" id=\"maxX\" name=\"maxX\" ng-model=\"model.maxX\" type=\"number\" min=\"{{model.minX}}\" ng-required=\"isRequired\">" +
            "<span class='help-block' ng-show='filterForm.boundedForm.maxX.$dirty && filterForm.boundedForm.maxX.$error.required'>" +
            "{{ i18n.__('MaxX is required') }}"+
            "</span> " +
            "<span class='help-block' ng-show='filterForm.boundedForm.maxX.$dirty && filterForm.boundedForm.maxX.$error.min'>" +
            "{{ i18n.__('MaxX is invalid') }}"+
            "</span> " +
          "</div>" +
        "</div>" +
      "</div>" +

      "<div class=\"col-md-12 terrama2-nopadding-box\">" +
        "<div class=\"col-md-6\">" +
          "<div class=\"form-group\" terrama2-show-errors>" +
            "<label>Y min:</label>" +
            "<input class=\"form-control\" id=\"minY\" name=\"minY\" ng-model=\"model.minY\" type=\"number\" max=\"{{model.maxY}}\" ng-required=\"isRequired\">" +
            "<span class='help-block' ng-show='filterForm.boundedForm.minY.$dirty && filterForm.boundedForm.minY.$error.required'>" +
            "{{ i18n.__('MinY is required') }}"+
            "</span> " +
            "<span class='help-block' ng-show='filterForm.boundedForm.minY.$dirty && filterForm.boundedForm.minY.$error.max'>" +
            "{{ i18n.__('MinY is invalid') }}"+
            "</span> " +
          "</div>" +
        "</div>" +

        "<div class=\"col-md-6\">" +
          "<div class=\"form-group\" terrama2-show-errors>" +
            "<label>Y max:</label>" +
            "<input class=\"form-control\" id=\"maxY\" name=\"maxY\" ng-model=\"model.maxY\" type=\"number\" min=\"{{model.minY}}\" ng-required=\"isRequired\">" +
            "<span class='help-block' ng-show='filterForm.boundedForm.maxY.$dirty && filterForm.boundedForm.maxY.$error.required'>" +
            "{{ i18n.__('MaxY is required') }}"+
            "</span> " +
            "<span class='help-block' ng-show='filterForm.boundedForm.maxY.$dirty && filterForm.boundedForm.maxY.$error.min'>" +
            "{{ i18n.__('MaxY is invalid') }}"+
            "</span> " +
          "</div>" +
        "</div>" +
      "</div>" +

      "<div class=\"col-md-6\">" +
        "<div class=\"form-group\" terrama2-show-errors>" +
          "<label ng-bind=\"i18n.__('Projection SRID')\">:</label>" +
          "<input class=\"form-control\" id=\"projection\" name=\"srid\" ng-model=\"model.srid\" type=\"number\" min=\"0\" max=\"998999\" placeholder=\"{{ i18n.__('Type a projetion.') }}\" ng-required=\"isRequired\">" +
          "<span class='help-block' ng-show='boundedForm.srid.$dirty && boundedForm.srid.$error.required'>" +
          "{{ i18n.__('SRID is required') }}"+
          "</span> " +
          "<span class='help-block' ng-show='boundedForm.srid.$dirty && boundedForm.srid.$error.number'>" +
          "{{ i18n.__('SRID is invalid') }}"+
          "</span> " +
        "</div>" +
      "</div>" +

      "<div class=\"col-md-6\" ng-if=\"model.showCrop\">" +
        "<div class=\"form-group \">" +
          "<div class=\"checkbox\" style=\"margin-left: 20px; margin-top:30px;\">" +
            "<span style=\"font-weight: 700;\" >" +
              "<input type=\"checkbox\" id=\"projection\" name=\"crop_raster\" ng-model=\"model.crop_raster\"> {{ i18n.__('Crop raster') }} "+
            "</span>" +
          "</div>" +
        "</div>" +
      "</div>" +
    "</div>" +
  "</form>");
}]);

app.directive("terrama2BoundedBox", ["i18n", "Polygon", "FilterForm", function(i18n, Polygon, FilterForm) {
  return {
    restrict: "E",
    templateUrl: "bounded-box.html",
    require: "ngModel",
    scope: {
      model: "=ngModel",
      isRequired: "="
    },
    controller: ["$scope", function($scope) {
      $scope.filterForm = FilterForm;
      $scope.i18n = i18n;
    }],
    link: function(scope, element, attrs, ngModelController) {
      function updateView(value) {
        ngModelController.$viewValue = value;
        ngModelController.$render();
      }

      function updateModel(value) {
        ngModelController.$modelValue = value;
        scope.ngModel = value;
      }

      scope.$on("fillBoundedBox", function(event, model) {
        updateModel(Polygon.read(model));
      });
    }
  };
}]);
  return moduleName;
})