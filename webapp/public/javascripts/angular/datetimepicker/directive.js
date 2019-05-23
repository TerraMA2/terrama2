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
          templateUrl: BASE_URL + "dist/templates/datetimepicker/templates/datetimepicker.html",
          scope: {
            dateTime: "=ngModel",
            options: "=?"
          }
        };
      })

      .directive("terrama2FilterDatetimePicker", ["i18n", "locales", function(i18n, locales) {
        return {
          restrict: "EA",
          templateUrl: BASE_URL + "dist/templates/datetimepicker/templates/filterdatetimepicker.html",
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

            this.optionsFrom = Object.assign({ locale, sideBySide: true, toolbarPlacement: "top", allowInputToggle: true, useCurrent: false,  format: "L HH:mm", timeZone: 'America/Danmarkshavn' }, this.options);
            this.optionsTo = Object.assign({ locale, sideBySide: true, toolbarPlacement: "top", allowInputToggle: true, useCurrent: false,  format: "L HH:mm", timeZone: 'America/Danmarkshavn' }, this.options);

            $scope.optionsTo = this.optionsTo;
            $scope.optionsFrom = this.optionsFrom;
            this.update = function (dateFrom, dateTo) {
              this.optionsFrom.maxDate = dateTo.set({ 'second': 0 });
              this.optionsTo.minDate = dateFrom.set({ 'second': 0 });
            };
            // change datatimepicker locale when change plataform language
            $scope.$on("LOCALE_UPDATED", function(event, args){
              var locale = locales[args.userLanguage];
              if (locale) {
                $scope.optionsTo.locale = locale;
                $scope.optionsFrom.locale = locale;
              }
            })
          }]
        };
      }]);
  }

  return moduleName;
});