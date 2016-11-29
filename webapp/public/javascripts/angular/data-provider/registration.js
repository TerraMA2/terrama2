'use strict';

var app = angular.module("terrama2.dataprovider.registration", ['terrama2', 'schemaForm', 'terrama2.components.messagebox']);

app.controller("RegisterController", ["$scope", "$http", "$q", "$window", "$httpParamSerializer", "$location", "i18n", "$timeout",
  function($scope, $http, $q, $window, $httpParamSerializer, $location, i18n, $timeout) {
    $scope.i18n = i18n;
    var model = {};

    var conf = configuration;
    if (conf.dataProvider.uriObject) {
      for(var k in conf.dataProvider.uriObject) {
        if (conf.dataProvider.uriObject.hasOwnProperty(k)) {
          model[k] = conf.dataProvider.uriObject[k];
        }
      }
    }

    // forcing port value to number
    if (model.port) {
      model.port = parseInt(model.port);
    }

    $scope.model = model;
    $scope.serverErrors = {};

    $scope.forms = {};
    $scope.css = {
      boxType: "box-solid"
    }

    if (conf.fields) {
      $scope.schema = {
      type: "object",
        properties: conf.fields.properties,
        required: conf.fields.required
      };

      $scope.options = {};
    } else {
      $scope.schema = {};
    }

    $scope.form = conf.fields.display || [];

    //  redraw form
    if ($scope.form) {
      $scope.$broadcast('schemaFormRedraw');
    }

    $scope.schemeList = [];
    $http.get("/api/DataProviderType/", {}).success(function(typeList) {
      $scope.typeList = typeList;
    }).error(function(err) {
      console.log("err type: ", err);
    });

    var makeRedirectUrl = function(extra) {
      var redirectUrl = conf.redirectTo.redirectTo || "/configuration/providers/";
      redirectUrl += (redirectUrl.indexOf('?') === -1) ? '?' : '&';

      var redirectData = Object.assign(conf.redirectTo, extra instanceof Object ? extra : {});
      delete redirectData.redirectTo;

      return redirectUrl + $httpParamSerializer(redirectData);
    }

    $scope.redirectUrl = makeRedirectUrl();
  
    $scope.errorFound = false;
    $scope.isEditing = conf.isEditing;
    $scope.alertBox = {};
    $scope.isChecking = false;
    $scope.message = "";
    $scope.remoteFieldsRequired = false;
    $scope.dataProvider = {
      name: conf.dataProvider.name,
      description: conf.dataProvider.description,
      project: conf.project,
      active: conf.dataProvider.active,
      protocol: conf.dataProvider.data_provider_type_name
    };

    $scope.initActive = function() {
      $scope.dataProvider.active = (conf.dataProvider.active === false || conf.dataProvider.active) ? conf.dataProvider.active : true;
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

          if (dataProviderType.display) {
            $scope.form = dataProviderType.display;
          } else {
            $scope.form = ["*"];
          }

          $scope.$broadcast('schemaFormRedraw');
          $timeout(function(){
            if ($scope.dataProvider.protocol === "POSTGIS"){
              var databaseInput = angular.element('#database');
              databaseInput.attr('list', 'databaseList');
            }
          });
        }
      });
    };

    // listen connection data to get database list
    
    $scope.dbList = [];
    var timeoutPromise;
    $scope.$watch('model', function(){
      $timeout.cancel(timeoutPromise);
      timeoutPromise = $timeout(function(){
        if ($scope.dataProvider.protocol !== "POSTGIS" || !$scope.forms.connectionForm.hostname || !$scope.forms.connectionForm.port || !$scope.forms.connectionForm.user){
          return;
        }
        getDatabaseList();
      }, 1000);
    }, true);

    var getDatabaseList = function(){
      var result = $q.defer();

      var params = $scope.model;
      params.protocol = $scope.dataProvider.protocol;
      params.objectToGet = "database";

      var httpRequest = $http({
        method: "GET",
        url: "/uri/",
        params: params
      });

      httpRequest.success(function(data) {
        $scope.dbList = data.data.map(function(item, index){
          return item.datname;
        });
        result.resolve(data);
      });

      httpRequest.error(function(err) {
        result.reject(err);
      });

      return result.promise;
    }

    $scope.isValidDataProviderTypeForm = function(form) {
      return $scope.forms.connectionForm.$valid;
    };

    $scope.save = function() {
      $scope.resetState();
      $scope.$broadcast('formFieldValidation');

      // calling auto generate form validation
      $scope.$broadcast('schemaFormValidate');

      var isConnectionFormValid = $scope.isValidDataProviderTypeForm($scope.forms.connectionForm);
      if (!$scope.forms.dataProviderForm.$valid || !isConnectionFormValid) {
        $scope.alertBox.title = i18n.__("Data Server Registration");
        $scope.alertBox.message = i18n.__("There are invalid fields on form");
        $scope.errorFound = true;
        return;
      }

      $scope.alertBox.title = i18n.__("Data Server Registration");
      $scope.message = "";
      $scope.errorFound = false;

      var formData = $scope.dataProvider;
      formData.uriObject = Object.assign({protocol: $scope.dataProvider.protocol}, $scope.model);

      $http({
        url: conf.saveConfig.url,
        method: conf.saveConfig.method,
        data: formData
      }).success(function(data) {
        $scope.isEditing = true;

        var defaultRedirectTo = "/configuration/providers?id=" + data.result.id + "&method=" + conf.saveConfig.method + "&";

        var redirectData = makeRedirectUrl({data_provider_id: data.result.id}) + "&token=" + data.token;

        // disable fields
        $scope.options = {formDefaults: {readonly: true}};

        $window.location.href = (redirectData || defaultRedirectTo);
      }).error(function(err) {
        $scope.errorFound = true;
        $scope.alertBox.message = err.message;
        $scope.serverErrors = err.errors || {};
        console.log(err);
      });
    };

    $scope.resetState = function() {
      $scope.errorFound = false;
      $scope.alertBox.message = "";
    };

    $scope.checkConnection = function(form) {
      $scope.model = $scope.model;
      $scope.$broadcast('schemaFormValidate');

      if (!$scope.isValidDataProviderTypeForm(form)) {
        return;
      }

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
          if (expired) {
            result.reject({message: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds.")});
          } else {
            result.reject(err);
          }
        });

        return result.promise;
      };

      var request = makeRequest();

      $scope.alertBox.title = i18n.__("Connection Status");

      request.then(function(data) {
        if (data.message){ // error found
          $scope.errorFound = true;
          $scope.alertBox.message = data.message;
        } else {
          $scope.errorFound = false;
          $scope.alertBox.message = i18n.__("Connection Successful");
        }
      }).catch(function(err) {
        $scope.errorFound = true;
        $scope.alertBox.message = err.message;
      }).finally(function() {
        $scope.isChecking = false;
      });
    };
  }]);
