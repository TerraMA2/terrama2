define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle Alert form registration.
   * @class AlertRegistration
   */
  function AlertRegisterUpdate($scope, $q, $window, $log, $http, $timeout, i18n, MessageBoxService, AlertService, DataSeriesService, DataProviderService, AnalysisService, Service, UniqueNumber) {
    /**
     * @type {AlertRegisterUpdate}
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
     * It handles Service Instance model
     *
     * @type {Object}
     */
    self.ServiceInstance = Service;

    /**
     * Flag to handle if is Updating or Registering
     *
     * @type {Boolean}
     */
    self.isUpdating = config.alert ? true : false;

    /**
     * It defines a list of cached data series
     * 
     * @type {Object[]}
     */
    self.dataSeries = [];

    /**
     * It represents a terrama2 box styles
     * 
     * @type {Object}
     */
    self.css = {
      boxType: "box-solid"
    };

    /**
     * It handles Alert Service Instance model
     *
     * @type {Object}
     */
    self.AlertService = AlertService;

    /**
     * It contains alert instance values
     * 
     * @type {Object}
     */
    self.alert = config.alert || {
      notifications: [
        {
          include_report: true,
          notify_on_change: true,
          simplified_report: false,
          notify_on_risk_level: 1
        }
      ]
    };

    /**
     * It contains the analysis table columns list
     * 
     * @type {array}
     */
    self.columnsList = [];

    self.dataSeriesType = null;

    /**
     * It contains the pre existent risks
     * 
     * @type {array}
     */
    self.risks = [
      {
        id: 0,
        name: "New Risk",
        description: "",
        levels: [
          {
            _id: UniqueNumber(),
            name: "",
            value: ""
          }
        ]
      },
      {
        id: 1,
        name: "Risco 1",
        description: "lala",
        levels: [
          {
            _id: UniqueNumber(),
            name: "level1",
            value: 100
          },
          {
            _id: UniqueNumber(),
            name: "level2",
            value: 200
          }
        ]
      },
      {
        id: 2,
        name: "Risco 2",
        description: "lala",
        levels: [
          {
            _id: UniqueNumber(),
            name: "level1",
            value: 100
          },
          {
            _id: UniqueNumber(),
            name: "level2",
            value: 200
          }
        ]
      },
      {
        id: 3,
        name: "Risco 3",
        description: "lala",
        levels: [
          {
            _id: UniqueNumber(),
            name: "level1",
            value: 100
          },
          {
            _id: UniqueNumber(),
            name: "level2",
            value: 200
          }
        ]
      },
    ];

    /**
     * It contains the selected risk model
     * 
     * @type {object}
     */
    self.riskModel = self.risks[0];

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
        // setting all alert services in cache
        self.filteredServices = self.ServiceInstance.list({'service_type_id': self.ServiceInstance.types.ALERT});

        /**
         * Retrieve data series
         */
        self.dataSeries = DataSeriesService.list().filter(function(dataSeriesToFilter) {
          return dataSeriesToFilter.data_series_semantics.data_series_type_name === "ANALYSIS_MONITORED_OBJECT" || dataSeriesToFilter.data_series_semantics.data_series_type_name === "GRID";
        });

        $timeout(function() {
          if(!self.isUpdating) {
            // forcing first alert service pre-selected
            if(self.filteredServices.length > 0)
              self.alert.service_instance_id = self.filteredServices[0].id;
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
          return "/images/dynamic-data-series/dcp/dcp.png";
          break;
        case DataSeriesService.DataSeriesType.OCCURRENCE:
          return "/images/dynamic-data-series/occurrence/occurrence.png";
          break;
        case DataSeriesService.DataSeriesType.GRID:
          if(dataSeries.data_series_semantics.temporality == "STATIC") {
            return "/images/static-data-series/grid/grid.png";
            break;
          } else {
            if(dataSeries.isAnalysis)
              return "/images/analysis/grid/grid_analysis.png";
            else
              return "/images/dynamic-data-series/grid/grid.png";

            break;
          }
        case DataSeriesService.DataSeriesType.ANALYSIS_MONITORED_OBJECT:
          return "/images/analysis/monitored-object/monitored-object_analysis.png";
          break;
        case DataSeriesService.DataSeriesType.POSTGIS:
        case DataSeriesService.DataSeriesType.GEOMETRIC_OBJECT:
          return "/images/static-data-series/vetorial/vetorial.png";
          break;
        default:
          return "/images/dynamic-data-series/occurrence/occurrence.png";
          break;
      }
    };

    /**
     * It handles Risk combobox change.
     * 
     * @returns {void}
     */
    self.onRisksChange = function() {
      for(var i = 0, risksLength = self.risks.length; i < risksLength; i++) {
        if(self.risks[i].id === self.riskModel.id) {
          if(self.riskModel.id === 0) {
            self.risks[i].name = "New Risk";
            self.risks[i].description = "";
            self.risks[i].levels = [
              {
                _id: UniqueNumber(),
                name: "",
                value: ""
              }
            ];
          }

          self.riskModel = $.extend(true, {}, self.risks[i]);
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
        if(dataSeries[0].data_series_semantics.data_series_type_name === "ANALYSIS_MONITORED_OBJECT") {
          var dataProvider = DataProviderService.list().filter(function(dataProvider) {
            return dataProvider.id == dataSeries[0].data_provider_id;
          });

          var analysis = AnalysisService.list().filter(function(analysisToFilter) {
            return analysisToFilter.output_dataset_id == dataSeries[0].dataSets[0].id;
          });

          if(dataProvider.length > 0 && analysis.length > 0) {
            listColumns(dataProvider[0], dataSeries[0].dataSets[0].format.table_name);
          }
        } else {
          self.columnsList = [];
        }

        self.dataSeriesType = dataSeries[0].data_series_semantics.data_series_type_name;
      }
    };

    /**
     * It creates a new level in the current risk.
     * 
     * @returns {void}
     */
    self.newLevel = function() {
      self.riskModel.levels.push({
        _id: UniqueNumber(),
        name: "",
        value: ""
      });
    };

    /**
     * It removes a given level from the current risk.
     * 
     * @returns {void}
     */
    self.removeLevel = function(level) {
      for(var j = 0, levelsLength = self.riskModel.levels.length; j < levelsLength; j++) {
        if(self.riskModel.levels[j]._id === level._id) {
          self.riskModel.levels.splice(j, 1);
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
        url: "/uri/",
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

    $scope.$watch("ctrl.riskModel.levels", function(newVal, oldVal) {
      if(self.riskModel.levels.length > 1) {
        var lastValue = self.riskModel.levels[0].value;
        var orderError = isNaN(lastValue) || lastValue === "";

        if(!orderError) {
          for(var i = 1, levelsLength = self.riskModel.levels.length; i < levelsLength; i++) {
            if(isNaN(self.riskModel.levels[i].value) || self.riskModel.levels[i].value === "" || parseFloat(lastValue) > parseFloat(self.riskModel.levels[i].value)) {
              orderError = true;
              break;
            } else
              lastValue = self.riskModel.levels[i].value;
          }
        }

        self.orderError = orderError;
      } else {
        self.orderError = isNaN(self.riskModel.levels[0].value) || self.riskModel.levels[0].value === "";
      }
    }, true);

    self.alert = {
      //active: true,
      //name: "Alerta teste",
      //description: "Sem description",
      project_id: 1,
      //data_series_id: 4,
      //service_instance_id: 4,
      //risk_attribute: "risco atributo",
      schedule: {
        data_ids: [1],
        scheduleType: 4
      },
      additionalData: [
        {
          dataseries_id: 1,
          data_set_id: 1,
          referrer_attribute: "attreferer",
          referred_attribute: "attrefered",
          data_attributes: "atributos"
        }
      ],
      reportMetadata: {
        title: "TituloReport",
        abstract: "Abstract",
        description: "Descricao",
        author: "Author",
        contact: "contato",
        copyrigth: "Copyright",
        timestamp_format: "formatoData",
        logo_path: "Caminho logo",
        document_format: "PDF"
      },
      risk: {
        name: "Risco",
        description: "",
        levels: [
          {
            name: "level1",
            level: 1,
            value: 100
          },
          {
            name: "level2",
            level: 2,
            value: 200
          }
        ]
      },
      notifications: [
        {
          include_report: true,
          notify_on_change: true,
          simplified_report: true,
          notify_on_risk_level: 1,
          recipients: "recipientes"
        }
      ]
    };
    self.isValid = true;
    self.save = saveOperation;

    function saveOperation(shouldRun) {
      $timeout(function(){
        var operation = self.AlertService.create(self.alert);
        operation.then(function(response){
          console.log(response);
        })
        .catch(function(err){
          console.log(err);
        });
      });
      
    }
  }

  AlertRegisterUpdate.$inject = ["$scope", "$q", "$window", "$log", "$http", "$timeout", "i18n", "MessageBoxService", "AlertService", "DataSeriesService", "DataProviderService", "AnalysisService", "Service", "UniqueNumber"];

  return AlertRegisterUpdate;
});
