angular.module('terrama2.listDataSeries', ['terrama2.table', 'terrama2.services'])
  .controller("ListController", ['$scope', 'DataSeriesFactory', function($scope, DataSeriesFactory) {
    $scope.dataSeriesType = configuration.dataSeriesType || 'dynamic';
    $scope.model = [];
    $scope.fields = [];
    
    $scope.remove = function(object) {
      return "/api/DataSeries/" + object.id + "/delete";
    };
    
    DataSeriesFactory.get({type: $scope.dataSeriesType, collector: true}).success(function(data) {
      $scope.model = data instanceof Array ? data : [];
      $scope.fields = [{key: 'name', as: "Name"}, {key: "semantics", as: "Format"}];
    }).error(function(err) {

    });

    $scope.link = configuration.link || null;

    $scope.linkToAdd = configuration.linkToAdd || null;

    $scope.iconFn = configuration.iconFn || null;
      
    $scope.iconProperties = configuration.iconProperties || {};
  }]);
