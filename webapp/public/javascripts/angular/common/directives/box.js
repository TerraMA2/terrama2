define(
  function() {
    /**
     * A generic component for displays a TerraMA² boxes.
     * 
     * @example
     * <terrama2-box title="'Data Provider'" css="{boxType: 'box-solid'}">
     *   <h1>Data Provider Registration</h1>
     * 
     *   <fieldset>
     *     ...
     *   </fieldset>
     * </terrama2-box>
     */
    function terrama2Box($parse) {
      return {
        restrict: 'E',
        transclude: true,
        templateUrl: 'box.html', // template cache
        scope: {
          titleHeader: '=title',
          helper: '=?helper',
          extra: '=?',
          handleButtons: '=',
          css: '=?'
        },
        controller: ["$scope", "$http", function($scope, $http) {
          $http({
            url: BASE_URL + "exist-helper",
            method: "POST",
            data: {
              file: $scope.helper
            }
          }).then(function(response) {
            $scope.existHelper = response.data.result;

            $scope.css = $scope.css || {};

            $scope.boxType = "";

            if($scope.css.boxType)
              $scope.boxType = $scope.css.boxType;
          });
        }],
        link: function(scope, element, attrs, ctrl, transclude) {
          var elm = element.find('#targetTransclude');

          scope.collapseBox = function() {
            var elementResult = element[0].getElementsByClassName("collapserBtn");
            angular.element(elementResult).click();
          };

          transclude(scope.$parent, function(clone, scope) {
            elm.append(clone);
          });
        }
      };
    }

    terrama2Box.$inject = ["$parse"];

    return terrama2Box;
  }
);