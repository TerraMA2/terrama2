(function() {
  "use strict";
  
  angular.module("terrama2.components.analysis", [
      "terrama2",
      "terrama2.analysis.services",
      "terrama2.components.collapser"
    ])
    .run(["$templateCache", function($templateCache) {
      $templateCache.put("helper.html",
        "<div class=\"dropup\">" +
          "<button aria-expanded=\"false\" type=\"button\" class=\"btn btn-warning dropdown-toggle\" data-toggle=\"dropdown\"> {{ i18n.__('Functions') }}</button>" +
          "<terrama2-list class=\"dropdown-menu\" data=\"AnalysisOperators.$data\"></terrama2-list>" +
        "</div>");
    }])
    .directive("terrama2AnalysisHelpers", ["i18n", "AnalysisOperators", terrama2AnalysisHelpersDirective]);

  /**
   * It defines a Analysis Button with Available helpers functions
   * 
   * @example
   * <terrama2-analysis-helpers class="MyClass"></terrama2-analysis-helpers>
   * 
   * @returns {angular.IDirective}
   */
  function terrama2AnalysisHelpersDirective(i18n, AnalysisOperators) {
    return {
      restrict: "E",
      replace: true,
      scope: {
        target: '='
      },
      controller: ["$scope", controllerFn],
      templateUrl: "helper.html",
      link: linkFn
    };

    /**
     * It handles crude directive behavior
     * 
     * @param {angular.IScope} $scope - Directive Scope. Used emit and listen children events
     */
    function controllerFn($scope) {
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
          $scope.target += item.code;
        }
      });
    }

    /**
     * It defines post-link directive binding. Once triggered, it injects Analysis Operators that contains operators data into scope
     * and prepares to populate recursive links
     * 
     * @param {angular.IScope} scope - Angular Directive Scope
     * @param {angular.IElement} element - Directive Selector (jQlite)
     * @param {angular.IAttributes} attrs - Angular directive attributes
     */
    function linkFn(scope, element, attrs) {
      scope.AnalysisOperators = AnalysisOperators;
      // Retrieving operators json async
      AnalysisOperators.init();

      var jElement = $(element);

      $(jElement, '.dropdown-submenu a.test').on("click", function(e){
        var jTarget = $(e.target);
        jTarget.next('ul').toggle();
        e.stopPropagation();
        e.preventDefault();
      });
    } // end linkFn
  } // end terrama2AnalysisHelpersDirective function

} ());