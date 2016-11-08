"use strict";

angular.module('terrama2.datetimepicker', ['terrama2', 'ae-datetimepicker'])
  .directive('terrama2DatetimePicker', function() {
    return {
      restrict: 'EA',
      templateUrl: '/javascripts/angular/datetimepicker/templates/datetimepicker.html',
      scope: {
        dateTime: '=ngModel',
        options: '=?',
        onChange: '&'
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
      controllerAs: 'vm',
      controller: function($scope) {

        this.optionsFrom = {sideBySide: true, toolbarPlacement: 'top', allowInputToggle: false, useCurrent: false};
        this.optionsTo = {sideBySide: true, toolbarPlacement: 'top', allowInputToggle: false, useCurrent: false};

        this.update = function (dateFrom, dateTo) {
            this.optionsFrom.maxDate = dateTo;
            this.optionsTo.minDate = dateFrom;
        };

        $scope.i18n = i18n;

      }
    }
  });