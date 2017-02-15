define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle View form registration.
   * @class ViewRegistration
   */
  function ViewRegisterUpdate($scope, i18n, ViewService, $log, $http, $timeout, MessageBoxService, $window, DataSeriesService, Service, StringFormat, ColorFactory, StyleType) {
    /**
     * @type {ViewRegisterUpdate}
     */
    var self = this;
    /**
     * It retrieves a configuration from main window.
     * 
     * @type {Object}
     */
    var config = window.configuration;

    /**
     * It retrieves Global variables from main window.
     * 
     * @type {Object}
     */
    var Globals = $window.globals;
    /**
     * It handles alert box display
     * 
     * @type {Object}
     */
    self.MessageBoxService = MessageBoxService;
    /**
     * It defines available DataSeries type from DataSeries Service
     * @enum string
     */
    self.DataSeriesType = DataSeriesService.DataSeriesType;
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
     * It handles if should show schedule or not. It may be changed on view data series change
     * 
     * @type {boolean}
     */
    self.isDynamic = false;
    /**
     * It defines a list of Service with type View. It will be filled out in ng-options generation
     * @type {Service[]}
     */
    self.filteredServices = [];
    /**
     * Flag to handle if is Updating or Registering
     * 
     * @type {Boolean}
     */
    self.isUpdating = config.view ? true : false;
    /**
     * It handles data series changed state in order to prevent select DCP or format !== GEOTIFF
     */
    self.isValid = true;
    /**
     * It defines a list of cached data series
     * @type {Object[]}
     */
    self.dataSeries = [];
    /**
     * It configures color picker (Angular Color Picker dependency)
     * @type {Object}
     */
    self.colorOptions = {
      format: 'hex'
    };
    /**
     * It defines a list of available columns to display
     * @type {string[]}
     */
    self.columnsList = [];
    /**
     * It defines a list of colors used to generate legend
     * @type {Object[]}
     */
    self.colors = [];
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

    self.hasStyle = false;

    self.styleButtons = {
      circle: {
        show: function () {
          return !self.hasStyle;
        },
        click: function() {
          self.hasStyle = true;
        }
      },
      minus: {
        show: function () {
          return self.hasStyle;
        },
        click: function() {
          self.hasStyle = false;
          self.legend = {};
        }
      } 
    };
    /**
     * It contains view instance values
     * @type {Object}
     */
    self.view = config.view || {};
    self.targetDataSeriesType = null;
    /**
     * It stores a legend values (Geometric, Grid, etc)
     */
    self.legend = {};
    /**
     * It defines a selected View DataSeries object
     * @type {DataSeries}
     */
    self.viewDataSeries = {};

    // function initializer
    self.onDataSeriesChanged = onDataSeriesChanged;
    self.initActive = initActive;
    // Setting view service dao
    self.ViewService = ViewService;
    // setting message box close fn
    self.close = closeDialog;
    // Setting Save operation attached into submit button
    self.save = saveOperation;

    /**
     * It retrieves all data provider type to get HTTP fields
     */
    $http.get("/api/DataProviderType", {}).then(function(response) {
      var data = response.data;

      if (config.view.legend) {
        self.hasStyle = true;
      }

      /**
       * Retrieve all service instances
       */
      self.ServiceInstance.init().then(function() {
        // setting all view services in cache
        self.filteredServices = self.ServiceInstance.list({'service_type_id': self.ServiceInstance.types.VIEW});
        /**
         * Retrieve all data series
         */
        return DataSeriesService.init({schema: "all"}).then(function(dataSeries) {
          //Filter data series to not show dcp - remove when back implements dcp creation view
          self.dataSeries = dataSeries;

          var styleCache = config.view.style;

          if (self.view.data_series_id) {
            self.onDataSeriesChanged(self.view.data_series_id);

            var legend = config.view.legend;
            if (legend && Object.keys(legend).length !== 0) {
              self.legend.operation_id = legend.operation_id;
              self.legend.type = legend.type;

              if (legend.type !== StyleType.VALUE) {
                legend.colors.forEach(function(color) {
                  if (!color.isDefault) {
                    color.value = parseFloat(color.value);
                  }
                });
              }

              self.legend.colors = legend.colors;
              self.legend.bands = legend.colors.length - 1;
              if (legend.metadata && legend.metadata.band_number) {
                legend.metadata.band_number = parseInt(legend.metadata.band_number);
              }
              self.legend.metadata = legend.metadata;

              // notify component to refil begin/end
              $timeout(function() {
                $scope.$broadcast("updateStyleColor");
              });
            }
          }
          /**
           * Configuring Schema form http. This sentence is important because child controller may be not initialized yet.
           * Using $timeout 0 forces to execute when angular ready state is OK.
           */
          $timeout(function() {
            if (self.isUpdating) {
              self.schedule = {};
              if (self.view.schedule && (self.view.schedule.frequency_unit || self.view.schedule.schedule_unit)){
                self.view.schedule.scheduleType = Globals.enums.ScheduleType.SCHEDULE;
              } else {
                self.view.schedule.scheduleType = Globals.enums.ScheduleType.MANUAL;
              }
              $scope.$broadcast("updateSchedule", self.view.schedule || {});
            } else {
              if (!config.view) {
                // forcing first view pre-selected
                if (self.filteredServices.length > 0) {
                  self.view.service_instance_id = self.filteredServices[0].id;
                }
              }
            }
          });
        });
      });
    }).catch(function(err) {
      $log.info(err);
      self.MessageBoxService.danger(i18n.__("View"), err);
    });

    /**
     * It is used on ng-init active view. It will wait for angular ready condition and set active view checkbox
     * 
     * @returns {void}
     */    
    function initActive() {
      // wait angular digest cycle
      $timeout(function() {
        self.view.active = (config.view.active === false || config.view.active) ? config.view.active : true;
      });
    }

    /**
     * It handles Data Series combobox change. If it is GRID data series, there is a default style script
     * @param {DataSeries}
     */    
    function onDataSeriesChanged(dataSeriesId) {
      self.dataSeries.some(function(dSeries) {
        if (dSeries.id === dataSeriesId) {
          // reset message box
          self.close();
          // setting view data series
          self.viewDataSeries = dSeries;
          // setting target data series type name in order to display style view
          self.targetDataSeriesType = dSeries.data_series_semantics.data_series_type_name;
          // extra comparison just to setting if it is dynamic or static.
          // Here avoids to setting to true in many cases below
          self.isDynamic = dSeries.data_series_semantics.data_series_type_name !== DataSeriesService.DataSeriesType.GEOMETRIC_OBJECT;
          if (dSeries.data_series_semantics.data_series_format_name === "GEOTIFF") {
            self.isValid = false;
            MessageBoxService.danger(i18n.__("View"), i18n.__("You selected a GRID data series. Only GEOTIFF data series format are supported"));
          } else if (dSeries.data_series_semantics.data_series_type_name === DataSeriesService.DataSeriesType.DCP) {
            self.isValid = false;
            MessageBoxService.danger(i18n.__("View"), i18n.__("DCP data series is not supported yet"));
          } else {
            self.isValid = true;
          }

          // breaking loop
          return true;
        }
      });
    }
    /**
     * Helper to reset alert box instance
     */
    function closeDialog() {
      self.MessageBoxService.reset();
    }
    /**
     * It performs a save operation. It applies a form validation and try to save
     * @param {boolean} shouldRun - Determines if service should auto-run after save process
     * @returns {void}
     */
    function saveOperation(shouldRun) {
      // broadcasting each one terrama2 field directive validation 
      $scope.$broadcast("formFieldValidation");
      // broadcasting schema form validation
      $scope.$broadcast("schemaFormValidate");

      if (!self.isValid) {
        return;
      }

      $timeout(function(){
        $scope.$apply(function() {
          if ($scope.forms.viewForm.$invalid || 
            $scope.forms.dataSeriesForm.$invalid ||
            $scope.forms.styleForm && $scope.forms.styleForm.$invalid) {
            return;
          }

          if (Object.keys(self.legend).length !== 0) {
            if (!self.legend.colors || self.legend.colors.length === 0) {
              return MessageBoxService.danger(i18n.__("View"), i18n.__("You must generate the style colors to classify Data Series"));
            }
            for(var i = 0; i < self.legend.colors.length; ++i) {
              var colorIt = self.legend.colors[i];
              if (colorIt.isDefault) {
                continue;
              }
              for(var j = i + 1; j < self.legend.colors.length; ++j) {
                if (self.legend.colors[j].value == colorIt.value) {
                  return MessageBoxService.danger(i18n.__("View"), i18n.__("The colors must have unique values"));
                }
              }
            }
          }

          // If dynamic, schedule validation is required
          if (self.isDynamic) {
            if (self.view.schedule && Object.keys(self.view.schedule).length !== 0) {
              /**
               * @todo Implement Angular ScheduleService to handle it, since is common on dynamic data series and analysis registration.
               */
              var scheduleForm = angular.element('form[name="scheduleForm"]').scope()['scheduleForm'];
              // form validation
              if (scheduleForm.$invalid) {
                return;
              }

              // preparing schedule.  
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
            }
          } // end if isDynamic

          /**
           * It contains a view model with flag "run" to determines if service should run
           * @type {Object}
           */
          var mergedView = angular.merge(self.view, {run: shouldRun});
          mergedView.legend = self.legend;
          // tries to save
          var operation = self.isUpdating ? self.ViewService.update(self.view.id, self.view) : self.ViewService.create(self.view);
          operation.then(function(response) {
            $log.info(response);
            $window.location.href = "/configuration/views?token=" + response.token;
          }).catch(function(err) {
            $log.info(err);
            self.MessageBoxService.danger(i18n.__("View"), err);
          });
        });
      });
    }
  }

  ViewRegisterUpdate.$inject = ["$scope", "i18n", "ViewService", "$log", "$http", "$timeout", "MessageBoxService", "$window", 
    "DataSeriesService", "Service", "StringFormat", "ColorFactory", "StyleType"];

  return ViewRegisterUpdate;
});