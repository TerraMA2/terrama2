"use strict";

var app = angular.module('terrama2.components.geo');

app.run(function($templateCache) {
  $templateCache.put("bounded-box.html",
  "<form name='boundedForm'>" +
    "<div class=\"col-md-12 terrama2-nopadding-box\">" +
      "<div class=\"col-md-6\">" +
        "<div class=\"form-group\" terrama2-show-errors>" +
          "<label>{{ i18n.__('X min') }}:</label>" +
          "<input class=\"form-control\" id=\"minX\" name=\"minX\" ng-model=\"model.minX\" type=\"number\">" +
          "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.required'>" +
          "{{ i18n.__('MinX is required') }}"+
          "</span> " +
          "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.number'>" +
          "{{ i18n.__('MinX is invalid') }}"+
          "</span> " +
        "</div>" +
      "</div>" +

      "<div class=\"col-md-6\">" +
        "<div class=\"form-group\" terrama2-show-errors>" +
          "<label>X max:</label>" +
            "<input class=\"form-control\" id=\"maxX\" name=\"maxX\" ng-model=\"model.maxX\" type=\"number\">" +
            "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.required'>" +
            "{{ i18n.__('MinX is required') }}"+
            "</span> " +
            "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.number'>" +
            "{{ i18n.__('MinX is invalid') }}"+
            "</span> " +
          "</div>" +
        "</div>" +

        "<div class=\"col-md-6\">" +
          "<div class=\"form-group\" terrama2-show-errors>" +
            "<label>Y min:</label>" +
              "<input class=\"form-control\" id=\"minY\" name=\"minY\" ng-model=\"model.minY\" type=\"number\">" +
              "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.required'>" +
              "{{ i18n.__('MinX is required') }}"+
              "</span> " +
              "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.number'>" +
              "{{ i18n.__('MinX is invalid') }}"+
              "</span> " +
            "</div>" +
          "</div>" +

          "<div class=\"col-md-6\">" +
            "<div class=\"form-group\" terrama2-show-errors>" +
              "<label>Y max:</label>" +
              "<input class=\"form-control\" id=\"maxY\" name=\"maxY\" ng-model=\"model.maxY\" type=\"number\">" +
              "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.required'>" +
              "{{ i18n.__('MinX is required') }}"+
              "</span> " +
              "<span class='help-block' ng-show='boundedForm.minX.$dirty && boundedForm.minX.$error.number'>" +
              "{{ i18n.__('MinX is invalid') }}"+
              "</span> " +
            "</div>" +
          "</div>" +
        "</div>" +
      "</div>" +
    "</div>" +
  "</form>");
});

app.directive("terrama2BoundedBox", function(i18n) {
  return {
    restrict: "E",
    templateUrl: "bounded-box.html",
    require: "ngModel",
    scope: {
      model: "=ngModel"
    },
    controller: function($scope) {
      $scope.i18n = i18n;

      $scope.$on("updateBoundedBox", function(event, coordinates) {
        $scope.model.minX = coordinates[0][0][0];
        $scope.model.minY = coordinates[0][0][1];
        $scope.model.maxX = coordinates[0][2][0];
        $scope.model.maxY = coordinates[0][2][1];
      });
    }
  };
});