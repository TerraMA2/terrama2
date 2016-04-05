angular.module('terrama2.table', ['terrama2'])
  .directive('terrama2Table', function(i18n) {
    return {
      restrict: 'E',
      templateUrl: '/javascripts/angular/table/templates/table.html',
      scope: {
        fields: '=fields',
        model: '=model',
        link: '&',
        icon: '&',
        iconProperties: '=?iconProperties',
        linkToAdd: '=?linkToAdd'
      },
      
      controller: function($scope, i18n) {
        $scope.i18n = i18n;
        $scope.searchInput = '';

        if (!$scope.iconProperties)
          $scope.iconProperties = {type: 'img'};

        $scope.width = $scope.iconProperties.width || 24;
        $scope.width = $scope.iconProperties.height || 24;

        $scope.isFunction = function() {
          return angular.isFunction($scope.icon());
        };
        
        $scope.capitalizeIt = function(str) {
          return str.charAt(0).toUpperCase() + str.slice(1);
        }
      }
    }
  });