angular.module("terrama2.projection", [])

  .directive("terrama2Projection", function(i18n) {
    return {
      restrict: 'E',
      templateUrl: "/javascripts/angular/projection.html",
      scope: {
        url: "@",
        projection: "=projection"
      },
      controller: function($scope, $http, i18n) {
        $scope.i18n = i18n;
        $http.get($scope.url, {}).success(function(projections) {
          $scope.projections = projections;
        }).error(function(err) {
          console.log("error in downloading projections list");
        });

        $scope.setActive = function(value) {
          console.log(value);
          $scope.projection = value.name;
        }
      }
    }
  });

