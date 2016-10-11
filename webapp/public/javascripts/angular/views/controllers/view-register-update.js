(function() {
  'use strict';

  angular.module("terrama2.views.controllers.viewregisterupdate", [
      "terrama2",
      "terrama2.services",
      "terrama2.views.services",
      "schemaForm",
      'terrama2.datetimepicker',
      "terrama2.dataseries.services",
      "terrama2.schedule",
      "terrama2.administration.services.iservices",
      "terrama2.ace",
      "terrama2.components.messagebox.services",
      "terrama2.components.messagebox"])
    .controller("ViewRegisterUpdate", ViewRegisterUpdate)

  /**
   * It represents a Controller to handle View form registration.
   * @class ViewRegistration
   */
  function ViewRegisterUpdate($scope, i18n, ViewService, $log, $http, $timeout, MessageBoxService, $window, DataSeriesService, Service) {
    /**
     * @type {ViewRegisterUpdate}
     */
    var self = this;

    var config = window.configuration;

    /**
     * It handles alert box display
     * 
     * @type {Object}
     */
    self.MessageBoxService = MessageBoxService;

    /**
     * It handles Service Instance model
     * 
     * @type {Object}
     */
    self.ServiceInstance = Service;

    /** 
     * @type {Object}
     */
    self.scheduleOptions = {};

    /**
     * Flag to handle if is Updating or Registering
     * 
     * @type {Boolean}
     */
    self.isUpdating = config.view ? true : false;

    /**
     * It defines a list of cached data series
     * @type {Object[]}
     */
    self.dataSeries = [];

    /**
     * It retrieves all data provider type to get HTTP fields
     */
    $http.get("/api/DataProviderType", {}).success(function(data) {
      // Setting HTTP default syntax
      self.httpSyntax = data.find(function(element) {
        return element.name === "HTTP";
      });

      /**
       * Retrieve all service instances
       */
      self.ServiceInstance.init().then(function() {

        /**
         * Retrieve all data series
         */
        return DataSeriesService.list({schema: "all"}).then(function(dataSeries) {
          self.dataSeries = dataSeries;

          /**
           * Configuring Schema form http. This sentence is important because child controller may be not initialized yet.
           * Using $timeout 0 forces to execute when angular ready state is OK.
           */
          $timeout(function() {
            $scope.schema = {
              type: "object",
              properties: self.httpSyntax.properties,
              required: self.httpSyntax.required || []
            };

            if (self.isUpdating) {
              self.schedule = {};
              $scope.$broadcast("updateSchedule", self.view.schedule || {});
            }

            $scope.model = config.view ? config.view.serverUriObject || {} : {port: 8080};

            if (self.httpSyntax.display) {
              $scope.form = self.httpSyntax.display;
            } else {
              $scope.form = ["*"];
            }

            $scope.$broadcast('schemaFormRedraw');
          });
        });
      });
    }).catch(function(err) {
      $log.info(err);
      self.MessageBoxService.danger(i18n.__("View"), err);
    });

    // Setting view service dao
    self.ViewService = ViewService;
    /**
     * It is used on ng-init active view. It will wait for angular ready condition and set active view checkbox
     * 
     * @returns {void}
     */
    self.initActive = function() {
      $timeout(function() {
        self.view.active = (self.view.active === false || !config.view.active) ? false : true;
      });
    };

    /**
     * It contains all forms. It must be appended on scope instance due schema form support;
     * @type {Object}
     */
    $scope.forms = {};
    /**
     * It represents a terrama2 box styles
     * @type {Object}
     */
    self.css = {
      boxType: "box-solid"
    };

    /**
     * It contains view instance values
     * @type {Object}
     */
    self.view = config.view || {};

    /**
     * Helper to reset alert box instance
     */
    self.close = function() {
      self.MessageBoxService.reset();
    }
    /**
     * It handles file upload to retrieve xml style
     */
    self.onFileUploadClick = function() {
      
    };

    /**
     * It performs a save operation. It applies a form validation and try to save
     * @returns {void}
     */
    self.save = function() {
      // broadcasting each one terrama2 field directive validation 
      $scope.$broadcast("formFieldValidation");
      // broadcasting schema form validation
      $scope.$broadcast("schemaFormValidate");

      var scheduleForm = angular.element('form[name="scheduleForm"]').scope()['scheduleForm'];

      if ($scope.forms.viewForm.$invalid || 
          $scope.forms.connectionForm.$invalid ||
          scheduleForm.$invalid ||
          $scope.forms.dataSeriesForm.$invalid) {
        return;
      }

      self.view.serverUriObject = $scope.model;
      self.view.serverUriObject.protocol = self.httpSyntax.name;

      // preparing schedule
      var scheduleValues = self.view.schedule;
      switch(scheduleValues.scheduleHandler) {
        case "seconds":
        case "minutes":
        case "hours":
          scheduleValues.frequency_unit = scheduleValues.scheduleHandler;
          scheduleValues.frequency_start_time = scheduleValues.frequency_start_time ? scheduleValues.frequency_start_time.toISOString() : "";
          break;
        case "weeks":
        case "monthly":
        case "yearly":
          // todo: verify
          var dt = scheduleValues.schedule_time;
          scheduleValues.schedule_unit = scheduleValues.scheduleHandler;
          scheduleValues.schedule_time = moment(dt).format("HH:mm:ss");
          break;

        default:
          break;
      }

      // tries to save
      var operation = self.isUpdating ? self.ViewService.update(self.view.id, self.view) : self.ViewService.create(self.view);
      operation.then(function(response) {
        $log.info(response);
        $window.location.href = "/configuration/views?token=" + response.token
      }).catch(function(err) {
        $log.info(err);
        self.MessageBoxService.danger(i18n.__("View"), err);
      });
    };
  }

    // Injecting Angular Dependencies
  ViewRegisterUpdate.$inject = ["$scope", "i18n", "ViewService", "$log", "$http", "$timeout", "MessageBoxService", "$window", "DataSeriesService", "Service"];
} ());