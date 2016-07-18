angular.module('terrama2.analysis.registration', [
    'terrama2',
    'terrama2.services',
    'terrama2.components.messagebox',
    'terrama2.ace',
    'ui.bootstrap.datetimepicker',
    'ui.dateTimeInput',
    'schemaForm',
    'terrama2.schedule',
    'treeControl'
  ])

  .controller('AnalysisRegistration',
    [
      '$scope',
      'i18n',
      'ServiceInstanceFactory',
      'DataSeriesFactory',
      'DataSeriesSemanticsFactory',
      'AnalysisFactory',
      'DataProviderFactory',
      'TryCaster',
      'Socket',
  function($scope, i18n, ServiceInstanceFactory, DataSeriesFactory, DataSeriesSemanticsFactory, AnalysisFactory, DataProviderFactory, TryCaster, Socket) {
    var socket = Socket;

    // injecting i18n module
    $scope.i18n = i18n;

    // TerraMA2 box
    $scope.css = {
      boxType: "box-solid"
    };

    // flag to handling script status
    $scope.testingScript = false;

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
      $scope.analysis.script = editor.getSession().getDocument().getValue();
    }

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
      socket.emit('checkPythonScriptRequest', {script: $scope.analysis.script || ""});
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
    }

    // filter for dataseries basead analysis type. If obj monitored, then this list will be list of obj monitored and occurrences
    $scope.filteredDataSeries = [];

    // helper of semantics selected to display in gui: Object Monitored, Dcp, Grid, etc.
    $scope.semanticsSelected = "";

    // dataseries tree modal
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
    }

    $scope.dataSeriesGroups = [
      {name: "Static", children: []},
      {name: "Dynamic", children: []}
    ]

    // watchers
    // cleaning analysis metadata when analysis type has been changed.
    // fill storager formats
    $scope.$watch("analysis.type_id", function(value) {
      $scope.analysis.metadata = {};
      var semanticsType;
      switch(parseInt(value)) {
        case globals.enums.AnalysisType.DCP:
          semanticsType = globals.enums.DataSeriesType.DCP;
          $scope.semanticsSelected = "Dcp";
          break;
        case globals.enums.AnalysisType.GRID:
          semanticsType = globals.enums.DataSeriesType.GRID;
          $scope.semanticsSelected = "Grid";
          break;
        case globals.enums.AnalysisType.MONITORED:
          semanticsType = globals.enums.DataSeriesType.ANALYSIS_MONITORED_OBJECT;
          $scope.semanticsSelected = "Object Monitored";
          break;
        default:
          console.log("invalid analysis type");
          return;
      }

      $scope.onTargetDataSeriesChange = function() {
        if ($scope.targetDataSeries && $scope.targetDataSeries.name)
          $scope.metadata[$scope.targetDataSeries.name] = {
            alias: $scope.targetDataSeries.name
          };
      }

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
        if (semantics.data_series_type_name === globals.enums.DataSeriesType.STATIC_DATA)
          $scope.filteredDataSeries.push(dataSeries);
      });

      if ($scope.isUpdating)
        $scope.$emit("analysisTypeChanged");
    });

    // targetDataSeries change. When

    // terrama2 alert box
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
    if ($scope.isUpdating)
      $scope.options.formDefaults = {readonly: true};

    $scope.$on('storagerFormatChange', function(event, args) {
      $scope.formatSelected = args.format;
      // todo: fix it. It is hard code

      DataSeriesSemanticsFactory.get(args.format.code, {metadata:true}).success(function(data) {
        var metadata = data.metadata;
        var properties = metadata.schema.properties;

        if ($scope.isUpdating) {
          $scope.modelStorager = configuration.analysis.dataSeries.dataSets[0].format;
        } else
          $scope.modelStorager = {};

        $scope.formStorager = metadata.form;
        $scope.schemaStorager = {
          type: 'object',
          properties: metadata.schema.properties,
          required: metadata.schema.required
        };

        $scope.dataProviders = [];

        $scope.dataProvidersList.forEach(function(dataProvider) {
          data.data_providers_semantics.forEach(function(demand) {
            if (dataProvider.data_provider_type.id == demand.data_provider_type_id)
              $scope.dataProviders.push(dataProvider);
          })
        });

        $scope.$broadcast('schemaFormRedraw');
      }).error(function(err) {
        console.log(err);
      });
    });

    DataSeriesSemanticsFactory.list().success(function(semanticsList) {
      $scope.dataSeriesSemantics = semanticsList;

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

          // schedule update
          $scope.$broadcast("updateSchedule", analysisInstance.schedule);

          // wait for watchers (type_id)
          $scope.$on("analysisTypeChanged", function(event) {
            analysisInstance.analysis_dataseries_list.forEach(function(analysisDs) {
              var ds = analysisDs.dataSeries;

              if (analysisDs.type === globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE)
                $scope.selectedDataSeriesList.push(ds);
              else {
                $scope.filteredDataSeries.some(function(filteredDs) {
                  if (filteredDs.id === ds.id) {
                    $scope.targetDataSeries = filteredDs;
                    $scope.onTargetDataSeriesChange();

                    // set identifier
                    $scope.identifier = analysisDs.metadata['identifier'] ;
                    return true;
                  }
                })
              }

              $scope.metadata[ds.name] = Object.assign({alias: analysisDs.alias}, analysisDs.metadata);
            });

            // setting storager format
            $scope.storagerFormats.some(function(storagerFmt) {
              if (analysisInstance.dataSeries.data_series_semantics.id === storagerFmt.id) {
                $scope.storager.format = storagerFmt;
                $scope.onStoragerFormatChange();
                return true;
              }
            });

            $scope.analysis.data_provider_id = analysisInstance.dataSeries.data_provider_id;
          })

          // TODO: change it to angular ui-ace.
          editor.setValue($scope.analysis.script);
          // editor.setOptions({
          //   readOnly: true,
          //   highlightActiveLine: false,
          //   highlightGutterLine: false
          // })
          // editor.renderer.$cursorLayer.element.style.opacity=0

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

      dataSeriesObjects.forEach(function(dSeries) {
        var semantics = dSeries.data_series_semantics;

        if (semantics.data_series_type_name == "STATIC_DATA") {
          dSeries.isDynamic = false;
          $scope.buffers["static"].push(dSeries);
        }
        else {
          dSeries.isDynamic = true;
          $scope.buffers["dynamic"].push(dSeries);
        }
      });

      $scope.dataSeriesGroups[0].children = $scope.buffers["static"];
      $scope.dataSeriesGroups[1].children = $scope.buffers["dynamic"];
    }).error(errorHelper);

    // helpers
    var formErrorDisplay = function(form) {
      angular.forEach(form.$error, function (field) {
        angular.forEach(field, function(errorField){
          errorField.$setDirty();
        })
      });
    };

    var makeDialog = function(level, bodyMessage, show, title) {
      $scope.alertBox.title = title || "Analysis Registration";
      $scope.alertBox.message = bodyMessage;
      $scope.alertLevel = level;
      $scope.display = show;
    }

    // handling functions
    // checking for empty data series table
    $scope.isEmptyDataSeries = function() {
      return $scope.selectedDataSeriesList.length === 0;
    }

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
      }

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
      })

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
      })
    }

    // it check if there is a dataseries selected
    $scope.isAnyDataSeriesSelected = function() {
      return $scope.selectedDataSeries && $scope.selectedDataSeries.id > 0;
    }

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
      $scope.analysis_script_error = false;
      if ($scope.generalDataForm.$invalid) {
        formErrorDisplay($scope.generalDataForm);
        return;
      }

      if ($scope.storagerDataForm.$invalid || $scope.storagerForm.$invalid) {
        formErrorDisplay($scope.storagerDataForm);
        return;
      }

      // TODO: emit a signal to validate form like $scope.$broadcast('scheduleFormValidate')
      var scheduleForm = angular.element('form[name="scheduleForm"]').scope().scheduleForm;
      if (scheduleForm.$invalid) {
        errorHelper(scheduleForm);
        return;
      }

      if ($scope.targetDataSeriesForm.$invalid) {
        formErrorDisplay($scope.targetDataSeriesForm);
        return;
      }

      if ($scope.scriptForm.$invalid) {
        formErrorDisplay($scope.scriptForm);
        return;
      }

      // checking script form if there any "add_value"
      if ($scope.analysis.script.indexOf("add_value") < 0) {
        $scope.analysis_script_error = true;
        $scope.analysis_script_error_message = "Analysis will not able to generate a output data. Please fill at least a add_value() in script field.";
        angular.element("#scriptCheckResult").html($scope.analysis_script_error_message);
        // makeDialog("alert-danger", $scope.analysis_script_error_message, true);
        return;
      }

      // checking dataseries analysis
      var dataSeriesError = {};
      var hasError = $scope.selectedDataSeriesList.some(function(dSeries) {
        if (!$scope.metadata[dSeries.name]) {
          dataSeriesError = dSeries;
          return true;
        }
      })

      if (hasError) {
        makeDialog("alert-danger", "Invalid data series. Please fill out alias in " + dataSeriesError.name, true);
        return;
      }

      // cheking influence form: DCP and influence form valid
      if ($scope.analysis.type_id == 1) {
        var form = angular.element('form[name="influenceForm"]').scope()['influenceForm'];
        if (form.$invalid) {
          formErrorDisplay(form);
          return;
        }
      }

      var analysisDataSeriesArray = [];

      var _makeAnalysisDataSeries = function(selectedDS, type_id) {
        var metadata = Object.assign({}, $scope.metadata[selectedDS.name] || {});
        var alias = ($scope.metadata[selectedDS.name] || {}).alias;

        delete metadata.alias;

        return {
          data_series_id: selectedDS.id,
          metadata: metadata,
          alias: alias,
          // todo: check it
          type_id: type_id
        };
      };

      // target data series
      var analysisTypeId;
      switch(parseInt($scope.analysis.type_id)) {
        case globals.enums.AnalysisType.DCP:
          analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_DCP_TYPE;
          break;
        case globals.enums.AnalysisType.GRID:
          analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_GRID_TYPE;
          break;
        case globals.enums.AnalysisType.MONITORED:
          analysisTypeId = globals.enums.AnalysisDataSeriesType.DATASERIES_MONITORED_OBJECT_TYPE;
          $scope.metadata[$scope.targetDataSeries.name]['identifier'] = $scope.identifier;
          break;
      }

      // setting target data series metadata (monitored object, dcp..)
      analysisDataSeriesArray.push(_makeAnalysisDataSeries($scope.targetDataSeries, analysisTypeId));

      // todo: improve it
      // temp code for sending analysis dataseries
      $scope.selectedDataSeriesList.forEach(function(selectedDS) {
        // additional data
        var analysisDataSeries = _makeAnalysisDataSeries(selectedDS, globals.enums.AnalysisDataSeriesType.ADDITIONAL_DATA_TYPE);
        analysisDataSeriesArray.push(analysisDataSeries);
      });

      var analysisToSend = Object.assign({}, $scope.analysis);
      analysisToSend.dataSeries = $scope.selectedDataSeriesList;
      analysisToSend.analysisDataSeries = analysisDataSeriesArray;

      var storager = Object.assign({}, $scope.storager, $scope.modelStorager);

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

      if ($scope.isUpdating)
        request = AnalysisFactory.put(configuration.analysis.id, objectToSend);
      else
        request = AnalysisFactory.post(objectToSend);

      request.success(function(data) {
        window.location = "/configuration/analyses?token=" + data.token;
      }).error(function(err) {
        console.log(err);
        makeDialog("alert-danger", err.message, true);
      });
    };
  }]);
