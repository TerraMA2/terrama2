(function() {
  'use strict';

  angular.module('terrama2.analysis.controller.registerupdate', [
    'terrama2',
    'terrama2.services',
    'terrama2.analysis.services',
    'terrama2.dataproviders.services',
    'terrama2.administration.services.iservices',
    'terrama2.dataseries.services',
    'terrama2.components.messagebox',
    'terrama2.components.messagebox.services',
    'terrama2.datetimepicker',
    'terrama2.ace',
    'terrama2.components.geo',
    'schemaForm',
    'terrama2.schedule',
    'treeControl'
  ])

  .controller('RegisterUpdateController', RegisterUpdateController);
  
  function RegisterUpdateController($scope, $q, $log, i18n, Service, DataSeriesService,
                                    DataSeriesSemanticsService, AnalysisService, DataProviderService, 
                                    Socket, DateParser, MessageBoxService, Polygon, $http) {
    var self = this;
    $scope.i18n = i18n;

    var config = configuration;
    /**
     * It defines a style for TerraMA² box directive
     * @type {Object}
     */
    self.css = {
      boxType: "box-solid"
    };

    /**
     * It defines all TerraMA² forms. Due schema form usage, it must be defined in $scope.
     * @type {Object}
     */
    $scope.forms = {};

    /**
     * It defines a options to use angular tree control directive. It is customized with bootstrap layout
     * 
     * @type {Object}
     */
    self.treeOptions = {
      nodeChildren: "children",
      multiSelection: true,
      dirSelectable: false,
      injectClasses: {
        ul: "list-group",
        li: "list-group-item",
        liSelected: "active",
        iExpanded: "without-border",
        iCollapsed: "without-border",
        iLeaf: "as",
        label: "a6",
        labelSelected: "2"
      }
    };

    /**
     * It defines TerraMA² Schedule values. Used to build terrama2-schedule directive
     * @type {Schedule}
     */
    self.schedule = {};

    /**
     * It defines Options to build terrama2-schedule directive
     * 
     * @param {Object}
     */
    self.scheduleOptions = {};

    /**
     * It defines a TerraMA² MessageBox Service for handling alert box
     * 
     * @type {MessageBoxService}
     */
    self.MessageBoxService = MessageBoxService;

    /**
     * TerraMA² Analysis Object to be created
     * 
     * @type {Object}
     */
    self.analysis = {
      metadata: {}
    };

    /**
     * It initializes Analysis active checkbox. It compares from object received from NodeJS
     */
    self.initActive = function() {
      self.analysis.active = (config.analysis.active === false || config.analysis.active) ?
          config.analysis.active : true;
    };

    /**
     * Helper to reset alert box instance
     */
    self.close = function() {
      self.MessageBoxService.reset();
    };

    // initializing async services
    $q
      .all([
        i18n.ensureLocaleIsLoaded(),
        Service.init({type: 'ANALYSIS'}),
        DataSeriesService.init({schema: "all"}),
        DataSeriesSemanticsService.init({metadata: true}),
        DataProviderService.init()
      ])
      // when done
      .then(function() {
        var socket = Socket;

        /**
         * It defines if UPDATE mode or SAVE mode
         */
        self.isUpdating = Object.keys(config.analysis).length > 0;

        /**
         * It defines a cached service instances
         * @type {ServiceInstance[]}
         */
        self.instances = Service.list({service_type_id: Service.types.ANALYSIS});

        /**
         * It defines a TerraMA² Service Instance DAO. Used to retrieve analysis services
         * @type {Service}
         */
        self.Service = Service;

        /**
         * It defines a TerraMA² Data Series DAO.
         * @type {DataSeriesService}
         */
        self.DataSeriesService = DataSeriesService;

        /**
         * It defines a TerraMA² analysis DAO
         * @type {AnalysisService}
         */
        self.AnalysisService = AnalysisService;

        /** 
         * flag to handling script status
         * 
         * @type {boolean}
         */ 
        self.testingScript = false;

        /**
         * It defines a TerraMA² box name when selecting a Analysis Type
         * 
         * @type {string}
         */
        self.dataSeriesBoxName =  i18n.__('Additional Data');

        /**
         * It defines Selected Semantics (Format Store)
         * Used to filter data providers
         * 
         * @type {DataSeriesSemantics}
         */
        self.currentSemantics = {};

        /**
         * It defines a list of filtered data providers from Format Store
         * @type {DataProvider[]}
         */
        self.dataProviders = [];

        /**
         * It defines enums used to handling DCP and Grid
         * @todo Angular EnumService
         */
        self.interpolationMethods = globals.enums.InterpolationMethod;
        self.interestAreaTypes = globals.enums.InterestAreaType;
        self.resolutionTypes = globals.enums.ResolutionType;

        /**
         * It defines a target data series that should be monitored/grid or even DCP
         * 
         * @type {DataSeries}
         */
        self.targetDataSeries = {};

        /**
         * It used to add identifier in Analysis Data Series result
         * 
         * @type {string}
         */
        self.identifier = "";

        /**
         * Socket listener for script validation. It just display the script state in result div.
         * 
         * @param {Object} result - A object received
         * @param {boolean} result.hasError - Flag to determine if if there error in request
         * @param {boolean} result.hasPythonError - Flag to determine if script is invalid
         * @param {string} result.messages - A lint result of pycheck
         * @param {string} result.systemError - Error message
         */
        socket.on('checkPythonScriptResponse', function(result) {
          self.testingScript = false;

          var errorBlock = angular.element('#systemError');
          var statusBlock = angular.element('#scriptCheckResult');

          if (result.hasError || result.hasPythonError) {
            if (result.hasError) {
              errorBlock.text(result.systemError);
              errorBlock.css('display', '');
            } else {
              errorBlock.css('display', 'none');
            }
          } else {
            errorBlock.css('display', 'none');
          }

          statusBlock.text(result.messages);
        });

        /**
         * It fills out GUI interface with analysis object. Used when updating analysis
         * 
         * @returns {void}
         */
        function fillGUI() {
          var analysisInstance = config.analysis;

          self.analysis.name = analysisInstance.name;
          self.analysis.description = analysisInstance.description;
          self.analysis.type_id = analysisInstance.type.id.toString();
          self.analysis.instance_id = analysisInstance.service_instance_id.toString();
          self.analysis.script = analysisInstance.script;

          // auto-trigger analysis type id changed
          self.onAnalysisTypeChanged();
          // auto-trigger format changed
          // self.onStoragerFormatChange();

          /**
           * It defines a reprocessing historical data values
           * @type {Object}
           */
          var historicalData = analysisInstance.reprocessing_historical_data || {};
          if (historicalData.startDate) {
            historicalData.startDate = DateParser(historicalData.startDate);
          }

          if (historicalData.endDate) {
            historicalData.endDate = DateParser(historicalData.endDate);
          }
          // setting default historical data
          self.analysis.historical = historicalData;

          // schedule update
          $scope.$broadcast("updateSchedule", analysisInstance.schedule);

          // Filtering Analysis DataSeries table if there is.
          analysisInstance.analysis_dataseries_list.forEach(function(analysisDs) {
            var ds = analysisDs.dataSeries;

            if (analysisDs.type === globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE) {
              self.selectedDataSeriesList.push(ds);
            } else {
              self.filteredDataSeries.some(function(filteredDs) {
                if (filteredDs.id === ds.id) {
                  self.targetDataSeries = filteredDs;
                  self.onTargetDataSeriesChange();

                  /**
                   * Defines target analysis data series identifier
                   * @type {string}   
                   */
                  self.identifier = analysisDs.metadata.identifier;
                  return true;
                }
              });
            }

            self.metadata[ds.name] = Object.assign({id: analysisDs.id, alias: analysisDs.alias}, analysisDs.metadata);
          });

          if (analysisInstance.type.id === globals.enums.AnalysisType.GRID) {
            // fill interpolation
            self.analysis.grid = {
              interpolation_method: analysisInstance.output_grid.interpolation_method,
              area_of_interest_type: analysisInstance.output_grid.area_of_interest_type,
              resolution_type: analysisInstance.output_grid.resolution_type
            };
            /**
             * It retrieves a dummy values (GRID Analysis) if there is.
             * @type {string}
             */
            var dummy = analysisInstance.output_grid.interpolation_dummy;
            if (dummy) {
              self.analysis.grid.interpolation_dummy = Number(dummy);
            }
            var resolutionDS = analysisInstance.output_grid.resolution_data_series_id;
            if (resolutionDS) {
              self.analysis.grid.resolution_data_series_id = resolutionDS;
            }
            var interestDS = analysisInstance.output_grid.area_of_interest_data_series_id;
            if (interestDS) {
              self.analysis.grid.area_of_interest_data_series_id = interestDS;
            }
            var resX = analysisInstance.output_grid.resolution_x;
            var resY = analysisInstance.output_grid.resolution_y;
            if (resX && resY) {
              self.analysis.grid.resolution_x = Number(resX);
              self.analysis.grid.resolution_y = Number(resY);
            }
            var coordinates = (analysisInstance.output_grid.area_of_interest_box || {}).coordinates;
            if (coordinates) {
              self.analysis.grid.area_of_interest_bounded = {
                minX: coordinates[0][0][0],
                minY: coordinates[0][0][1],
                maxX: coordinates[0][2][0],
                maxY: coordinates[0][2][1]
              };
            }
          } else { // if  monitored object or dcp
            self.analysis.metadata.INFLUENCE_TYPE = analysisInstance.metadata.INFLUENCE_TYPE;
            self.analysis.metadata.INFLUENCE_RADIUS = Number(analysisInstance.metadata.INFLUENCE_RADIUS);
            self.analysis.metadata.INFLUENCE_RADIUS_UNIT = analysisInstance.metadata.INFLUENCE_RADIUS_UNIT;
            self.analysis.metadata.INFLUENCE_DATASERIES_ID = analysisInstance.metadata.INFLUENCE_DATASERIES_ID;
            self.analysis.metadata.INFLUENCE_ATTRIBUTE = analysisInstance.metadata.INFLUENCE_ATTRIBUTE;
          }

          // setting storager format
          self.storagerFormats.some(function(storagerFmt) {
            if (analysisInstance.dataSeries.data_series_semantics.id === storagerFmt.id) {
              self.storager.format = storagerFmt;
              self.onStoragerFormatChange();
              return true;
            }
          });

          self.analysis.data_provider_id = analysisInstance.dataSeries.data_provider_id;
        }

        /**
         * Called when click to validate script. It prepares a script, wrapping it with dummy function (def) in order to validate correctly.
         * 
         * @emit #checkPythonScriptRequest
         * @returns {void}
         */
        self.onScriptValidation = function() {
          self.testingScript = true;
          var split = self.analysis.script.split("\n");
          var wrapScript = "def dummy():\n";
          split.forEach(function(element) {
            if (element) {
              wrapScript += "    " + element + "\n";
            }
          })
          socket.emit('checkPythonScriptRequest', {script: wrapScript || ""});
        };

        // define dataseries selected in modal
        self.nodesDataSeries = [];

        /**
         * Define dataseries selected to analysis
         * 
         * @type {Object[]}
         */ 
        self.selectedDataSeriesList = [];
        self.metadata = {};

        self.storagerFormats = [];
        self.storager = {};
        /**
         * It defines a local cache buffers with data series to help #dataSeriesGroups in Additional Groups (Static/Dynamic).
         * 
         * @type {Object}
         */
        self.buffers = {
          "static": [],
          "dynamic": []
        };

        // filter for dataseries basead analysis type. If obj monitored, then this list will be list of obj monitored and occurrences
        self.filteredDataSeries = [];

        /** 
         * Helper of selected semantics to display in gui "terrama2-box": Object Monitored, Dcp, Grid, etc.
         * @type {string}
         */
        self.semanticsSelected = "";

        /**
         * It defines a groups of additional data series
         * 
         * @type {Object[]}
         */
        self.dataSeriesGroups = [
          {name: "Static", children: []},
          {name: "Dynamic", children: []}
        ];

        /**
         * It will re-map available data series in buffers (additional data) depending analysis type.
         * 
         * For GRID data series, only dynamic data series is displayed (TODO: check it, since GRID static should exists soon in TerraMA²)
         * For Monitored Object, both dynamic as static data series may be selected
         */
        self._processBuffers = function() {
          // clean old data
          self.buffers = {
            "dynamic": [],
            "static": []
          };

          // cleaning already selected data series
          self.selectedDataSeriesList = [];

          if (parseInt(self.analysis.type_id) === AnalysisService.types.GRID) {
            DataSeriesService.list().forEach(function(dSeries) {
              var semantics = dSeries.data_series_semantics;

              if (semantics.temporality == globals.enums.TemporalityType.DYNAMIC){
                dSeries.isDynamic = true;
                self.buffers.dynamic.push(dSeries);
              } else {
                dSeries.isDynamic = false;
                self.buffers.static.push(dSeries);
              }
            });
          } else {
            DataSeriesService.list().forEach(function(dSeries) {
              var semantics = dSeries.data_series_semantics;

              if (semantics.temporality === globals.enums.TemporalityType.STATIC) {
                // skip target data series in additional data
                if (self.targetDataSeries && self.targetDataSeries.id !== dSeries.id) {
                  dSeries.isDynamic = false;
                  self.buffers.static.push(dSeries);
                }
              } else {
                dSeries.isDynamic = true;
                self.buffers.dynamic.push(dSeries);
              }
            });
          }

          self.dataSeriesGroups[0].children = self.buffers.static;
          self.dataSeriesGroups[1].children = self.buffers.dynamic;
        };

        /**
         * It checks if there is any data series dcp in Analysis Data Series list. If found, return true.
         * It set on template. Angular call it whenever scope cycle iteration done ($digest)
         * 
         * @returns {boolean}
         */
        self.hasDcp = function() {
          return self.selectedDataSeriesList
            .find(function(element) {
              return element.data_series_semantics.data_series_type_name === globals.enums.DataSeriesType.DCP;
            });
        };

        /**
         * It handles when an analysis type has been changed. It will redraw and re-populate storager formats depending analysis type.
         * It also will display correctly all fields required for analysis.
         */
        self.onAnalysisTypeChanged = function() {
          self.analysis.metadata = {};
          var semanticsType;
          var intTypeId = parseInt(self.analysis.type_id);
          var dataseriesFilterType;

          if (self.analysis.grid) {
            delete self.analysis.grid;
          }

          self.dataSeriesBoxName = i18n.__("Additional Data");
          switch(intTypeId) {
            case AnalysisService.types.DCP:
              semanticsType = DataSeriesService.DataSeriesType.DCP;
              self.semanticsSelected = "Dcp";
              dataseriesFilterType = 'DCP';
              break;
            case AnalysisService.types.GRID:
              semanticsType = DataSeriesService.DataSeriesType.GRID;
              self.semanticsSelected = "Grid";
              self.dataSeriesBoxName = i18n.__("Grid Data Series");
              dataseriesFilterType = 'GRID';
              break;
            case AnalysisService.types.MONITORED:
              semanticsType = DataSeriesService.DataSeriesType.ANALYSIS_MONITORED_OBJECT;
              self.semanticsSelected = i18n.__("Object Monitored");
              dataseriesFilterType = 'GEOMETRIC_OBJECT';
              break;
            default:
              $log.log("Invalid analysis type ID");
              return;
          }

          // re-fill data series
          self._processBuffers();

          self.onTargetDataSeriesChange = function() {
            if (self.targetDataSeries && self.targetDataSeries.name) {
              self.metadata[self.targetDataSeries.name] = {
                alias: self.targetDataSeries.name
              };
              var dataProvider = DataProviderService.list().filter(function(dProvider){
                return dProvider.id == self.targetDataSeries.data_provider_id;
              });
              if (dataProvider.length > 0 && dataProvider[0].data_provider_type.id == 4){
                var table_name = self.targetDataSeries.dataSets[0].format.table_name;
                listColumns(dataProvider[0], table_name);
              }
            }
          };

          var listColumns = function(dataProvider, table_name){
            var result = $q.defer();

            var params = getPostgisUriInfo(dataProvider.uri);
            params.objectToGet = "column";
            params.table_name = table_name;

            var httpRequest = $http({
              method: "GET",
              url: "/uri/",
              params: params
            });

            httpRequest.success(function(data) {
              self.columnsList = data.data.map(function(item, index){
                return item.column_name;
              });
              result.resolve(data);
            });

            httpRequest.error(function(err) {
              result.reject(err);
            });

            return result.promise;

          }
          
          //help function to parse a URI
          var getPostgisUriInfo = function(uri){
            var params = {};
            params.protocol = uri.split(':')[0];
            var hostData = uri.split('@')[1];
            if (hostData){
              params.hostname = hostData.split(':')[0];
              params.port = hostData.split(':')[1].split('/')[0];
              params.database = hostData.split('/')[1];  
            }  

            var auth = uri.split('@')[0];
            if (auth){
              var userData = auth.split('://')[1];
              if (userData){
                params.user = userData.split(':')[0];
                params.password = userData.split(':')[1];
              }
            }
            
            return params;
          }

          // filtering formats
          self.storagerFormats = [];
          DataSeriesSemanticsService.list().forEach(function(dSemantics) {
            if(dSemantics.data_series_type_name === semanticsType && !dSemantics.collector && dSemantics.temporality === globals.enums.TemporalityType.DYNAMIC) {
              self.storagerFormats.push(Object.assign({}, dSemantics));
            }
          });

          // filtering dataseries
          self.filteredDataSeries = DataSeriesService.list({
            data_series_semantics: {
              data_series_type_name: dataseriesFilterType
            }
          });
        };

        /**
         * Triggered to handling when a store format changed. It redraw GUI fields and reset model storager values
         */
        self.onStoragerFormatChange = function() {
          self.showStoragerForm = true;
          // semantics
          self.currentSemantics = self.storager.format;

          if (!self.currentSemantics) {
            self.formStorager = [];
            self.schemaStorager = {};
            self.modelStorager = {};
            self.showStoragerForm = false;
          } else {
            self.showStoragerForm = true;
            var metadata = self.currentSemantics.metadata;
            var properties = metadata.schema.properties;

            if (self.isUpdating) {
              self.modelStorager = config.analysis.dataSeries.dataSets[0].format;
            } else { self.modelStorager = {}; }

            self.formStorager = metadata.form;
            self.schemaStorager = {
              type: 'object',
              properties: metadata.schema.properties,
              required: metadata.schema.required
            };

            self.dataProviders = [];

            DataProviderService.list().forEach(function(dataProvider) {
              self.currentSemantics.metadata.demand.forEach(function(demand) {
                if (dataProvider.data_provider_type.name == demand) {
                  self.dataProviders.push(dataProvider);
                }
              });
            });
          }

          $scope.$broadcast('schemaFormRedraw');
        };

        self.formStorager = [];
        self.modelStorager = {};
        self.schemaStorager = {};
        self.options = {};

        var semanticsList = DataSeriesSemanticsService.list();
        var semanticsListFiltered = [];
        semanticsList.forEach(function(element) {
          // Skipping semantics for only collector
          if (!element.collector) {
            semanticsListFiltered.push(element);
          }
        })

        self.dataSeriesSemantics = semanticsListFiltered;

        var dataProviders = DataProviderService.list();

        // fill analysis in gui
        if (self.isUpdating) {
          fillGUI();
        } else {
          // auto-trigger onChangeAnalysis (default)
          self.onAnalysisTypeChanged();
        }

        $scope.$watch("ctrl.targetDataSeries", function(newValue) {
          if (!newValue) {
            return;
          }
          var hasNewValue = Object.keys(newValue).length !== 1;
          if (!hasNewValue) {
            return;
          }

          self.buffers.static = [];
          DataSeriesService.list().forEach(function(dataSeries) {
            if (dataSeries.data_series_semantics.temporality === "STATIC") {
              if (dataSeries.id !== newValue.id) {
                $log.log(dataSeries);
                self.buffers.static.push(dataSeries);
              }
            }
          });
          self.dataSeriesGroups[0].children = self.buffers.static;
        });

        $scope.$watch("ctrl.analysis.grid.area_of_interest_type", function(value) {
          if (value) {
            if (value === self.interestAreaTypes.CUSTOM.value) {
              var analysisConfig = (config.analysis || {});
              var value = (analysisConfig.output_grid || {}).srid || 4326;
              if (self.analysis.grid.area_of_interest_bounded) {
                self.analysis.grid.area_of_interest_bounded.srid = value;
              } else {
                self.analysis.grid.area_of_interest_bounded = {
                  srid: value
                };
              }
            }
          }
        });

        /**
         * It handles Area Of Interest for Analysis GRID type, resetting values to initial state
         */
        self.onAreaOfInterestChange = function() {
          // resetting area of interest values
          self.analysis.grid.area_of_interest_bounded = {};
          self.analysis.grid.area_of_interest_data_series_id = null;
        };

        /**
         * It is triggered when a resolution combobox has been changed. It aims to clear the resolution values from scope model.
         */
        self.onResolutionChanged = function() {
          self.analysis.grid.resolution_data_series_id = null;
          self.analysis.grid.resolution_x = null;
          self.analysis.grid.resolution_y = null;
        };

        // handling functions
        // checking for empty data series table
        self.isEmptyDataSeries = function() {
          return self.selectedDataSeriesList.length === 0;
        };

        // it adds dataseries from modal to table
        self.addDataSeries = function() {
          var _helper = function(type, target) {
            self.buffers[type].some(function(element, index, arr) {
              if (element.id == target.id) {
                arr.splice(index, 1);
                return true;
              }
              return false;
            });
          };

          self.nodesDataSeries.forEach(function(target) {
            if (!target || !target.id)
              return;

            self.metadata[target.name] = {alias: target.name};
            self.selectedDataSeriesList.push(target);

            if (target.isDynamic) {
              _helper("dynamic", target);
            } else {
              _helper("static", target);
            }
          });

          self.nodesDataSeries = [];
        };

        self.removeDataSeries = function(dataSeries) {

          var _pushToBuffer = function(type, obj) {
            self.buffers[type].push(obj);
          };

          self.selectedDataSeriesList.some(function(dSeries, index, arr) {
            if (dSeries.id == dataSeries.id) {
              arr.splice(index, 1);
              var type = dSeries.isDynamic ? "dynamic" : "static";
              _pushToBuffer(type, dSeries);
              return true;
            }
          });
        };

        // it check if there is a dataseries selected
        self.isAnyDataSeriesSelected = function() {
          return self.selectedDataSeries && self.selectedDataSeries.id > 0;
        };

        // it handles hidden box with data-series analysis metadata
        self.onDataSeriesClick = function(dataSeries) {
          if (self.selectedDataSeries) {
            if (dataSeries.id == self.selectedDataSeries.id) {
              self.selectedDataSeries = {};
              return;
            }
          }

          self.metadata[dataSeries.name] = self.metadata[dataSeries.name] || {};
          self.selectedDataSeries = dataSeries;
        };

        /*
         * DCP metadata (radius format - km, m...)
         */ 
        self.onMetadataFormatClick = function(format) {
          self.analysis.metadata.INFLUENCE_RADIUS_UNIT = format;
        };

        /**
         * It performs a save operation, applying all validation processes and then calling angular service AnalysisService
         * 
         * @param {boolean?} shouldRun - Defines if should save and run the analysis after save process. Default: false
         */
        self.save = function(shouldRun) {
          // Emitting broadcast to colorize fields based on valid state
          $scope.$broadcast('formFieldValidation');

          self.analysis_script_error = false;
          // TODO: emit a signal to validate form like $scope.$broadcast('scheduleFormValidate')
          var scheduleForm = angular.element('form[name="scheduleForm"]').scope().scheduleForm;
          if ($scope.forms.generalDataForm.$invalid ||
              $scope.forms.storagerDataForm.$invalid ||
              $scope.forms.storagerForm.$invalid || 
              scheduleForm.$invalid ||
              $scope.forms.targetDataSeriesForm.$invalid ||
              $scope.forms.scriptForm.$invalid) {
            MessageBoxService.danger(i18n.__("Analysis"), i18n.__("There are invalid fields on form"));
            return;
          }

          // checking script form if there any "add_value"
          var typeId = parseInt(self.analysis.type_id);
          /**
           * It retrieves a dom element #scriptCheckResult in order to append script feedback message
           * @type {DOM}
           */
          var checkResult = angular.element("#scriptCheckResult");

          var hasScriptError = function(expression, message) {
            var output = false;
            if (self.analysis.script.indexOf(expression) < 0) {
              self.analysis_script_error = true;
              self.analysis_script_error_message = i18n.__("Analysis will not able to generate a output data. ") + message;
              output = true;
            } else {
              self.analysis_script_error_message = "";
              self.analysis_script_error = false;
            }
            checkResult.html(self.analysis_script_error_message);
            return output;
          };

          var expression, message;

          if (typeId === AnalysisService.types.GRID) {
            expression = "return";
            message = "Grid analysis script must end with 'return' statement";
          } else {
            expression = "add_value";
            message = "Please fill at least a add_value() in script field.";
          }
          if (hasScriptError(expression, i18n.__(message))) {
            return;
          }

          // checking dataseries analysis
          var dataSeriesError = {};
          var hasError = self.selectedDataSeriesList.some(function(dSeries) {
            if (!self.metadata[dSeries.name]) {
              dataSeriesError = dSeries;
              return true;
            }
          });

          if (hasError) {
            MessageBoxService.danger(i18n.__("Analysis"), 
                                     i18n.__("Invalid data series. Please fill out alias in ") + dataSeriesError.name);
            return;
          }

          // cheking influence form: DCP and influence form valid
          if (self.analysis.type_id == 1) {
            var form = $scope.forms.influenceForm;
            if (form.$invalid) {
              return;
            }
          } else if (self.analysis.type_id == globals.enums.AnalysisType.GRID) {
            if ($scope.forms.gridForm.$invalid) {
              return;
            }
          }

          var analysisDataSeriesArray = [];

          var _makeAnalysisDataSeries = function(selectedDS, type_id) {
            var metadata = Object.assign({}, self.metadata[selectedDS.name] || {});
            var alias = (self.metadata[selectedDS.name] || {}).alias;

            delete metadata.alias;
            var _id = 0;
            if (metadata.id) {
              _id = metadata.id;
              delete metadata.id;
            }

            return {
              id: _id,
              data_series_id: selectedDS.id,
              metadata: metadata,
              alias: alias,
              // todo: check it
              type_id: type_id
            };
          };

          // target data series
          var analysisTypeId;
          switch(typeId) {
            case globals.enums.AnalysisType.DCP:
              analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_DCP_TYPE;
              break;
            case globals.enums.AnalysisType.GRID:
              analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_GRID_TYPE;
              break;
            case globals.enums.AnalysisType.MONITORED:
              analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_MONITORED_OBJECT_TYPE;
              self.metadata[self.targetDataSeries.name]['identifier'] = self.identifier;
              // setting monitored object id in output data series format
              self.modelStorager.monitored_object_id = self.targetDataSeries.id;
              self.modelStorager.monitored_object_pk = self.identifier;
              break;
          }

          // preparing data to send
          var analysisToSend = Object.assign({}, self.analysis);

          // setting target data series metadata (monitored object, dcp..)
          if (typeId !== globals.enums.AnalysisType.GRID) {
            analysisDataSeriesArray.push(_makeAnalysisDataSeries(self.targetDataSeries, analysisTypeId));
          } else {
            // checking geojson
            if (self.analysis.grid && self.analysis.grid.area_of_interest_bounded &&
                !angular.equals({}, self.analysis.grid.area_of_interest_bounded)) {

              var boundedForm = (angular.element('form[name="boundedForm"]').scope() || {boundedForm: {}}).boundedForm;
              if (boundedForm.$invalid) {
                return;
              }

              var bounded = self.analysis.grid.area_of_interest_bounded;
              analysisToSend.grid.area_of_interest_box = Polygon.build(bounded);

              analysisToSend.grid.srid = bounded.srid;
            }
          }
          // preparing analysis data series
          self.selectedDataSeriesList.forEach(function(selectedDS) {
            // additional data
            var analysisDataSeries = _makeAnalysisDataSeries(selectedDS, globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE);
            analysisDataSeriesArray.push(analysisDataSeries);
          });

          analysisToSend.dataSeries = self.selectedDataSeriesList;
          analysisToSend.analysisDataSeries = analysisDataSeriesArray;

          var storager = {};
          storager.format = Object.assign({}, self.modelStorager);
          storager.semantics = Object.assign({}, self.storager);

          var scheduleValues = Object.assign({}, self.schedule);
          switch(scheduleValues.scheduleHandler) {
            case "seconds":
            case "minutes":
            case "hours":
              scheduleValues.frequency_unit = scheduleValues.scheduleHandler;
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

          // sending post operation
          var objectToSend = {
            analysis: analysisToSend,
            storager: storager,
            schedule: scheduleValues,
            run: shouldRun
          };
          /**
           * Target object request (update/insert)
           * @type {angular.IPromise<any>}
           */
          var request;

          if (self.isUpdating) { request = AnalysisService.update(config.analysis.id, objectToSend); }
          else { request = AnalysisService.create(objectToSend); }

          return request
            .then(function(data) {
              window.location = "/configuration/analysis?token=" + (data.token || (data.data || {}).token);
            })
            .catch(function(err) {
              $log.log(err);
              MessageBoxService.danger(i18n.__("Analysis"), err.message);
            });
        };
      })

      .catch(function(err) {
        $log.log("Could not load analysis interface due " + err.toString() + "\nPlease refresh this page (F5)");
      });
  }
  // Injecting angular dependencies in controller
  RegisterUpdateController.$inject = [
    '$scope',
    '$q',
    '$log',
    'i18n',
    'Service',
    'DataSeriesService',
    'DataSeriesSemanticsService',
    'AnalysisService',
    'DataProviderService',
    'Socket',
    'DateParser',
    'MessageBoxService',
    'Polygon',
    '$http'
  ];
} ());