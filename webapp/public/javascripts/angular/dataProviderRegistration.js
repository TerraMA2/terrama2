'use strict';

var app = angular.module("terrama2.dataprovider.registration", ['schemaForm']);

app.controller("RegisterController", ["$scope", "$http", "$q", function($scope, $http, $q) {
  $scope.model = {};
  $scope.schema = {};
  $scope.form = [];

  $http.get("/api/DataProviderType/", {}).success(function(typeList) {
    typeList.forEach(function(dataProviderType) {
      $scope.typeList = typeList;
    });

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
  $scope.protocol = configuration.dataProvider.kind,
  $scope.dataProvider = {
    name: configuration.dataProvider.name,
    description: configuration.dataProvider.description,
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

  $scope.onSchemeChanged = function() {
    // todo: disable fields?
    $scope.typeList.forEach(function(dataProviderType) {
      if (dataProviderType.name === $scope.protocol) {
        $scope.model = {};
        $scope.schema = {
          type: "object",
          properties: dataProviderType.properties,
          required: dataProviderType.required || []
        };

        if (dataProviderType.display)
          $scope.form = dataProviderType.display;
        else
          $scope.form = ["*"];

        $scope.$broadcast('schemaFormRedraw');
      }
    });
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
      $scope.isEditing = true;
      console.log(dataProvider);
    }).error(function(err) {
      $scope.errorFound = true;
      $scope.alertBox.message = err.message;
      console.log(err);
    });
  };

  $scope.resetState = function() {
    $scope.errorFound = false;
    $scope.alertBox.message = "";
  };

  $scope.checkConnection = function(form) {
    $scope.$broadcast('schemaFormValidate');

    if (!form.$valid)
      return;

    $scope.isChecking = true; // for handling loading page

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

      var params = $scope.model;
      params.protocol = $scope.protocol;

      var httpRequest = $http({
        method: "POST",
        url: "/uri/",
        data: params,
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