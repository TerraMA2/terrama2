angular.module('terrama2.listDataSeries', ['terrama2.table'])
  .controller("ListController", ['$scope', function($scope) {
    $scope.model = configuration.model || [];

    $scope.fields = configuration.fields || [];

    $scope.link = configuration.link || null;

    $scope.linkToAdd = configuration.linkToAdd || null;

    $scope.iconFn = configuration.iconFn || null;
      
    $scope.iconProperties = configuration.iconProperties || {};
  }]);
