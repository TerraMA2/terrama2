'use strict';

var app = angular.module("terrama2.dataprovider.registration", ['terrama2', 'schemaForm', 'terrama2.components.messagebox']);

app.controller("RegisterController", ["$scope", "$http", "$q", "$window", "$httpParamSerializer", "$location",
  function($scope, $http, $q, $window, $httpParamSerializer, $location) {

  $scope.model = configuration.dataProvider.uriObject || {};

  $scope.forms = {};
  $scope.css = {
    boxType: "box-solid"
  }

  if (configuration.fields) {
    $scope.schema = {
    type: "object",
      properties: configuration.fields.properties,
      required: configuration.fields.required
    };

    $scope.options = {};
  } else
    $scope.schema = {};

  $scope.form = configuration.fields.display || [];

//  redraw form
  if ($scope.form)
    $scope.$broadcast('schemaFormRedraw');

  $scope.schemeList = [];
  $http.get("/api/DataProviderType/", {}).success(function(typeList) {
    $scope.typeList = typeList;
  }).error(function(err) {
    console.log("err type: ", err);
  });

  var makeRedirectUrl = function(extra) {
    var redirectUrl = configuration.redirectTo.redirectTo || "/configuration/providers/";
    redirectUrl += (redirectUrl.indexOf('?') === -1) ? '?' : '&';

    var redirectData = Object.assign(configuration.redirectTo, extra instanceof Object ? extra : {});
    delete redirectData.redirectTo;

    return redirectUrl + $httpParamSerializer(redirectData);
  }

  $scope.redirectUrl = makeRedirectUrl();

  $scope.errorFound = false;
  $scope.isEditing = configuration.isEditing;
  $scope.alertBox = {};
  $scope.isChecking = false;
  $scope.message = "";
  $scope.remoteFieldsRequired = false;
  $scope.dataProvider = {
    name: configuration.dataProvider.name,
    description: configuration.dataProvider.description,
    project: configuration.project,
    active: configuration.dataProvider.active,
    protocol: configuration.dataProvider.data_provider_type_name
  };

  $scope.initActive = function() {
    $scope.dataProvider.active = (configuration.dataProvider.active === false || configuration.dataProvider.active) ? configuration.dataProvider.active : true;
  };

  $scope.onSchemeChanged = function(ref) {
    $scope.typeList.forEach(function(dataProviderType) {
      if (dataProviderType.name === $scope.dataProvider.protocol) {
        // temp code for port changing
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

  $scope.isValidDataProviderTypeForm = function(form) {
    $scope.$broadcast('schemaFormValidate');

    return $scope.forms.connectionForm.$valid;
  };

  $scope.save = function() {
    $scope.$broadcast('formFieldValidation');
    var isConnectionFormValid = $scope.isValidDataProviderTypeForm($scope.forms.connectionForm);
    if (!$scope.forms.dataProviderForm.$valid || !isConnectionFormValid) {
      return;
    }

    $scope.alertBox.title = "Data Provider Registration";
    $scope.errorFound = false;

    var formData = $scope.dataProvider;
    formData.uriObject = Object.assign({protocol: $scope.dataProvider.protocol}, $scope.model);

    $http({
      url: configuration.saveConfig.url,
      method: configuration.saveConfig.method,
      data: formData
    }).success(function(data) {
      $scope.isEditing = true;

      var defaultRedirectTo = "/configuration/providers?id=" + data.result.id + "&method=" + configuration.saveConfig.method + "&";

      var redirectData = makeRedirectUrl({data_provider_id: data.result.id}) + "&token=" + data.token;

      // disable fields
      $scope.options = {formDefaults: {readonly: true}};

      $window.location.href = (redirectData || defaultRedirectTo);
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
    if (!$scope.isValidDataProviderTypeForm(form))
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
      params.protocol = $scope.dataProvider.protocol;

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
