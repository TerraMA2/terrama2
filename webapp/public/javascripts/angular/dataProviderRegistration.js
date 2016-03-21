'use strict';

var app = angular.module("terrama2.dataprovider.registration", []);
app.config(['$interpolateProvider', function($interpolateProvider) {
  $interpolateProvider.startSymbol('{[');
  $interpolateProvider.endSymbol(']}');
}]);


app.controller("RegisterController", ["$scope", "$http", "$q", function($scope, $http, $q) {
  $http.get("/api/DataProviderType/", {}).success(function(typeList) {
    $scope.kindList = typeList;
  }).error(function(err) {
    console.log("err type: ", err);
  });

  $scope.errorFound = false;
  $scope.isDynamic = configuration.isDynamic;
  $scope.isEditing = configuration.isEditing;
  $scope.alertBox = {};
  $scope.isChecking = false;
  $scope.message = "";
  $scope.remoteFieldsRequired = false;
  $scope.dataProvider = {
    name: configuration.dataProvider.name,
    description: configuration.dataProvider.description,
    kind: configuration.dataProvider.kind,
    user: configuration.dataProvider.user,
    password: configuration.dataProvider.password,
    address: configuration.dataProvider.address,
    port: parseInt(configuration.dataProvider.port),
    path: configuration.dataProvider.path,
    project: configuration.project,
    active: configuration.dataProvider.active
  };

  var helperPort = function(port) {
    $scope.dataProvider.port = port;
    $scope.remoteFieldsRequired = true;
  };

  $scope.onKindChanged = function() {
    // todo: disable fields?

    switch ($scope.dataProvider.kind.toLocaleLowerCase()) {
      case "file":
        $scope.remoteFieldsRequired = false;
        $scope.dataProvider.port = "";
        break;
      case "http":
        helperPort(80);
        break;
      case "ftp":
        helperPort(21);
        break;
      case "postgis":
        helperPort(5432);
        break;
      default:
        ;
    }
  };

  $scope.save = function() {
    if (!$scope.form.$valid) {
      angular.forEach($scope.form.$error, function (field) {
        angular.forEach(field, function(errorField){
          errorField.$setTouched();
        })
      });
      return;
    }

    $scope.alertBox.title = "Data Provider Registration";
    $scope.errorFound = false;
    $http({
      url: configuration.saveConfig.url,
      method: configuration.saveConfig.method,
      data: $scope.dataProvider
    }).success(function(dataProvider) {
      $scope.alertBox.message = "Data Provider has been saved";
      if (!$scope.isEditing) {
        $scope.dataProvider = dataProvider;
      } else {

      }
      $scope.isEditing = true;
      console.log(dataProvider);
    }).error(function(err) {
      $scope.errorFound = true;
      $scope.alertBox.message = err.message;
      console.log(err);
    });
  };
  $scope.checkConnection = function() {
    $scope.isChecking = true; // for handling loading page
    $scope.resetState = function() {
      $scope.errorFound = false;
      $scope.alertBox.message = "";
    };

    // Timeout in seconds for handling connections
    $scope.timeOutSeconds = 8;

    // Function for requests success, error and timeout
    var makeRequest = function() {
      var timeOut = $q.defer();
      var result = $q.defer();
      var expired = false;
      setTimeout(function() {
        expired = true;
        timeOut.resolve();
      }, 1000 * $scope.timeOutSeconds);

      var httpRequest = $http({
        method: "POST",
        url: "/uri/",
        data: $scope.dataProvider,
        timeout: timeOut.promise
      });

      httpRequest.success(function(data) {
        result.resolve(data);
      });

      httpRequest.error(function(err) {
        if (expired)
          result.reject({message: "Timeout: Request took longer than " + $scope.timeOutSeconds + " seconds."});
        else
          result.reject(err);
      });

      return result.promise;
    };

    var request = makeRequest();

    request.then(function(data) {
      $scope.alertBox.title = "Connection Status";
      if (data.message){ // error found
        $scope.errorFound = true;
        $scope.alertBox.message = data.message;
      } else {
        $scope.errorFound = false;
        $scope.alertBox.message = "Connection Successful";
      }
    }).catch(function(err) {
      $scope.alertBox.title = "Connection Status";
      $scope.errorFound = true;
      $scope.alertBox.message = err.message;
    }).finally(function() {
      $scope.isChecking = false;
    });
  };
}]);