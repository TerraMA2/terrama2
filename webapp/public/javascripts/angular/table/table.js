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
        linkToAdd: '=?linkToAdd',
        context: '=context',
        remove: '&'
      },
      
      controller: function($scope, $http, i18n) {
        $scope.i18n = i18n;
        $scope.searchInput = '';
        $scope.emptyMessage = 'No ' + ($scope.context || 'data') + ' found';
        
        // defines display fields in table
        $scope.displayFields = [];
        // fields identifiers
        $scope.identityFields = [];

        // remove function
        $scope.removeOperation = function(object) {
          // todo: open model: confirmation
          $http({
            method: 'DELETE',
            url: $scope.remove({object: object})
          }).success(function(response) {
            $scope.model.forEach(function(element, index, arr) {
              if (element.id == object.id)
                arr.splice(index, 1);
            });
          }).error(function(err) {
            console.log(err);
          });
        };

        if (!$scope.iconProperties)
          $scope.iconProperties = {type: 'img'};

        $scope.$watch('fields', function(fields) {
          // processing fields
          fields.forEach(function(field) {
            if (field instanceof Object) {
              // todo: validation throwing errors
              $scope.displayFields.push(field.as);
              $scope.identityFields.push(field.key);
            } else {
              $scope.displayFields.push(field);
              $scope.identityFields.push(field);
            }
          });

        });

        $scope.width = $scope.iconProperties.width || 24;
        $scope.width = $scope.iconProperties.height || 24;

        $scope.isFunction = function(target) {
          return angular.isFunction(target);
        };
        
        $scope.capitalizeIt = function(str) {
          return str.charAt(0).toUpperCase() + str.slice(1);
        }
      }
    }
  });