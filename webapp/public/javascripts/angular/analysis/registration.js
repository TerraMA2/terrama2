"use strict";

angular.module('terrama2.analysis.registration', [
    'terrama2',
    'terrama2.services',
    'terrama2.administration.services.iservices',
    'terrama2.components.messagebox',
    'terrama2.datetimepicker',
    'terrama2.ace',
    'terrama2.components.geo',
    'schemaForm',
    'terrama2.schedule',
    'treeControl'
  ])

  .controller('AnalysisRegistration',
    [
      '$scope',
      'i18n',
      'Service',
      'DataSeriesFactory',
      'DataSeriesSemanticsFactory',
      'AnalysisFactory',
      'DataProviderFactory',
      'Socket',
      'DateParser',
  function($scope, i18n, Service, DataSeriesFactory, DataSeriesSemanticsFactory, AnalysisFactory, DataProviderFactory, Socket, DateParser) {
    var socket = Socket;
    var self = this;

    self.Service = Service;

    // injecting i18n module
    $scope.i18n = i18n;

    // TerraMA2 box
    $scope.css = {
      boxType: "box-solid"
    };
    $scope.forms = {};

    $scope.initActive = function(state) {
      $scope.analysis.active = (configuration.analysis.active === false || configuration.analysis.active) ?
          configuration.analysis.active : true;
    };

    // Retrieving all services instances
    Service
      .init()
      .then(function() {

      })
      .catch(function(err) {

      });

    $scope.dataSeriesBoxName = i18n.__('Additional Data');

    // flag to handling script status
    $scope.testingScript = false;

    $scope.interpolationMethods = globals.enums.InterpolationMethod;
    $scope.interestAreaTypes = globals.enums.InterestAreaType;
    $scope.resolutionTypes = globals.enums.ResolutionType;

    // checking if is update mode
    $scope.isUpdating = Object.keys(configuration.analysis).length > 0;

    // initializing objects
    $scope.analysis = {
      metadata: {}
    };
    $scope.targetDataSeries = {};

    $scope.instances = [];
    $scope.dataSeriesList = [];
    $scope.dataProvidersList = [];
    $scope.dataProviders = [];

    $scope.identifier = "";

    // schedule
    $scope.schedule = {};
    $scope.isFrequency = false;
    $scope.isSchedule = false;
    $scope.scheduleOptions = {};

    $scope.onScriptChanged = function(editor) {
      var session = editor[1];
      var document = editor[1].getSession().getDocument();

      $scope.analysis.script = editor[1].getSession().getDocument().getValue();
    };

    socket.on('checkPythonScriptResponse', function(result) {
      $scope.testingScript = false;

      var errorBlock = angular.element('#systemError');
      var statusBlock = angular.element('#scriptCheckResult');

      if(result.hasError || result.hasPythonError) {
        if(result.hasError) {
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

    $scope.onScriptValidation = function() {
      $scope.testingScript = true;
      var split = $scope.analysis.script.split("\n");
      var wrapScript = "def dummy():\n";
      split.forEach(function(element) {
        if (element) {
          wrapScript += "    " + element + "\n";
        }
      })
      console.log(wrapScript);
      socket.emit('checkPythonScriptRequest', {script: wrapScript || ""});
    };

    // define dataseries selected in modal
    $scope.nodesDataSeries = [];

    // define dataseries selected to analysis
    $scope.selectedDataSeriesList = [];
    $scope.metadata = {};
    $scope.semantics = {};
    $scope.storagerFormats = [];
    $scope.storager = {};
    $scope.buffers = {
      "static": [],
      "dynamic": []
    };

    // filter for dataseries basead analysis type. If obj monitored, then this list will be list of obj monitored and occurrences
    $scope.filteredDataSeries = [];

    // helper of semantics selected to display in gui: Object Monitored, Dcp, Grid, etc.
    $scope.semanticsSelected = "";

    // data series tree modal
    $scope.treeOptions = {
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

    $scope.dataSeriesGroups = [
      {name: "Static", children: []},
      {name: "Dynamic", children: []}
    ];

    // watchers
    // cleaning analysis metadata when analysis type has been changed.
    // fill storager formats
    $scope.$watch("analysis.type_id", function(value) {
      $scope.analysis.metadata = {};
      var semanticsType;
      var intTypeId = parseInt(value);

      if ($scope.analysis.grid) {
        delete $scope.analysis.grid;
      }

      $scope.dataSeriesBoxName = i18n.__("Additional Data");
      switch(intTypeId) {
        case globals.enums.AnalysisType.DCP:
          semanticsType = globals.enums.DataSeriesType.DCP;
          $scope.semanticsSelected = "Dcp";
          break;
        case globals.enums.AnalysisType.GRID:
          semanticsType = globals.enums.DataSeriesType.GRID;
          $scope.semanticsSelected = "Grid";
          $scope.dataSeriesBoxName = i18n.__("Grid Data Series");
          break;
        case globals.enums.AnalysisType.MONITORED:
          semanticsType = globals.enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT;
          $scope.semanticsSelected = i18n.__("Object Monitored");
          break;
        default:
          console.log("invalid analysis type");
          return;
      }

      // re-fill data series
      _processBuffers();

      $scope.onTargetDataSeriesChange = function() {
        if ($scope.targetDataSeries && $scope.targetDataSeries.name) {
          $scope.metadata[$scope.targetDataSeries.name] = {
            alias: $scope.targetDataSeries.name
          };
        }
      };

      // filtering formats
      $scope.storagerFormats = [];
      $scope.dataSeriesSemantics.forEach(function(dSemantics) {
        if(dSemantics.data_series_type_name === semanticsType) {
          $scope.storagerFormats.push(Object.assign({}, dSemantics));
        }
      });

      // filtering dataseries
      $scope.filteredDataSeries = [];
      $scope.dataSeriesList.forEach(function(dataSeries) {
        var semantics = dataSeries.data_series_semantics;
        if (semantics.data_series_type_name === globals.enums.DataSeriesType.STATIC_DATA) {
          $scope.filteredDataSeries.push(dataSeries);
        }
      });

      if ($scope.isUpdating) { $scope.$emit("analysisTypeChanged"); }
    });

    // targetDataSeries change. When

    // TerraMA2 alert box
    $scope.alertBox = {};
    $scope.display = false;
    $scope.alertLevel = null;
    $scope.close = function() {
      $scope.display = false;
    };

    $scope.formStorager = [];
    $scope.modelStorager = {};
    $scope.schemaStorager = {};
    $scope.options = {};
    if ($scope.isUpdating) { $scope.options.formDefaults = {readonly: true}; }

    $scope.$on('storagerFormatChange', function(event, args) {
      $scope.formatSelected = args.format;
      // todo: fix it. It is hard code

      DataSeriesSemanticsFactory.get(args.format.code, {metadata:true}).success(function(data) {
        var metadata = data.metadata;
        var properties = metadata.schema.properties;

        if ($scope.isUpdating) {
          $scope.modelStorager = configuration.analysis.dataSeries.dataSets[0].format;
        } else { $scope.modelStorager = {}; }

        $scope.formStorager = metadata.form;
        $scope.schemaStorager = {
          type: 'object',
          properties: metadata.schema.properties,
          required: metadata.schema.required
        };

        $scope.dataProviders = [];

        $scope.dataProvidersList.forEach(function(dataProvider) {
          data.data_providers_semantics.forEach(function(demand) {
            if (dataProvider.data_provider_type.id == demand.data_provider_type_id) {
              $scope.dataProviders.push(dataProvider);
            }
          });
        });

        $scope.$broadcast('schemaFormRedraw');
      }).error(function(err) {
        console.log(err);
      });
    });

    DataSeriesSemanticsFactory.list().success(function(semanticsList) {
      var semanticsListFiltered = [];
      semanticsList.forEach(function(element) {
        // Skipping semantics for only collector
        if (!element.collector) {
          semanticsListFiltered.push(element);
        }
      })

      $scope.dataSeriesSemantics = semanticsListFiltered;

      DataProviderFactory.get().success(function(dataProviders) {
        $scope.dataProvidersList = dataProviders;

        // fill analysis in gui
        if ($scope.isUpdating) {
          var analysisInstance = configuration.analysis;

          $scope.analysis.name = analysisInstance.name;
          $scope.analysis.description = analysisInstance.description;
          $scope.analysis.type_id = analysisInstance.type.id.toString();
          $scope.analysis.instance_id = analysisInstance.service_instance_id.toString();
          $scope.analysis.script = analysisInstance.script;

          var historicalData = analysisInstance.reprocessing_historical_data || {};
          if (historicalData.startDate) {
            historicalData.startDate = DateParser(historicalData.startDate);
          }

          if (historicalData.endDate) {
            historicalData.endDate = DateParser(historicalData.endDate);
          }

          $scope.analysis.historical = historicalData;

          // schedule update
          $scope.$broadcast("updateSchedule", analysisInstance.schedule);

          // wait for watchers (type_id)
          $scope.$on("analysisTypeChanged", function(event) {
            analysisInstance.analysis_dataseries_list.forEach(function(analysisDs) {
              var ds = analysisDs.dataSeries;

              if (analysisDs.type === globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE) {
                $scope.selectedDataSeriesList.push(ds);
              } else {
                $scope.filteredDataSeries.some(function(filteredDs) {
                  if (filteredDs.id === ds.id) {
                    $scope.targetDataSeries = filteredDs;
                    $scope.onTargetDataSeriesChange();

                    // set identifier
                    $scope.identifier = analysisDs.metadata.identifier;
                    return true;
                  }
                });
              }

              $scope.metadata[ds.name] = Object.assign({id: analysisDs.id, alias: analysisDs.alias}, analysisDs.metadata);
            });

            if (analysisInstance.type.id === globals.enums.AnalysisType.GRID) {
              // fill interpolation
              debugger;
              $scope.analysis.grid = {
                interpolation_method: analysisInstance.output_grid.interpolation_method,
                area_of_interest_type: analysisInstance.output_grid.area_of_interest_type,
                resolution_type: analysisInstance.output_grid.resolution_type
              };
              var dummy = analysisInstance.output_grid.interpolation_dummy;
              if (dummy) {
                $scope.analysis.grid.interpolation_dummy = Number(dummy);
              }
              var resolutionDS = analysisInstance.output_grid.resolution_data_series_id;
              if (resolutionDS) {
                $scope.analysis.grid.resolution_data_series_id = resolutionDS;
              }
              var interestDS = analysisInstance.output_grid.area_of_interest_data_series_id;
              if (interestDS) {
                $scope.analysis.grid.area_of_interest_data_series_id = interestDS;
              }
              var resX = analysisInstance.output_grid.resolution_x;
              var resY = analysisInstance.output_grid.resolution_y;
              if (resX && resY) {
                $scope.analysis.grid.resolution_x = Number(resX);
                $scope.analysis.grid.resolution_y = Number(resY);
              }
              var coordinates = (analysisInstance.output_grid.area_of_interest_box || {}).coordinates;
              if (coordinates) {
                $scope.analysis.grid.area_of_interest_bounded = {
                  minX: coordinates[0][0][0],
                  minY: coordinates[0][0][1],
                  maxX: coordinates[0][2][0],
                  maxY: coordinates[0][2][1]
                };
              }
            } else if (analysisInstance.type.id === globals.enums.AnalysisType.DCP) {
              console.log("HERE");
              $scope.analysis.metadata.INFLUENCE_TYPE = analysisInstance.metadata.INFLUENCE_TYPE;
              $scope.analysis.metadata.INFLUENCE_RADIUS = analysisInstance.metadata.INFLUENCE_RADIUS;
              $scope.analysis.metadata.INFLUENCE_RADIUS_UNIT = analysisInstance.metadata.INFLUENCE_RADIUS_UNIT;
              $scope.analysis.metadata.INFLUENCE_DATASERIES_ID = analysisInstance.metadata.INFLUENCE_DATASERIES_ID;
              $scope.analysis.metadata.INFLUENCE_ATTRIBUTE = analysisInstance.metadata.INFLUENCE_ATTRIBUTE;
            }

            // setting storager format
            $scope.storagerFormats.some(function(storagerFmt) {
              if (analysisInstance.dataSeries.data_series_semantics.id === storagerFmt.id) {
                $scope.storager.format = storagerFmt;
                $scope.onStoragerFormatChange();
                return true;
              }
            });

            $scope.analysis.data_provider_id = analysisInstance.dataSeries.data_provider_id;
          });
        }

      }).error(function(err) {
        console.log(err);
      });

    }).error(function(err) {
      console.log(err);
    });

    $scope.onStoragerFormatChange = function() {
      $scope.showStoragerForm = true;
      $scope.$broadcast('storagerFormatChange', {format: $scope.storager.format});
    };

    // temp code for debugging
    var errorHelper = function(err) {
      console.log(err);
    };

    // getting instances
    ServiceInstanceFactory.get({type: 'ANALYSIS'}).success(function(services) {
      $scope.instances = services;
    }).error(errorHelper);

    // getting DataSeries
    DataSeriesFactory.get({schema: "all"}).success(function(dataSeriesObjects) {
      $scope.dataSeriesList = dataSeriesObjects;
      // fill buffers
      _processBuffers();
    }).error(errorHelper);

    $scope.$watch("targetDataSeries", function(newValue) {
      var hasNewValue = Object.keys(newValue).length !== 1;
      if (!hasNewValue) {
        return;
      }

      $scope.buffers.static = [];
      ($scope.dataSeriesList || []).forEach(function(dataSeries) {
        if (dataSeries.data_series_semantics.data_series_type_name === "STATIC_DATA") {
          if (dataSeries.id !== newValue.id) {
            console.log(dataSeries);
            $scope.buffers.static.push(dataSeries);
          }
        }
      });
      $scope.dataSeriesGroups[0].children = $scope.buffers.static;
    });

    $scope.$watch("analysis.grid.area_of_interest_type", function(value) {
      if (value) {
        if (value === $scope.interestAreaTypes.CUSTOM.value) {
          var analysisConfig = (configuration.analysis || {});
          var value = (analysisConfig.output_grid || {}).srid || 4326;
          if ($scope.analysis.grid.area_of_interest_bounded) {
            $scope.analysis.grid.area_of_interest_bounded.srid = value;
          } else {
            $scope.analysis.grid.area_of_interest_bounded = {
              srid: value
            }
          }
        } else {

        }
      }
    })

    // helpers
    var _processBuffers = function() {
      // clean old data
      $scope.buffers = {
        "dynamic": [],
        "static": []
      };

      // cleaning already selected data series
      $scope.selectedDataSeriesList = [];

      if (parseInt($scope.analysis.type_id) === globals.enums.AnalysisType.GRID) {
        $scope.dataSeriesList.forEach(function(dSeries) {
          var semantics = dSeries.data_series_semantics;

          if (semantics.data_series_type_name === globals.enums.DataSeriesType.GRID) {
            dSeries.isDynamic = true;
            $scope.buffers.dynamic.push(dSeries);
          }
        });
      } else {
        $scope.dataSeriesList.forEach(function(dSeries) {
          var semantics = dSeries.data_series_semantics;

          if (semantics.data_series_type_name === "STATIC_DATA") {
            // skip target data series in additional data
            if ($scope.targetDataSeries && $scope.targetDataSeries.id !== dSeries.id) {
              dSeries.isDynamic = false;
              $scope.buffers.static.push(dSeries);
            }
          }
          else {
            dSeries.isDynamic = true;
            $scope.buffers.dynamic.push(dSeries);
          }
        });
      }

      $scope.dataSeriesGroups[0].children = $scope.buffers.static;
      $scope.dataSeriesGroups[1].children = $scope.buffers.dynamic;
    };

    var formErrorDisplay = function(form) {
      angular.forEach(form.$error, function (field) {
        angular.forEach(field, function(errorField){
          errorField.$setDirty();
        });
      });
    };

    $scope.onAreaOfInterestChange = function() {
      // resetting area of interest values
      $scope.analysis.grid.area_of_interest_bounded = {};
      $scope.analysis.grid.area_of_interest_data_series_id = null;
    };

    /**
     * It is triggered when a resolution combobox has been changed. It aims to clear the resolution values from scope model.
     */
    $scope.onResolutionChanged = function() {
      $scope.analysis.grid.resolution_data_series_id = null;
      $scope.analysis.grid.resolution_x = null;
      $scope.analysis.grid.resolution_y = null;
    };

    var makeDialog = function(level, bodyMessage, show, title) {
      $scope.alertBox.title = title || "Analysis Registration";
      $scope.alertBox.message = bodyMessage;
      $scope.alertLevel = level;
      $scope.display = show;
    };

    // handling functions
    // checking for empty data series table
    $scope.isEmptyDataSeries = function() {
      return $scope.selectedDataSeriesList.length === 0;
    };

    // it adds dataseries from modal to table
    $scope.addDataSeries = function() {
      var _helper = function(type, target) {
        $scope.buffers[type].some(function(element, index, arr) {
          if (element.id == target.id) {
            arr.splice(index, 1);
            return true;
          }
          return false;
        });
      };

      $scope.nodesDataSeries.forEach(function(target) {
        if (!target || !target.id)
          return;

        $scope.metadata[target.name] = {alias: target.name};
        $scope.selectedDataSeriesList.push(target);

        if (target.isDynamic) {
          _helper("dynamic", target);
        } else {
          _helper("static", target);
        }
      });

      $scope.nodesDataSeries = [];
    };

    $scope.removeDataSeries = function(dataSeries) {

      var _pushToBuffer = function(type, obj) {
        $scope.buffers[type].push(obj);
      };

      $scope.selectedDataSeriesList.some(function(dSeries, index, arr) {
        if (dSeries.id == dataSeries.id) {
          arr.splice(index, 1);
          var type = dSeries.isDynamic ? "dynamic" : "static";
          _pushToBuffer(type, dSeries);
          return true;
        }
      });
    };

    // it check if there is a dataseries selected
    $scope.isAnyDataSeriesSelected = function() {
      return $scope.selectedDataSeries && $scope.selectedDataSeries.id > 0;
    };

    // it handles hidden box with data-series analysis metadata
    $scope.onDataSeriesClick = function(dataSeries) {
      if ($scope.selectedDataSeries) {
        if (dataSeries.id == $scope.selectedDataSeries.id) {
          $scope.selectedDataSeries = {};
          return;
        }
      }

      $scope.metadata[dataSeries.name] = $scope.metadata[dataSeries.name] || {};
      $scope.selectedDataSeries = dataSeries;

      // getting data series semantics
      DataSeriesSemanticsFactory.get(dataSeries.data_series_semantics.code).success(function(data) {
        $scope.semantics = data;
      }).error(errorHelper);
    };

    // pcd metadata (radius format - km, m...)
    $scope.onMetadataFormatClick = function(format) {
      $scope.analysis.metadata.INFLUENCE_RADIUS_UNIT = format;
    };

    // save function
    $scope.save = function() {

      $scope.close();
      $scope.$broadcast('formFieldValidation');

      $scope.analysis_script_error = false;
      if ($scope.forms.generalDataForm.$invalid) {
        makeDialog("alert-danger", "There are invalid fields on form", true);
        return;
      }

      if ($scope.forms.storagerDataForm.$invalid || $scope.forms.storagerForm.$invalid) {
        makeDialog("alert-danger", "There are invalid fields on form", true);
        return;
      }

      // TODO: emit a signal to validate form like $scope.$broadcast('scheduleFormValidate')
      var scheduleForm = angular.element('form[name="scheduleForm"]').scope().scheduleForm;
      if (scheduleForm.$invalid) { 
        makeDialog("alert-danger", "There are invalid fields on form", true);
        return;
      }

      if ($scope.forms.targetDataSeriesForm.$invalid) { 
        makeDialog("alert-danger", "There are invalid fields on form", true);
        return;
      }

      if ($scope.forms.scriptForm.$invalid) {
        makeDialog("alert-danger", "There are invalid fields on form", true);
        return;
      }

      // checking script form if there any "add_value"
      var typeId = parseInt($scope.analysis.type_id);

      var checkResult = angular.element("#scriptCheckResult");

      var hasScriptError = function(expression, message) {
        var output = false;
        if ($scope.analysis.script.indexOf(expression) < 0) {
          $scope.analysis_script_error = true;
          $scope.analysis_script_error_message = "Analysis will not able to generate a output data. " + message;
          output = true;
        } else {
          $scope.analysis_script_error_message = "";
          $scope.analysis_script_error = false;
        }
        checkResult.html($scope.analysis_script_error_message);
        return output;
      };

      var expression, message;

      if (typeId === globals.enums.AnalysisType.GRID) {
        expression = "return";
        message = "Grid analysis script must end with 'return' statement";
      } else {
        expression = "add_value";
        message = "Please fill at least a add_value() in script field.";
      }
      if (hasScriptError(expression, message)) {
        return;
      }

      // checking dataseries analysis
      var dataSeriesError = {};
      var hasError = $scope.selectedDataSeriesList.some(function(dSeries) {
        if (!$scope.metadata[dSeries.name]) {
          dataSeriesError = dSeries;
          return true;
        }
      });

      if (hasError) {
        makeDialog("alert-danger", "Invalid data series. Please fill out alias in " + dataSeriesError.name, true);
        return;
      }

      // cheking influence form: DCP and influence form valid
      if ($scope.analysis.type_id == 1) {
        var form = $scope.forms.influenceForm;
        if (form.$invalid) {
          formErrorDisplay(form);
          return;
        }
      } else if ($scope.analysis.type_id == globals.enums.AnalysisType.GRID) {
        if ($scope.forms.gridForm.$invalid) {
          return;
        }
      }

      var analysisDataSeriesArray = [];

      var _makeAnalysisDataSeries = function(selectedDS, type_id) {
        var metadata = Object.assign({}, $scope.metadata[selectedDS.name] || {});
        var alias = ($scope.metadata[selectedDS.name] || {}).alias;

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
          $scope.metadata[$scope.targetDataSeries.name]['identifier'] = $scope.identifier;
          // setting monitored object id in output data series format
          $scope.modelStorager.monitored_object_id = $scope.targetDataSeries.id;
          $scope.modelStorager.monitored_object_pk = $scope.identifier;
          break;
      }

      // preparing data to send
      var analysisToSend = Object.assign({}, $scope.analysis);

      // setting target data series metadata (monitored object, dcp..)
      if (typeId !== globals.enums.AnalysisType.GRID) {
        analysisDataSeriesArray.push(_makeAnalysisDataSeries($scope.targetDataSeries, analysisTypeId));
      } else {
        // checking geojson
        if ($scope.analysis.grid && $scope.analysis.grid.area_of_interest_bounded &&
            !angular.equals({}, $scope.analysis.grid.area_of_interest_bounded)) {

          var boundedForm = (angular.element('form[name="boundedForm"]').scope() || {boundedForm: {}}).boundedForm;
          if (boundedForm.$invalid) {
            return;
          }

          var bounded = $scope.analysis.grid.area_of_interest_bounded;
          var coordinates = [
            [
              [bounded.minX, bounded.minY],
              [bounded.minX, bounded.maxY],
              [bounded.maxX, bounded.maxY],
              [bounded.maxX, bounded.minY],
              [bounded.minX, bounded.minY]
            ]
          ];
          analysisToSend.grid.area_of_interest_box = {
            type: 'Polygon',
            coordinates: coordinates,
            crs: {
              type: 'name',
              properties : {
                name: "EPSG:" + bounded.srid || "4326"
              }
            }
          };
          analysisToSend.grid.srid = bounded.srid;
        }
      }

      // todo: improve it
      // temp code for sending analysis data series
      $scope.selectedDataSeriesList.forEach(function(selectedDS) {
        // additional data
        var analysisDataSeries = _makeAnalysisDataSeries(selectedDS, globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE);
        analysisDataSeriesArray.push(analysisDataSeries);
      });

      analysisToSend.dataSeries = $scope.selectedDataSeriesList;
      analysisToSend.analysisDataSeries = analysisDataSeriesArray;

      var storager = {};
      storager.format = Object.assign({}, $scope.modelStorager);
      storager.semantics = Object.assign({}, $scope.storager);

      var scheduleValues = Object.assign({}, $scope.schedule);
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
        schedule: scheduleValues
      };

      var request;

      if ($scope.isUpdating) { request = AnalysisFactory.put(configuration.analysis.id, objectToSend); }
      else { request = AnalysisFactory.post(objectToSend); }

      request.success(function(data) {
        window.location = "/configuration/analysis?token=" + data.token;
      }).error(function(err) {
        console.log(err);
        makeDialog("alert-danger", err.message, true);
      });
    };
  }]);
