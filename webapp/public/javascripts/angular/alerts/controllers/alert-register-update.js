define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle Alert form registration.
   * @class AlertRegistration
   */
  var AlertRegisterUpdate = function($scope, $q, $window, $log, $http, $timeout, i18n, MessageBoxService, AlertService, DataSeriesService, DataProviderService, AnalysisService, Service, UniqueNumber, Utility, Socket, ColorFactory) {
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
     * Flag that indicates if the PDF is disabled.
     *
     * @type {boolean}
     */
    self.disablePDF = config.disablePDF;

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
     * It keeps the rgba color values
     * 
     * @type {object}
     */
    self.rgba = {
      r: null,
      g: null,
      b: null,
      a: 1,
      index: null
    };
    
    /**
     * Regex to validade data series attribute
     */
    self.regexColumn = "^[a-zA-Z_][a-zA-Z0-9_]*$";

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
      automatic_schedule: {
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
     * @type {Object}
     */
    self.scheduleOptions = {
      showAutomaticOption: true
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

    self.showAutoCreateLegendButton = false;

    /**
     * Indicates if there are views attached
     * 
     * @type {boolean}
     */
    self.attachViews = (self.isUpdating ? !!config.alertAttachment : true);

    /**
     * Views available to be attached
     * 
     * @type {array}
     */
    self.viewsToAttach = [];

    /**
     * Attached views
     * 
     * @type {array}
     */
    self.attachedViews = self.isUpdating ? [] : [
      {
        _id: "alertView",
        view: null,
        viewName: null
      }
    ];

    /**
     * Attachment
     * 
     * @type {object}
     */
    self.alertAttachment = (self.isUpdating ? config.alertAttachment : {
      y_max: null,
      y_min: null,
      x_max: null,
      x_min: null,
      srid: null
    });

    /**
     * Current width of views selects
     * 
     * @type {integer}
     */
    self.selectWidth = null;

    /**
     * Current width of views td
     * 
     * @type {integer}
     */
    self.tdWidth = null;

    if (self.isUpdating && self.alert.view && self.alert.view.legend){
      self.colors = [];
      var legendColors = self.alert.view.legend.colors.sort(function(a, b){
        return a.id - b.id;
      })
      for (var i = 0; i < legendColors.length; i ++){
        self.colors.push(legendColors[i].color);
      }
      self.alert.hasView = true;
      self.view_private = self.alert.view.private;
      self.view_service_instance_id = self.alert.view.service_instance_id;
      self.alert.schedule = Object.assign({}, self.alert.view.schedule);
      self.alert.automatic_schedule = Object.assign({}, self.alert.view.automatic_schedule);
      self.alert.schedule_type = self.alert.view.schedule_type;

    } else {
      self.colors = [
        "#FFFFFFFF",
        "#FFFFFFFF"      
      ];
    }

    self.defaultColorOpts = {
      format: "hex8",
      required: true,
      alpha: true
    };
    /**
     * It defines a event listeners for color handling
     */
    self.events = {
      onChange: function (api, color, $event) {
        //console.log("fra");
      }
    };

    var hasProjectPermission = config.hasProjectPermission;
    
    if (self.isUpdating && !hasProjectPermission){
      MessageBoxService.danger(i18n.__("Permission"), i18n.__("You can not edit this alert. He belongs to a protected project!"));
    }
    
    // Flag to verify if can not save if the service is not running
    var canSave = true;
    var serviceOfflineMessage = "If service is not running you can not save the alert. Start the service before create or update an alert!";
    
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

        self.filteredViewServices = self.ServiceInstance.list({'service_type_id': self.ServiceInstance.types.VIEW});

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

        $timeout(function() {
          if (self.isUpdating) {
            self.schedule = {};
            self.alert.schedule.scheduleType = self.alert.schedule_type.toString();
            $scope.$broadcast("updateSchedule", self.alert.schedule || {});
          }
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
              if (legend.levels[j].value == null){
                legend.levels[j].isDefault = true;
              }
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
                  if (self.legendModel.levels[j].value == null){
                    self.legendModel.levels[j].isDefault = true;
                    break;
                  }
                }
                for(var j = 0, levelsLength = self.legendModel.levels.length; j < levelsLength; j++) {
                  if(self.legendModel.levels[j].level === self.alert.notifications[0].notify_on_legend_level) {
                    self.notify_on_legend_level = self.legendModel.levels[j]._id;
                    break;
                  }
                }

                $timeout(function() {
                  self.onLegendsChange();
                  self.getColumnValues();
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

            if(config.alertAttachedViews) {
              if(config.views)
                self.viewsToAttach = config.views;

              var dbAlertAttachedViews = config.alertAttachedViews;

              if(dbAlertAttachedViews.length === 0) {
                var viewId = (self.alert.view && self.alert.view.id ? self.alert.view.id.toString() : null);

                self.newAttachedView(viewId, null, null, "alertView");
              } else {
                for(var i = 0, alertAttachedViewsLength = dbAlertAttachedViews.length; i < alertAttachedViewsLength; i++) {
                  var interfaceId = (self.alert.view.id === dbAlertAttachedViews[i].View.id ? "alertView" : null);

                  self.newAttachedView(dbAlertAttachedViews[i].View.id.toString(), dbAlertAttachedViews[i].View.name, dbAlertAttachedViews[i].id, interfaceId);
                }
              }
            }
          } else {
            self.legendModel = self.legends[0];

            $timeout(function() {
              self.onLegendsChange();

              // Forcing first alert service pre-selected
              if(self.filteredServices.length > 0)
                self.alert.service_instance_id = self.filteredServices[0].id;
            });
          }

          if(self.alert.notifications[0].include_report === undefined || self.alert.notifications[0].include_report === null)
            self.alert.notifications[0].include_report = "PDF";
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
          if(dataSeries.data_series_semantics.temporality == "STATIC") {
            return BASE_URL + "images/static-data-series/vetorial/vetorial.png";
            break;
          } else {
            return BASE_URL + "images/dynamic-data-series/geometric-object/geometric-object.png";
            break;
          }
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
                name: "Default",
                isDefault: true
              },
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

    self.columnValues = [];
    /**
     * Lists the values of a column from a given table.
     * 
     * @returns {void}
     */
    self.getColumnValues = function(){
      if(self.columnsList.length == 0){
        self.columnValues = [];
        self.showAutoCreateLegendButton = false;
        return;
      } else {
        var hasInList = self.columnsList.some(function(column){
          return column == self.legend_attribute_mo;
        });
        if (!hasInList){
          self.columnValues = [];
          self.showAutoCreateLegendButton = false;
          return;
        }
      }
      if (!self.legend_attribute_mo){
        self.columnValues = [];
        self.showAutoCreateLegendButton = false;
        return;
      }
      var dataSeries = self.dataSeries.filter(function(dataSeriesToFilter) {
        return dataSeriesToFilter.id == self.alert.data_series_id;
      });
      if (!dataSeries[0]){
        self.columnValues = [];
        self.showAutoCreateLegendButton = false;
        return;
      }

      var tableName = dataSeries[0].dataSets[0].format.table_name;
      var dataProviderId = dataSeries[0].data_provider_id;
      
        DataProviderService.listPostgisObjects({providerId: dataProviderId, objectToGet: "values", tableName: tableName, columnName: self.legend_attribute_mo})
          .then(function(response){
            if (response.data.status == 400){
              self.columnValues = [];
              self.showAutoCreateLegendButton = false;
            } else {
              if (response.data.data)
                self.columnValues = response.data.data;
              else
                self.columnValues = [];

              if (self.columnValues.length > 0){
                var thereAreNaNInValues = self.columnValues.some(function(columnValue){
                  return isNaN(Number(columnValue));
                });
                if (thereAreNaNInValues)
                  self.showAutoCreateLegendButton = false;
                else
                  self.showAutoCreateLegendButton = true;
              } else {
                self.showAutoCreateLegendButton = false;
              }
            }
          });
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
        self.showAutoCreateLegendButton = false;

        if(self.isUpdating)
          self.legend_attribute_grid = parseInt(self.alert.legend_attribute);
      }

      if(!self.isUpdating) {
        delete self.legend_attribute_grid;
        delete self.legend_attribute_mo;
      }

      self.dataSeriesType = dataSeries.data_series_semantics.data_series_type_name;

    };

    /**
     * Auto create legends with possible values of attribute
     * 
     * @returns {void}
     */
    self.autoCreateLegend = function(){
      var defaultColors = ColorFactory.getDefaultColors();
      self.legendModel.levels = [
        {
          _id: UniqueNumber(),
          name: "Default",
          isDefault: true
        }
      ];
      for (var i = 0; i < self.columnValues.length; i++){
        self.legendModel.levels.push({
          _id: UniqueNumber(),
          name: "",
          value: Number(self.columnValues[i])
        });
      }
      self.colors = defaultColors.slice(0, self.legendModel.levels.length);
    }

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

      self.colors.push("#FFFFFFFF");
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
          self.colors.splice(j, 1);
          break;
        }
      }
    };

    /**
     * It opens the rgba modal.
     * 
     * @returns {void}
     */
    self.rgbaModal = function(index) {
      self.rgba.index = index;
      var rgbaColor = Utility.hex2rgba(self.colors[index]);
      if (rgbaColor){
        self.rgba.r = rgbaColor.r;
        self.rgba.g = rgbaColor.g;
        self.rgba.b = rgbaColor.b;
        self.rgba.a = rgbaColor.a;
      }
      $("#rgbaModal").modal();
    };

    /**
     * It fillls the hex color field, converting the rgba to hex8.
     * 
     * @returns {void}
     */
    self.rgba2hex = function() {
      self.colors[self.rgba.index] = Utility.rgba2hex(self.rgba.r, self.rgba.g, self.rgba.b, self.rgba.a);

      self.rgba.r = null;
      self.rgba.g = null;
      self.rgba.b = null;
      self.rgba.a = 1;
      self.rgba.index = null;
    };

    /**
     * Lists the columns from a given table.
     * 
     * @returns {void}
     */
    var listColumns = function(dataProvider, tableName) {
      var result = $q.defer();

      DataProviderService.listPostgisObjects({providerId: dataProvider.id, objectToGet: "column", tableName: tableName})
        .then(function(response){
          if (response.data.status == 400){
            return result.reject(response.data);
          }
          self.columnsList = response.data.data.map(function(item, index) {
            return item.column_name;
          });
          result.resolve(response.data.data);
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
        if(!self.legendModel.levels[i].isDefault){
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
        }

        if(self.legendModel.levels[i].name === undefined || self.legendModel.levels[i].name === "") {
          self.legendLevelNameError[self.legendModel.levels[i]._id] = true;
          self.isNotValid = true;
        } else {
          self.legendLevelNameError[self.legendModel.levels[i]._id] = false;
        }
      }
    }, true);

    $scope.$watch("ctrl.alert.service_instance_id", function(instanceId){
      if (instanceId)
        Socket.emit('status', {service: instanceId});
    }, true);

    Socket.on('statusResponse', function(response){
      if(response.service == self.alert.service_instance_id){
        if (response.checking === undefined || (!response.checking && response.status === 400)) {
          if (!response.online){
            self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__(serviceOfflineMessage));
            canSave = false;
          } else {
            canSave = true;
            self.MessageBoxService.reset();
          }
        }
      }
    });

    /**
     * Helper to reset alert box instance.
     * 
     * @returns {void}
     */
    self.close = function() {
      self.MessageBoxService.reset();
    };

    /**
     * Window resize event.
     * 
     * @returns {void}
     */
    $(window).resize(function() {
      if(self.attachedViews.length > 1) {
        for(var i = 0, attachedViewsLength = self.attachedViews.length; i < attachedViewsLength; i++) {
          if(self.attachedViews[i]._id !== "alertView") {
            self.selectWidth = $("#" + self.attachedViews[i]._id).width();
            break;
          }
        }
      } else {
        self.selectWidth = null;
      }

      self.tdWidth = $("#alertView").width();
    });

    /**
     * Service change event.
     * 
     * @returns {void}
     */
    self.onServiceChanged = function() {
      $http({
        method: "GET",
        url: BASE_URL + "api/ViewByService/" + self.view_service_instance_id + "/" + config.activeProject.id
      }).then(function(views) {
        self.viewsToAttach = views.data;
        self.attachedViews = [
          {
            _id: "alertView",
            view: null,
            viewName: null
          }
        ];
      });
    };

    /**
     * Creation of a new attached view.
     * 
     * @returns {void}
     */
    self.newAttachedView = function(view, viewName, id, interfaceId) {
      var newItem = {
        _id: (interfaceId ? interfaceId : UniqueNumber()),
        view: (view ? view : null),
        viewName: (viewName && !interfaceId ? viewName : null)
      };

      if(id)
        newItem.id = id;

      self.attachedViews.push(newItem);

      $timeout(function() {
        self.selectWidth = $("#" + newItem._id).width();
        self.tdWidth = $("#alertView").width();
      });
    };

    /**
     * Removal of an attached view.
     * 
     * @returns {void}
     */
    self.removeAttachedView = function(attachedViewId) {
      for(var i = 0, attachedViewsLength = self.attachedViews.length; i < attachedViewsLength; i++) {
        if(self.attachedViews[i]._id === attachedViewId) {
          self.attachedViews.splice(i, 1);
          break;
        }
      }
    };

    /**
     * Sort start event.
     * 
     * @returns {void}
     */
    self.startSort = function($item, $part, $index, $helper) {
      if(self.attachedViews[$index].viewName !== null) {
        $(".sv-helper select > option").text(self.attachedViews[$index].viewName);
        $(".sv-helper select").width(self.selectWidth);
      } else {
        $(".sv-helper #alertView").width(self.tdWidth);
      }
    }; 

    /**
     * Attached view selection event.
     * 
     * @returns {void}
     */
    self.selectAttachedView = function(item) {
      for(var i = 0, viewsToAttachLength = self.viewsToAttach.length; i < viewsToAttachLength; i++) {
        if(item.attachedView.view == self.viewsToAttach[i].id) {
          self.attachedViews[item.$index].viewName = self.viewsToAttach[i].name;
          break;
        }
      }
    };

    /**
     * Attach views change event.
     * 
     * @returns {void}
     */
    self.attachViewsChange = function() {
      if(self.attachViews) {
        self.alertAttachment = {
          y_max: null,
          y_min: null,
          x_max: null,
          x_min: null,
          srid: null
        };

        self.attachedViews = [
          {
            _id: "alertView",
            view: null,
            viewName: null
          }
        ];
      } else {
        self.alertAttachment = null;
        self.attachedViews = [];
      }
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

      if (self.isUpdating && !hasProjectPermission){
        return MessageBoxService.danger(i18n.__("Permission"), i18n.__("You can not edit this alert. He belongs to a protected project!"));
      }

      if (!canSave){
        self.MessageBoxService.danger(i18n.__("Alerts"), i18n.__(serviceOfflineMessage));
        return;
      }

      $timeout(function() {
        if($scope.forms.alertForm.$invalid || $scope.forms.dataSeriesForm.$invalid || $scope.forms.legendLevel.$invalid || $scope.forms.reportForm.$invalid || $scope.forms.notificationForm.$invalid || ($scope.forms.alertAttachmentForm && $scope.forms.alertAttachmentForm.$invalid)) {
          self.MessageBoxService.danger(i18n.__("Alerts"), errMessageInvalidFields);
          return;
        }

        if(self.alert.hasView && self.attachViews) {
          self.alertAttachment.alert_id = (self.isUpdating ? self.alert.id : null);

          var attachViewsError = false;
          var attachViewsFinal = [];

          for(var i = 0, attachedViewsLength = self.attachedViews.length; i < attachedViewsLength; i++) {
            if(self.attachedViews[i].view === null && self.attachedViews[i]._id !== "alertView") {
              attachViewsError = true;
              break;
            } else {
              var attachedViewFinal = {
                layer_order: i + 1,
                alert_attachment_id: (self.isUpdating ? self.alertAttachment.id : null),
                view_id: (self.attachedViews[i].view !== null ? self.attachedViews[i].view : null)
              };

              if(self.attachedViews[i].id)
                attachedViewFinal.id = self.attachedViews[i].id;

              attachViewsFinal.push(attachedViewFinal);
            }
          }

          if(attachViewsError)
            return self.MessageBoxService.danger(i18n.__("Alert"), i18n.__("Select a view in all the attached views"));
          else {
            self.alert.attachedViews = attachViewsFinal;
            self.alert.alertAttachment = self.alertAttachment;
          }
        }

        var legendTemp = $.extend(true, {}, self.legendModel);
        var level = 1;

        for(var i = 0, levelsLength = legendTemp.levels.length; i < levelsLength; i++) {
          if (legendTemp.levels[i].isDefault){
            continue;
          }
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

        if (self.alert.schedule && Object.keys(self.alert.schedule).length !== 0) {
          self.alert.schedule_type = self.alert.schedule.scheduleType;
          /**
           * @todo Implement Angular ScheduleService to handle it, since is common on dynamic data series and analysis registration.
           */
          var scheduleForm = angular.element('form[name="scheduleForm"]').scope()['scheduleForm'];
          // form validation
          if (scheduleForm.$invalid) {
            return;
          }

          // preparing schedule.
          var scheduleValues = self.alert.schedule;
          switch(scheduleValues.scheduleHandler) {
            case "seconds":
            case "minutes":
            case "hours":
              scheduleValues.frequency_unit = scheduleValues.scheduleHandler;
              scheduleValues.frequency_start_time = scheduleValues.frequency_start_time ? moment(scheduleValues.frequency_start_time).format("HH:mm:ssZ") : "";
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
              if (scheduleValues.scheduleType == "4"){
                scheduleValues.data_ids = [self.alert.data_series_id];
              }
              break;
          }
        }
        if (self.alert.hasView){
          var viewLegend = {
            colors: [],
            metadata: {
              column: self.alert.legend_attribute,
              creation_type: "editor"
            },
            type: 2
          };
          for (var i = 0; i < self.alert.legend.levels.length; i++){
            var colorModel = {
              color: self.colors[i],
              isDefault: i == 0,
              title: self.alert.legend.levels[i].name,
              value: i == 0 ? "" : self.alert.legend.levels[i].value
            }
            viewLegend.colors.push(colorModel);
          }
          var viewModel = {
            name: self.alert.name,
            description: "Generated by alert: " + self.alert.name,
            schedule_type: self.alert.schedule_type,
            schedule: Object.assign({}, self.alert.schedule),
            automatic_schedule: self.alert.automatic_schedule,
            active: true,
            data_series_id: self.alert.data_series_id,
            service_instance_id: self.view_service_instance_id,
            private: self.view_private ? true : false,
            legend: viewLegend,
            source_type: 4
          }
          self.alert.view = viewModel;
          self.alert.schedule_type = "3";
          self.alert.schedule = {scheduleType: "3"};
        } else {
          delete self.alert.view;
          delete self.alert.view_id;
        }
        self.alert.run = shouldRun;
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

  AlertRegisterUpdate.$inject = ["$scope", "$q", "$window", "$log", "$http", "$timeout", "i18n", "MessageBoxService", "AlertService", "DataSeriesService", "DataProviderService", "AnalysisService", "Service", "UniqueNumber", "Utility", "Socket", "ColorFactory"];

  return AlertRegisterUpdate;
});
