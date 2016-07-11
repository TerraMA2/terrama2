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
        remove: '&?',
        extra: '=?extra'
      },

      controller: function($scope, $http, i18n) {
        $scope.i18n = i18n;
        $scope.searchInput = '';
        $scope.selected = {};
        $scope.emptyMessage = 'No ' + ($scope.context || 'data') + ' found';

        // defines display fields in table
        $scope.displayFields = [];
        // fields identifiers
        $scope.identityFields = [];

        var cacheLinks = {};

        $scope.extra = $scope.extra ? $scope.extra : {};

        $scope.objectToRemove = null;

        $scope.setModelSelected = function(modelSelected) {
          $scope.selected = modelSelected;
        }

        $scope.confirmRemoval = function(object) {
          $scope.objectToRemove = object;

          $('#myModal').modal();
        };

        $scope.resetObjectToRemove = function() {
          $scope.objectToRemove = null;
        };

        // remove function
        $scope.removeOperation = function() {
          if($scope.objectToRemove !== null) {
            var object = $scope.objectToRemove;
            $scope.resetObjectToRemove();

            // callback
            var callback = $scope.extra.removeOperationCallback;
            $http({
              method: 'DELETE',
              url: $scope.remove({object: object})
            }).success(function(response) {
              $scope.model.forEach(function(element, index, arr) {
                if (element.id == object.id)
                  arr.splice(index, 1);

                if ($scope.isFunction(callback))
                  callback(null, response);
              });

            }).error(function(err) {
              if ($scope.isFunction(callback))
                callback(err);
              console.log(err);
            }).finally(function() {
              // $scope.selected = {};
            });
          }
        };

        if (!$scope.iconProperties)
          $scope.iconProperties = {type: 'img'};

        $scope.makeLink = function(element) {
          var link = cacheLinks[element.id];
          if (!link) {
            var link = $scope.link()(element);
            cacheLinks[element.id] = link;
          }
          return link;
        }

        $scope.processField = function(key, obj) {
          if (key.indexOf('.') > 0) {
            var arr = key.split(".");
            var output = obj;

            arr.forEach(function(elm) {
              output = output[elm];
            })
            return output;
          } else {
            return obj[key];
          }
        }

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
