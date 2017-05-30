define(function() {
  function RegisterController($scope, $http, $q, $window, $httpParamSerializer, $location, i18n, $timeout, DataProviderService, MessageBoxService, FormTranslator) {
    $scope.i18n = i18n;
    var model = {};
    var title = "Data Server Registration";
    $scope.MessageBoxService = MessageBoxService;

    var conf = $window.configuration;
    if (conf.dataProvider.uriObject) {
      for(var k in conf.dataProvider.uriObject) {
        if (conf.dataProvider.uriObject.hasOwnProperty(k)) {
          model[k] = conf.dataProvider.uriObject[k];
        }
      }
    }

    if (conf.dataProvider.data_provider_type_name == 'FTP'){
      model['active_mode'] = conf.dataProvider.active_mode;
      model['timeout'] = conf.dataProvider.timeout;
    }

    // forcing port value to number
    if (model.port) {
      model.port = parseInt(model.port);
    }

    $scope.model = model;

    $scope.forms = {};
    $scope.css = {
      boxType: "box-solid"
    }

    $timeout(function() {
      if(conf.fields) {
        if(conf.fields.display) {
          var formTranslatorResult = FormTranslator(conf.fields.properties, conf.fields.display, conf.fields.required);
          var propertiesLocale = formTranslatorResult.object;
          var fieldsForm = formTranslatorResult.display;
        } else {
          var propertiesLocale = FormTranslator(conf.fields.properties);
          var fieldsForm = [];
        }

        $scope.schema = {
          type: "object",
          properties: propertiesLocale,
          required: conf.fields.required
        };
        $scope.options = {};
        $scope.form = fieldsForm;
      } else {
        $scope.schema = {};
        $scope.form = [];
      }
    }, 1000);

    //  redraw form
    if ($scope.form) {
      $scope.$broadcast("schemaFormRedraw");
    }

    $scope.schemeList = [];
    $http.get(BASE_URL + "api/DataProviderType/", {}).then(function(response) {
      $scope.typeList = response.data;
    }).catch(function(response) {
      console.log("err type: ", response.data);
    });

    var makeRedirectUrl = function(extra) {
      var redirectUrl = conf.redirectTo.redirectTo || BASE_URL + "configuration/providers/";
      redirectUrl += (redirectUrl.indexOf("?") === -1) ? "?" : "&";

      var redirectData = Object.assign(conf.redirectTo, extra instanceof Object ? extra : {});
      delete redirectData.redirectTo;

      return redirectUrl + $httpParamSerializer(redirectData);
    }

    $scope.redirectUrl = makeRedirectUrl();
  
    $scope.errorFound = false;
    $scope.isEditing = conf.isEditing;
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
          if(dataProviderType.display) {
            var formTranslatorResult = FormTranslator(dataProviderType.properties, dataProviderType.display, dataProviderType.required);
            var propertiesLocale = formTranslatorResult.object;
            var fieldsForm = formTranslatorResult.display;
          } else {
            var propertiesLocale = FormTranslator(dataProviderType.properties);
            var fieldsForm = ["*"];
          }

          $scope.model = {};
          $scope.schema = {
            type: "object",
            properties: propertiesLocale,
            required: dataProviderType.required || []
          };

          $scope.form = fieldsForm;

          $scope.$broadcast("schemaFormRedraw");
          $timeout(function() {
            if($scope.dataProvider.protocol === "POSTGIS") {
              var databaseInput = angular.element("#database");
              databaseInput.attr("list", "databaseList");
            }
          });
        }
      });
    };

    // listen connection data to get database list
    
    $scope.dbList = [];
    var timeoutPromise;
    $scope.$watch("model", function(){
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
        url: BASE_URL + "uri/",
        params: params
      });

      httpRequest.then(function(response) {
        $scope.dbList = response.data.data.map(function(item, index){
          return item.datname;
        });
        result.resolve(response.data);
      });

      httpRequest.catch(function(response) {
        result.reject(response.data);
      });

      return result.promise;
    }

    $scope.isValidDataProviderTypeForm = function(form) {
      return $scope.forms.connectionForm.$valid;
    };

    $scope.save = function() {
      $scope.close();
      $scope.$broadcast("formFieldValidation");

      // calling auto generate form validation
      $scope.$broadcast("schemaFormValidate");

      var isConnectionFormValid = $scope.isValidDataProviderTypeForm($scope.forms.connectionForm);
      if(!$scope.forms.dataProviderForm.$valid || !isConnectionFormValid) {
        return MessageBoxService.danger(i18n.__(title), i18n.__("There are invalid fields on form"));
      }

      var formData = $scope.dataProvider;
      formData.uriObject = Object.assign({protocol: $scope.dataProvider.protocol}, $scope.model);

      $http({
        url: conf.saveConfig.url,
        method: conf.saveConfig.method,
        data: formData
      }).then(function(response) {
        $scope.isEditing = true;

        var defaultRedirectTo = BASE_URL + "configuration/providers?id=" + response.data.result.id + "&method=" + conf.saveConfig.method + "&";

        var redirectData = makeRedirectUrl({data_provider_id: response.data.result.id}) + "&token=" + response.data.token;

        // disable fields
        $scope.options = {formDefaults: {readonly: true}};

        $window.location.href = (redirectData || defaultRedirectTo);
      }).catch(function(response) {
        return MessageBoxService.danger(i18n.__(title), i18n.__(response.data.message));
      });
    };

    $scope.close = function() {
      MessageBoxService.reset();
    };

    $scope.checkConnection = function(form) {
      $scope.model = $scope.model;
      $scope.$broadcast("schemaFormValidate");

      if(!$scope.isValidDataProviderTypeForm(form)) {
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
          url: BASE_URL + "uri/",
          data: params,
          timeout: timeOut.promise
        });

        httpRequest.then(function(response) {
          result.resolve(response.data);
        });

        httpRequest.catch(function(response) {
          if(expired) {
            result.reject({message: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds."), translated: true});
          } else {
            result.reject(response.data);
          }
        });

        return result.promise;
      };

      var request = makeRequest();

      var connectionTitle = i18n.__("Connection Status");

      request.then(function(data) {
        if(data.message) { // error found
          MessageBoxService.danger(connectionTitle, (data.translated ? data.message : i18n.__(data.message)));
        } else {
          MessageBoxService.success(connectionTitle, i18n.__("Connection Successful"));
        }
      }).catch(function(err) {
        MessageBoxService.danger(connectionTitle, (err.translated ? err.message : i18n.__(err.message)));
      }).finally(function() {
        $scope.isChecking = false;
      });
    };
  }

  RegisterController.$inject = ["$scope", "$http", "$q", "$window", "$httpParamSerializer", "$location", "i18n", "$timeout", "DataProviderService", "MessageBoxService", "FormTranslator"];

  return RegisterController;
});