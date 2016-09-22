"use strict";

angular.module('terrama2.datetimepicker', ['terrama2'])
  .directive('terrama2DatetimePicker', function() {
    return {
      restrict: 'EA',
      templateUrl: '/javascripts/angular/datetimepicker/templates/datetimepicker.html',
      scope: {
        dateTime: '=ngModel',
        options: '=?'
      }
    };
  })

  .directive('terrama2FilterDatetimePicker', function(i18n) {
    return {
      restrict: 'EA',
      templateUrl: '/javascripts/angular/datetimepicker/templates/filterdatetimepicker.html',
      scope: {
        ngModel: "=",
        beforeDatetime: '=beforeDatetime',
        afterDatetime: '=afterDatetime',
        beforeLabel: '=',
        afterLabel: '=',
        options: '=?'
      },
      controller: function($scope) {
        $scope.i18n = i18n;
        $scope.options = angular.extend({}, {
          format: "YYYY/MM/DD HH:mm:ss",
          sideBySide: true,
          toolbarPlacement: 'top',
          allowInputToggle: false,
          useCurrent: false
        }, $scope.options);

        $scope.beforeOptions = angular.extend({}, $scope.options, {maxDate: $scope.afterDatetime});
        $scope.afterOptions = angular.extend({}, $scope.options, {minDate: $scope.beforeDatetime});
      }
    }
  });