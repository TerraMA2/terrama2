angular.module('terrama2.listDataSeries', ['terrama2.table', 'terrama2.services', 'terrama2.components.messagebox'])
  .controller("ListController", ['$scope', 'DataSeriesFactory', function($scope, DataSeriesFactory) {
    $scope.dataSeriesType = configuration.dataSeriesType || 'dynamic';
    $scope.model = [];
    $scope.fields = [];
    
    $scope.remove = function(object) {
      return "/api/DataSeries/" + object.id + "/delete";
    };

    $scope.extra = {
      removeOperationCallback: function(err, data) {
        $scope.display = true;
        if (err) {
          $scope.alertLevel = "alert-danger";
          $scope.alertBox.message = err.message;
          return;
        }

        $scope.alertLevel = "alert-success";
        $scope.alertBox.message = data.name + " removed";
      }
    };
    $scope.method = "{[ method ]}";
    $scope.alertLevel = "alert-success";
    $scope.alertBox = {
      title: "Data Series",
      message: configuration.message
    };
    $scope.resetState = function() { $scope.display = false; };
    $scope.display = configuration.message !== "";
    
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
