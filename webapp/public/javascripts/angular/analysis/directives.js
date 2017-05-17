define([
  "TerraMA2WebApp/analysis/services",
  "TerraMA2WebApp/collapser/directives"
], function(servicesApp, collapserApp) {
  "use strict";

  var moduleName = "terrama2.analysis.directives";
  
  angular.module(moduleName, [servicesApp, collapserApp])
    .run(["$templateCache", function($templateCache) {
      $templateCache.put("helper.html",
        "<div class=\"dropup pull-left\" style=\"margin-left: 10px;\">" + 
          "<button aria-expanded=\"false\" type=\"button\" class=\"btn dropdown-toggle\" data-toggle=\"dropdown\">" +
            "<img style=\"height: 20px;\" ng-src=\"{{operators.imagePath}}\" data-toggle=\"tooltip\" data-placement=\"top\" ng-attr-title=\"{{operators.name}}\"/>" +
          "</button>" +
          "<terrama2-list class=\"dropdown-menu\" data=\"operatorsData\" expression=\"restriction\"></terrama2-list>" +
        "</div>");
    }])
    .directive("terrama2AnalysisHelpers", ["i18n", "$http", terrama2AnalysisHelpersDirective]);

  /**
   * It defines a Analysis Button with Available helpers functions
   * 
   * @example
   * <terrama2-analysis-helpers class="MyClass"></terrama2-analysis-helpers>
   * 
   * @returns {angular.IDirective}
   */
  function terrama2AnalysisHelpersDirective(i18n, $http) {
    return {
      restrict: "E",
      replace: true,
      transclude: true,
      scope: {
        target: '=',
        restriction: "=",
        operators: '='
      },
      controller: ["$scope", "i18n", controllerFn],
      templateUrl: "helper.html",
      link: linkFn
    };

    /**
     * It handles crude directive behavior
     * 
     * @param {angular.IScope} $scope - Directive Scope. Used emit and listen children events
     */
    function controllerFn($scope, i18n) {
      $scope.i18n = i18n;
      $scope.operatorsData = [];
      /**
       * Listener for Item clicked. Whenever retrieve a item, It must have code in order to append script context
       * 
       * @event #itemClicked
       * @param {angular.IEvent} event - Angular event
       * @param {Object}    item - TerraMA² operator item
       * @param {string}    item.name - Operator name
       * @param {string?}   item.code - TerraMA² operator representation
       * @param {Object[]?} item.children - Sub Items of operator
       */
      $scope.$on("itemClicked", function(event, item) {
        if (item && item.code) {
          if ($scope.target === undefined || $scope.target === null) {
            $scope.target = item.code;
          } else {
            $scope.target += item.code;
          }
        }
      });
    }

    /**
     * It defines post-link directive binding. Once triggered, populate operators data
     * 
     * @param {angular.IScope} scope - Angular Directive Scope
     * @param {angular.IElement} element - Directive Selector (jQlite)
     * @param {angular.IAttributes} attrs - Angular directive attributes
     */
    function linkFn(scope, element, attrs) {

      // watch operators to get file data
      scope.$watch('operators', function(operators){
        if (operators){
          if (operators.fileName){
            var pathFile = "/javascripts/angular/analysis/data/" + operators.fileName;

            $http.get(pathFile).then(function(response){
              scope.operatorsData = response.data;
            });
          } else {
            scope.operatorsData = operators.data;
          }
        }
      });
    } // end linkFn
  } // end terrama2AnalysisHelpersDirective function

  return moduleName;
});