angular.module('terrama2.listDataSeries', ['terrama2.table', 'terrama2.services'])
  .controller("ListController", ['$scope', 'DataSeriesFactory', function($scope, DataSeriesFactory) {
    $scope.dataSeriesType = configuration.dataSeriesType || 'dynamic';
    $scope.model = [];
    $scope.fields = [];
    DataSeriesFactory.get({type: $scope.dataSeriesType}).success(function(data) {
      $scope.model = data;
      $scope.fields = [{key: 'name', as: "Name"}, {key: "semantics", as: "Format"}];
    }).error(function(err) {

    });

    $scope.link = configuration.link || null;

    $scope.linkToAdd = configuration.linkToAdd || null;

    $scope.iconFn = configuration.iconFn || null;
      
    $scope.iconProperties = configuration.iconProperties || {};
  }]);
