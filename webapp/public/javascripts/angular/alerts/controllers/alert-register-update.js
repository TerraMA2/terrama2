define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle Alert form registration.
   * @class AlertRegistration
   */
  var AlertRegisterUpdate = function($scope, $q, $window, $log, $http, $timeout, i18n, MessageBoxService, AlertService, DataSeriesService, DataProviderService, AnalysisService, Service, UniqueNumber) {
    /**
     * @type {AlertRegisterUpdate}
     */
    var self = this;

    /**
     * It retrieves a configuration from main window.
     *
     * @type {object}
     */
    var config = window.configuration;

    /**
     * It retrieves Global variables from main window.
     * 
     * @type {object}
     */
    var Globals = $window.globals;

    /**
     * It handles alert box display
     *
     * @type {object}
     */
    self.MessageBoxService = MessageBoxService;

    /**
     * It handles Service Instance model
     *
     * @type {object}
     */
    self.ServiceInstance = Service;

    /**
     * Flag to handle if is Updating or Registering
     *
     * @type {boolean}
     */
    self.isUpdating = config.alert ? true : false;

    /**
     * It handles legend levels errors
     *
     * @type {boolean}
     */
    self.isNotValid = true;

    /**
     * It defines a list of cached data series
     * 
     * @type {object[]}
     */
    self.dataSeries = [];

    /**
     * It represents a terrama2 box styles
     * 
     * @type {object}
     */
    self.css = {
      boxType: "box-solid"
    };

    /**
     * It handles Alert Service Instance model
     *
     * @type {object}
     */
    self.AlertService = AlertService;

    /**
     * It contains alert instance values
     * 
     * @type {object}
     */
    self.alert = config.alert || {
      conditional_schedule: {
        data_ids: [],
        scheduleType: 4
      },
      notifications: [
        {
          notify_on_change: false,
          simplified_report: false
        }
      ]
    };

    /**
     * It contains the analysis table columns list
     * 
     * @type {array}
     */
    self.columnsList = [];

    /**
     * It contains the type of the selected Data Series
     * 
     * @type {string}
     */
    self.dataSeriesType = null;

    /**
     * It contains the pre existent legends
     * 
     * @type {array}
     */
    self.legends = [
      {
        id: 0,
        name: i18n.__("New Legend"),
        description: "",
        levels: [
          {
            _id: UniqueNumber(),
            name: "",
            value: ""
          }
        ]
      }
    ];

    $q.all([
      i18n.ensureLocaleIsLoaded(),
      DataSeriesService.init({schema: "all"}),
      DataProviderService.init(),
      AnalysisService.init()
    ]).then(function() {
      /**
       * Retrieve all service instances
       */
      return self.ServiceInstance.init().then(function() {
        // Setting all alert services in cache
        self.filteredServices = self.ServiceInstance.list({'service_type_id': self.ServiceInstance.types.ALERT});

        /**
         * Retrieve data series
         */
        self.dataSeries = DataSeriesService.list().filter(function(dataSeriesToFilter) {
          return dataSeriesToFilter.data_series_semantics.temporality === "DYNAMIC" && (dataSeriesToFilter.data_series_semantics.data_series_type_name === "ANALYSIS_MONITORED_OBJECT" || dataSeriesToFilter.data_series_semantics.data_series_type_name === "GRID");
        });

        var legendRequest = $http({
          method: "GET",
          url: BASE_URL + "api/Legend"
        });

        legendRequest.then(function(response) {
          for(var i = 0, legendsLength = response.data.length; i < legendsLength; i++) {
            var legend = response.data[i];

            legend.levels.sort(function(a, b) {
              if(a.level < b.level) return -1;
              if(a.level > b.level) return 1;
              return 0;
            });

            for(var j = 0, levelsLength = legend.levels.length; j < levelsLength; j++) {
              legend.levels[j]._id = UniqueNumber();
              delete legend.levels[j].legend_id;
            }

            self.legends.push(legend);
          }

          if(self.isUpdating) {
            for(var i = 0, legendsLength = self.legends.length; i < legendsLength; i++) {
              if(self.legends[i].id === self.alert.legend.id) {
                self.legendModel = self.legends[i];

                for(var j = 0, levelsLength = self.legendModel.levels.length; j < levelsLength; j++) {
                  if(self.legendModel.levels[j].level === self.alert.notifications[0].notify_on_legend_level) {
                    self.notify_on_legend_level = self.legendModel.levels[j]._id;
                    break;
                  }
                }

                $timeout(function() {
                  self.onLegendsChange();
                });

                break;
              }
            }

            for(var i = 0, dataSeriesLength = self.dataSeries.length; i < dataSeriesLength; i++) {
              if(self.dataSeries[i].id === self.alert.data_series_id) {
                setDataSeriesData(self.dataSeries[i]);
                break;
              }
            }

            if(self.alert.notifications[0].include_report !== null)
              self.includeReport = true;

            if(self.alert.notifications[0].notify_on_legend_level !== null)
              self.notifyOnLegendLevel = true;
          } else {
            self.legendModel = self.legends[0];

            $timeout(function() {
              self.onLegendsChange();

              // Forcing first alert service pre-selected
              if(self.filteredServices.length > 0)
                self.alert.service_instance_id = self.filteredServices[0].id;
            });
          }
        });
      });
    });

    /**
     * It is used on ng-init active alert. It will wait for angular ready condition and set active alert checkbox
     *
     * @returns {void}
     */
    self.initActive = function() {
      // wait angular digest cycle
      $timeout(function() {
        self.alert.active = (config.alert.active === false || config.alert.active) ? config.alert.active : true;
      });
    };

    /**
     * Returns the image url for a given Data Series
     *
     * @returns {string}
     */
    self.getImageUrl = function(dataSeries) {
      if(typeof dataSeries != 'object') return '';

      switch(dataSeries.data_series_semantics.data_series_type_name) {
        case DataSeriesService.DataSeriesType.DCP:
          return BASE_URL + "images/dynamic-data-series/dcp/dcp.png";
          break;
        case DataSeriesService.DataSeriesType.OCCURRENCE:
          return BASE_URL + "images/dynamic-data-series/occurrence/occurrence.png";
          break;
        case DataSeriesService.DataSeriesType.GRID:
          if(dataSeries.data_series_semantics.temporality == "STATIC") {
            return BASE_URL + "images/static-data-series/grid/grid.png";
            break;
          } else {
            if(dataSeries.isAnalysis)
              return BASE_URL + "images/analysis/grid/grid_analysis.png";
            else
              return BASE_URL + "images/dynamic-data-series/grid/grid.png";

            break;
          }
        case DataSeriesService.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
          return BASE_URL + "images/analysis/monitored-object/monitored-object_analysis.png";
          break;
        case DataSeriesService.DataSeriesType.POSTGIS:
        case DataSeriesService.DataSeriesType.GEOMETRIC_OBJECT:
          return BASE_URL + "images/static-data-series/vetorial/vetorial.png";
          break;
        default:
          return BASE_URL + "images/dynamic-data-series/occurrence/occurrence.png";
          break;
      }
    };

    /**
     * It handles Legend combobox change.
     * 
     * @returns {void}
     */
    self.onLegendsChange = function() {
      for(var i = 0, legendsLength = self.legends.length; i < legendsLength; i++) {
        if(self.legends[i].id === self.legendModel.id) {
          if(self.legendModel.id === 0) {
            self.legends[i].name = i18n.__("New Legend");
            self.legends[i].description = "";
            self.legends[i].levels = [
              {
                _id: UniqueNumber(),
                name: "",
                value: ""
              }
            ];
          }

          self.legendModel = $.extend(true, {}, self.legends[i]);
          break;
        }
      }
    };

    /**
     * It handles DataSeries combobox change.
     * 
     * @returns {void}
     */
    self.onDataSeriesChange = function() {
      var dataSeries = self.dataSeries.filter(function(dataSeriesToFilter) {
        return dataSeriesToFilter.id == self.alert.data_series_id;
      });

      if(dataSeries.length > 0) {
        setDataSeriesData(dataSeries[0]);
      }
    };

    /**
     * Sets DataSeries data when a DataSeries is selected.
     *
     * @returns {void}
     */
    var setDataSeriesData = function(dataSeries) {
      if(dataSeries.data_series_semantics.data_series_type_name === "ANALYSIS_MONITORED_OBJECT") {
        var dataProvider = DataProviderService.list().filter(function(dataProvider) {
          return dataProvider.id == dataSeries.data_provider_id;
        });

        if(dataProvider.length > 0)
          self.alert.project_id = dataProvider[0].project_id;

        var analysis = AnalysisService.list().filter(function(analysisToFilter) {
          return analysisToFilter.output_dataset_id == dataSeries.dataSets[0].id;
        });

        if(dataProvider.length > 0 && analysis.length > 0) {
          listColumns(dataProvider[0], dataSeries.dataSets[0].format.table_name);
        }

        if(self.isUpdating)
          self.legend_attribute_mo = self.alert.legend_attribute;
      } else {
        self.columnsList = [];

        if(self.isUpdating)
          self.legend_attribute_grid = parseInt(self.alert.legend_attribute);
      }

      if(!self.isUpdating) {
        delete self.legend_attribute_grid;
        delete self.legend_attribute_mo;
      }

      self.dataSeriesType = dataSeries.data_series_semantics.data_series_type_name;
      self.alert.conditional_schedule.data_ids = [dataSeries.id];
    };

    /**
     * It creates a new level in the current legend.
     * 
     * @returns {void}
     */
    self.newLevel = function() {
      var uniqueNumberValue = UniqueNumber();

      self.legendModel.levels.push({
        _id: uniqueNumberValue,
        name: "",
        value: ""
      });
    };

    /**
     * It removes a given level from the current legend.
     * 
     * @returns {void}
     */
    self.removeLevel = function(level) {
      for(var j = 0, levelsLength = self.legendModel.levels.length; j < levelsLength; j++) {
        if(self.legendModel.levels[j]._id === level._id) {
          self.legendModel.levels.splice(j, 1);
          break;
        }
      }
    };

    /**
     * Lists the columns from a given table.
     * 
     * @returns {void}
     */
    var listColumns = function(dataProvider, tableName) {
      var result = $q.defer();

      var params = getPostgisUriInfo(dataProvider.uri);
      params.objectToGet = "column";
      params.table_name = tableName;

      var httpRequest = $http({
        method: "GET",
        url: BASE_URL + "uri/",
        params: params
      });

      httpRequest.then(function(response) {
        self.columnsList = response.data.data.map(function(item, index) {
          return item.column_name;
        });

        result.resolve(response.data.data);
      });

      httpRequest.catch(function(err) {
        result.reject(err);
      });

      return result.promise;
    };

    /**
     * Helper function to parse a PostGIS URI.
     * 
     * @returns {object}
     */
    var getPostgisUriInfo = function(uri) {
      var params = {};
      params.protocol = uri.split(':')[0];
      var hostData = uri.split('@')[1];

      if(hostData) {
        params.hostname = hostData.split(':')[0];
        params.port = hostData.split(':')[1].split('/')[0];
        params.database = hostData.split('/')[1];
      }

      var auth = uri.split('@')[0];

      if(auth) {
        var userData = auth.split('://')[1];

        if(userData) {
          params.user = userData.split(':')[0];
          params.password = userData.split(':')[1];
        }
      }

      return params;
    };

    /**
     * Watcher for handling legend levels change. It validates if the values are numeric and are in a growing order.
     */
    $scope.$watch("ctrl.legendModel.levels", function() {
      if(!self.legendModel)
        return;

      var lastValue = null;
      self.legendLevelOrderError = false;
      self.isNotValid = false;

      if(self.legendLevelValueError === undefined) self.legendLevelValueError = {};
      if(self.legendLevelNameError === undefined) self.legendLevelNameError = {};

      for(var i = 0, levelsLength = self.legendModel.levels.length; i < levelsLength; i++) {
        if(isNaN(self.legendModel.levels[i].value) || self.legendModel.levels[i].value === "") {
          self.legendLevelValueError[self.legendModel.levels[i]._id] = true;
          self.isNotValid = true;
        } else if(lastValue !== null && parseFloat(lastValue) > parseFloat(self.legendModel.levels[i].value)) {
          self.legendLevelOrderError = true;
          self.legendLevelValueError[self.legendModel.levels[i]._id] = false;
          lastValue = self.legendModel.levels[i].value;
          self.isNotValid = true;
        } else {
          self.legendLevelValueError[self.legendModel.levels[i]._id] = false;
          lastValue = self.legendModel.levels[i].value;
        }

        if(self.legendModel.levels[i].name === undefined || self.legendModel.levels[i].name === "") {
          self.legendLevelNameError[self.legendModel.levels[i]._id] = true;
          self.isNotValid = true;
        } else {
          self.legendLevelNameError[self.legendModel.levels[i]._id] = false;
        }
      }
    }, true);

    /**
     * Helper to reset alert box instance.
     * 
     * @returns {void}
     */
    self.close = function() {
      self.MessageBoxService.reset();
    };

    /**
     * Saves the alert.
     * 
     * @returns {void}
     */
    self.save = function(shouldRun) {
      if(self.dataSeriesType === 'GRID')
        self.alert.legend_attribute = self.legend_attribute_grid;
      else
        self.alert.legend_attribute = self.legend_attribute_mo;

      // Broadcasting each one terrama2 field directive validation
      $scope.$broadcast("formFieldValidation");
      // Broadcasting schema form validation
      $scope.$broadcast("schemaFormValidate");

      /**
       * Defines a common message for invalid fields
       * @type {string}
       */
      var errMessageInvalidFields = i18n.__("There are invalid fields on form");

      if(self.isNotValid) {
        self.MessageBoxService.danger(i18n.__("Alerts"), errMessageInvalidFields);
        return;
      }

      $timeout(function() {
        if($scope.forms.alertForm.$invalid || $scope.forms.dataSeriesForm.$invalid || $scope.forms.legendLevel.$invalid || $scope.forms.reportForm.$invalid || $scope.forms.notificationForm.$invalid) {
          self.MessageBoxService.danger(i18n.__("Alerts"), errMessageInvalidFields);
          return;
        }

        var legendTemp = $.extend(true, {}, self.legendModel);
        var level = 1;

        for(var i = 0, levelsLength = legendTemp.levels.length; i < levelsLength; i++) {
          if(self.notify_on_legend_level === legendTemp.levels[i]._id)
            self.alert.notifications[0].notify_on_legend_level = level;

          delete legendTemp.levels[i]._id;
          legendTemp.levels[i].level = level;
          level++;
        }

        if(legendTemp.id === 0)
          delete legendTemp.id;

        self.alert.legend = legendTemp;

        if(!self.includeReport && self.alert.notifications[0].include_report !== undefined)
          self.alert.notifications[0].include_report = null;

        if(!self.notifyOnLegendLevel && self.alert.notifications[0].notify_on_legend_level !== undefined)
          self.alert.notifications[0].notify_on_legend_level = null;

        var operation = self.isUpdating ? self.AlertService.update(self.alert.id, self.alert) : self.AlertService.create(self.alert);
        operation.then(function(response) {
          $log.info(response);
          $window.location.href = BASE_URL + "configuration/alerts?token=" + response.token;
        }).catch(function(err) {
          $log.info(err);
          self.MessageBoxService.danger(i18n.__("Alert"), i18n.__(err));
        });
      });
    };
  };

  AlertRegisterUpdate.$inject = ["$scope", "$q", "$window", "$log", "$http", "$timeout", "i18n", "MessageBoxService", "AlertService", "DataSeriesService", "DataProviderService", "AnalysisService", "Service", "UniqueNumber"];

  return AlertRegisterUpdate;
});
