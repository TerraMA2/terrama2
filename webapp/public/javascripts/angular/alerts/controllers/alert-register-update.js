define([], function() {
  'use strict';

  /**
   * It represents a Controller to handle Alert form registration.
   * @class AlertRegistration
   */
  function AlertRegisterUpdate($scope, $q, $window, $log, $http, $timeout, i18n, MessageBoxService, AlertService, DataSeriesService, Service) {
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
     * @type {Object[]}
     */
    self.dataSeries = [];

    /**
     * It represents a terrama2 box styles
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
     * @type {Object}
     */
    self.alert = config.alert || {};

    self.risks = [
      {
        id: 0,
        name: "New Risk"
      },
      {
        id: 1,
        name: "Risco 1",
        description: "lala",
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
      {
        id: 2,
        name: "Risco 2",
        description: "lala",
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
      {
        id: 3,
        name: "Risco 3",
        description: "lala",
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
    ];

    self.risksAlert = [];

    for(var i = 0, risksLength = self.risks.length; i < risksLength; i++) {
      self.risksAlert.push(Object.assign({}, self.risks[i]));
    }

    self.riskModel = self.risksAlert[0];
    self.riskModelLala = {
      name: "",
      description: "",
      levels: [
        {
          name: "",
          level: 1,
          value: ""
        }
      ]
    };

    $q.all([
      i18n.ensureLocaleIsLoaded(),
      DataSeriesService.init({schema: "all"})
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
        self.dataSeries = DataSeriesService.list({
          data_series_semantics: {
            data_series_type_name: "ANALYSIS_MONITORED_OBJECT",
            data_format_name: "POSTGIS"
          }
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
      if(self.riskModel.id !== 0) {
        self.riskModelLala = self.riskModel;
      } else {
        self.riskModelLala = {
          name: "",
          description: "",
          levels: [
            {
              name: "",
              level: 1,
              value: 0
            }
          ]
        };
      }
    };

    self.newLevel = function() {
      if(self.riskModel.id === 0) {
        self.riskModelLala.levels.push({
          name: "",
          level: 1,
          value: 0
        });
      } else {
        for(var i = 0, risksLength = self.risksAlert.length; i < risksLength; i++) {
          if(self.risksAlert[i].id === self.riskModel.id) {
            self.risksAlert[i].levels.push({
              name: "",
              level: 1,
              value: 0
            });

            break;
          }
        }
      }
    };

    self.removeLevel = function(level) {
      for(var i = 0, risksLength = self.risksAlert.length; i < risksLength; i++) {
        if(self.risksAlert[i].id === self.riskModel.id) {
          for(var j = 0, levelsLength = self.risksAlert[i].levels.length; j < levelsLength; j++) {
            if(self.risksAlert[i].levels[j].level === level.level) {
              self.risksAlert[i].levels.splice(j, 1);

              break;
            }
          }

          break;
        }
      }
    };

    $scope.$watch("ctrl.riskModelLala.levels", function(newVal, oldVal) {
      if(self.riskModelLala.levels.length > 1) {
        var lastValue = self.riskModelLala.levels[0].value;
        var orderError = false;

        for(var i = 1, levelsLength = self.riskModelLala.levels.length; i < levelsLength; i++) {
          if(lastValue > self.riskModelLala.levels[i].value) {
            orderError = true;
            break;
          } else
            lastValue = self.riskModelLala.levels[i].value;
        }

        self.orderError = orderError;
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

  AlertRegisterUpdate.$inject = ["$scope", "$q", "$window", "$log", "$http", "$timeout", "i18n", "MessageBoxService", "AlertService", "DataSeriesService", "Service"];

  return AlertRegisterUpdate;
});
