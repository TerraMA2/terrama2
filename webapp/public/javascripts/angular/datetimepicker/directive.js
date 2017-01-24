define([
  "TerraMA2WebApp/common/loader"
], function(moduleLoader) {

  "use strict";

  var moduleName = "terrama2.datetimepicker";
  var deps = [];

  if (moduleLoader("ae-datetimepicker", deps)) {
    angular.module(moduleName, deps)
      .constant("locales", {en_US: "en", pt_BR: "pt-br", es_ES: "es", fr_FR: "fr"})
      .directive("terrama2DatetimePicker", function() {
        return {
          restrict: "EA",
          templateUrl: "/dist/templates/datetimepicker/templates/datetimepicker.html",
          scope: {
            dateTime: "=ngModel",
            options: "=?"
          }
        };
      })

      .directive("terrama2FilterDatetimePicker", ["i18n", "locales", function(i18n, locales) {
        return {
          restrict: "EA",
          templateUrl: "/dist/templates/datetimepicker/templates/filterdatetimepicker.html",
          scope: {
            ngModel: "=",
            beforeDatetime: "=beforeDatetime",
            afterDatetime: "=afterDatetime",
            beforeLabel: "=",
            afterLabel: "=",
            options: "=?"
          },
          controllerAs: "vm",
          controller: ["$scope", function($scope) {

            $scope.i18n = i18n;
            var locale = locales[i18n.userLanguage];

            if (!locale) {
              locale = "en";
            }

            this.optionsFrom = {locale: locale, sideBySide: true, toolbarPlacement: "top", allowInputToggle: true, useCurrent: false,  format: "L HH:mm"};
            this.optionsTo = {locale: locale, sideBySide: true, toolbarPlacement: "top", allowInputToggle: true, useCurrent: false,  format: "L HH:mm"};

            this.update = function (dateFrom, dateTo) {
                this.optionsFrom.maxDate = dateTo;
                this.optionsTo.minDate = dateFrom;
            };
          }]
        };
      }]);
  }

  return moduleName;
});