angular.module('terrama2.dataseries.registration', [
    'terrama2',
    'terrama2.services',
    'terrama2.components.messagebox', // handling alert box
    'ui.router',
    'mgo-angular-wizard', // wizard
    'ui.bootstrap.datetimepicker',
    'ui.dateTimeInput',
    'schemaForm',
    'xeditable'
  ])
  .config(["$stateProvider", "$urlRouterProvider", function($stateProvider, $urlRouterProvider) {
    $stateProvider.state('main', {
      abstract: true,
      template: '<div ui-view=""></div>',
      resolve: {
        "i18nData": ["i18n", function (i18n) {
          return i18n.ensureLocaleIsLoaded();
        }]
      }
    });

    $urlRouterProvider.otherwise(function ($injector, $location) {
      var $state = $injector.get('$state');

      // It prevents wrong state in user request.
      if ($location.$$hash.indexOf("/wizard" == -1) || $location.$$hash.indexOf("/advanced") == -1)
        if ($state.current.name) // go to current state
          $state.go($state.current.name);
        else // go to default state
          $state.go('advanced');
    });

    $stateProvider
      .state('wizard', {
        parent: 'main',
        url: "/wizard",
        templateUrl: '/javascripts/angular/wizard.html'
      }).state('advanced', {
        parent: 'main',
        url: "/advanced",
        templateUrl: '/javascripts/angular/advanced.html'
      }
    );
  }])

  .run(function(editableOptions) {
    editableOptions.theme = 'bs3'; // bootstrap3 theme. Can be also 'bs2', 'default'
  })

  .controller('StoragerController', ['$scope', 'i18n', 'DataSeriesSemanticsFactory', function($scope, i18n, DataSeriesSemanticsFactory) {
    $scope.formStorager = [];
    $scope.modelStorager = {};
    $scope.schemaStorager = {};
    $scope.options = {};
    $scope.tableFieldsStorager = [];
    $scope.formatSelected = {};
    $scope.dcpsStorager = [];
    $scope.inputDataSets = [];
    $scope.storage = {};

    var removeInput = function(dcpMask) {
      $scope.inputDataSets.forEach(function(dcp, pcdIndex, array) {
        // todo: which fields should compare to remove?
        if (dcp.mask === dcpMask) {
          array.splice(pcdIndex, 1);
          return;
        }
      });
    };

    $scope.removePcdStorager = function(dcpItem) {
      $scope.dcpsStorager.forEach(function(dcp, pcdIndex, array) {
        // todo: which fields should compare to remove?
        if (dcp.id === dcpItem.id) {
          array.splice(pcdIndex, 1);
          $scope.$emit("storagerDcpRemoved", dcpItem);
          return;
        }
      });
    };

    $scope.addDcpStorager = function() {
      $scope.$broadcast('schemaFormValidate');
      var form = angular.element('form[name="storagerForm"]').scope()['storagerForm'];
      var inputDataSetForm = angular.element('form[name="inputDataSetForm"]').scope()['inputDataSetForm'];
      if (form.$valid && inputDataSetForm.$valid) {
        $scope.model['inputDataSet'] = $scope.storage.inputDataSet.mask;
        $scope.dcpsStorager.push(Object.assign({}, $scope.model));
        $scope.model = {};

        // remove it from input list
        removeInput($scope.storage.inputDataSet.mask);

        // reset form to do not display feedback class
        form.$setPristine();
        inputDataSetForm.$setPristine();
      }
    };

    $scope.$on("requestStorageValues", function(event) {
      // apply a validation
      $scope.$broadcast('schemaFormValidate');
      var form = angular.element('form[name="storagerForm"]').scope()['storagerForm'];
      var dataForm = angular.element('form[name="storagerDataForm"]').scope()['storagerDataForm'];
      if (form.$valid && dataForm.$valid) {
        // checking if it is a dcp
        switch ($scope.formatSelected.data_series_type_name) {
          case "Dcp":
            $scope.$emit("storageValuesReceive", {
              data: $scope.dcpsStorager,
              data_provider: $scope['storager_data_provider_id'],
              service: $scope["storager_service"],
              type: $scope.formatSelected.data_series_type_name,
              semantics: $scope.formatSelected
            });
            break;
          case "Occurrence":
            $scope.$emit("storageValuesReceive", {
              data: $scope.modelStorager,
              data_provider: $scope['storager_data_provider_id'],
              service: $scope["storager_service"],
              type: $scope.formatSelected.data_series_type_name,
              semantics: $scope.formatSelected
            });
            break;
          default:
            $scope.$emit("storageValuesReceive", {data: null, type: null});
            break;
        }
      } else {
        angular.forEach(dataForm.$error, function (field) {
          angular.forEach(field, function(errorField){
            errorField.$setDirty();
          })
        });
      }
    });

    $scope.$on("dcpOperation", function(event, args) {
      if (args.action === "remove") {
        $scope.removePcdStorager(args.dcp);
      //  todo: remove it from list
        removeInput(args.dcp.mask);
      } else if (args.action === "add") {
        $scope.dcpsStorager.push({table_name: args.dcp.mask, id: args.dcp.id});
        // $scope.inputDataSets.push({table_name: args.dcp.mask, inputDataSet: args.dcp.mask});
      }
    });

    $scope.$on("resetStoragerDataSets", function(event) {
      $scope.dcpsStorager = [];
    });

    $scope.$on('storagerFormatChange', function(event, args) {
      $scope.formatSelected = args.format;
      // todo: fix it. It is hard code
      // $scope.tableFieldsStorager = ["table_name", "inputDataSet"];

      DataSeriesSemanticsFactory.get(args.format.name, {metadata:true}).success(function(data) {
        var metadata = data.metadata;
        var properties = metadata.schema.properties;
        if ($scope.formatSelected.data_series_type_name === "Dcp") {
          Object.keys(properties).forEach(function(key) {
            $scope.tableFieldsStorager.push(key);
          });
        } else {
        //   occurrence
          $scope.modelStorager = {};
          $scope.formStorager = metadata.form;
          $scope.schemaStorager = {
            type: 'object',
            properties: metadata.schema.properties,
            required: metadata.schema.required
          };
          $scope.$broadcast('schemaFormRedraw');
        }

      }).error(function(err) {

      });
    });

  }])

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsFactory", "DataProviderFactory", "DataSeriesFactory", "ServiceInstanceFactory",
    function($scope, $http, i18n, $window, $state, $httpParamSerializer, DataSeriesSemanticsFactory, DataProviderFactory, DataSeriesFactory, ServiceInstanceFactory) {
      // definition of schema form
      $scope.schema = {};
      $scope.form = [];
      $scope.model = {};

      $scope.service = {};

      $scope.alertLevel = "";

      // terrama2 messagebox
      $scope.errorFound = false;
      $scope.alertBox = {};
      $scope.display = false;
      $scope.extraProperties = {};
      $scope.resetState = function() {
        $scope.errorFound = false;
        $scope.alertBox.message = "";
        $scope.display = false;
      };

      // table fields
      $scope.tableFields = [];

      // injecting state handler in scope
      $scope.stateApp = $state;

      // injecting helper functions in scope
      $scope.capitalizeIt = function(text) {
        return text.charAt(0).toUpperCase() + text.slice(1);
      };
      $scope.isBoolean = function(value) {
        return typeof value === 'boolean';
      };

      // wizard helper
      var isWizardStepValid = function(formName, isSchemaForm) {
        // todo: temp code. It is a "trick" for getting undefined form in scope. It must be changed
        var form = angular.element('form[name="'+ formName + '"]').scope()[formName];

        if (isSchemaForm == true) {
          $scope.$broadcast('schemaFormValidate');
        }

        if (form.$valid)
          return true;

        errorHelper(form);
        return false;
      };

      // storager
      $scope.showStoragerForm = false;
      $scope.storager = {};
      $scope.formStorager = [];
      $scope.modelStorager = {};
      $scope.schemaStorager = {};
      $scope.onStoragerFormatChange = function() {
        console.log($scope.dataSeries.access);
        $scope.showStoragerForm = true;
        $scope.$broadcast('storagerFormatChange', {format: $scope.storager.format, dcps: $scope.dcps});
      };

      $scope.$on("storagerDcpRemoved", function(event, dcp) {
        $scope.dcps.forEach(function(element, index, arr) {
          if (element.mask == dcp.inputDataSet) {
            arr.splice(index, 1);
            return;
          }
        });
      });

      // schedule
      $scope.range = function(min, max) {
        var output = [];
        for(var i = min; i < max; ++i)
          output.push(i);
        return output;
      };
      $scope.schedule = {};
      $scope.isFrequency = false;
      $scope.isSchedule = false;
      $scope.services = [];
      // fix: temp code
      ServiceInstanceFactory.get({type: 'COLLECT'}).success(function(services) {
        $scope.services = services;
      }).error(function(err) {
        console.log(err);
      });


      $scope.onScheduleChange = function(value) {
        var resetHelper = function(i) {
          if (i == 1) {
            delete $scope.schedule.schedule;
            delete $scope.schedule.schedule_retry;
            delete $scope.schedule.schedule_retry_unit;
            delete $scope.schedule.schedule_timeout;
            delete $scope.schedule.schedule_timeout_unit;
            $scope.isFrequency = true;
            $scope.isSchedule = false;
          } else if (i == 2) {
            delete $scope.frequency;
            delete $scope.frequency_unit;
            $scope.isFrequency = false;
            $scope.isSchedule = true;
          }
        };

        switch(value) {
          case "seconds":
          case "minutes":
            resetHelper(1);
            $scope.minSchedule = 1;
            $scope.maxSchedule = 60;
            break;
          case "hours":
            resetHelper(1);
            $scope.minSchedule = 1;
            $scope.maxSchedule = 24;
            break;
          case "weekly":
            resetHelper(2);
            $scope.minSchedule = 1;
            $scope.maxSchedule = 7;
            break;
          case "monthly":
            resetHelper(2);
            $scope.minSchedule = 1;
            $scope.maxSchedule = 31;
            break;
          case "yearly":
            resetHelper(2);
            $scope.minSchedule = 1;
            $scope.maxSchedule = 366;
            break;
          default:
            $scope.minSchedule = 0;
            $scope.maxSchedule = 0;
            break;
        }
      };

      // Wizard validations
      $scope.isFirstStepValid = function(obj) {
        this["wzData"].error = !isWizardStepValid("generalDataForm");
        return true;
      };

      $scope.isSecondStepValid = function(obj) {
        if ($scope.dataSeries.semantics.data_series_type_name === "Dcp")
          if ($scope.dcps.length === 0) {
            // todo: display alert box
            console.log("it should have at least one dcp");
            this["wzData"].error = true;
            return true;
          } else {
            this["wzData"].error = false;
            return true;
          }
        this["wzData"].error = !isWizardStepValid("parametersForm", true);
        return true;
      };

      $scope.isThirdStepValid = function(obj) {
        this["wzData"].error = !isWizardStepValid("storagerForm");
        return true;
      };

      $scope.semantics = "";
      $scope.isDynamic = params.state.toLowerCase() === "dynamic";
      $scope.dcps = [];

      $scope.updatingDcp = false;

      // filter values
      $scope.filter = {date: {}};
      $scope.radioPreAnalysis = {};
      $scope.handlePreAnalysisFilter = function(selected) {
        $scope.filter.pre_analysis = {};
        $scope.radioPreAnalysis = selected;
      };
      $scope.beforeRenderStartDate = function($view, $dates, $leftDate, $upDate, $rightDate) {
        if ($scope.filter.date.afterDate) {
          var activeDate = moment($scope.filter.date.afterDate);
          for (var i = 0; i < $dates.length; i++) {
            if ($dates[i].localDateValue() >= activeDate.valueOf()) $dates[i].selectable = false;
          }
        }
      };
      $scope.beforeRenderEndDate = function($view, $dates, $leftDate, $upDate, $rightDate) {
        if ($scope.filter.date.beforeDate) {
          var activeDate = moment($scope.filter.date.beforeDate).subtract(1, $view).add(1, 'minute');
          for (var i = 0; i < $dates.length; i++) {
            if ($dates[i].localDateValue() <= activeDate.valueOf()) {
              $dates[i].selectable = false;
            }
          }
        }
      };

      $scope.parametersData = configuration.parametersData || {};

      $scope.dataSeries = {
        data_provider_id: configuration.dataSeries.data_provider_id || "",
        name: configuration.dataSeries.name || "",
        access: configuration.dataSeries.access
      };

      $scope.dataSeries.semantics = {};

      DataSeriesSemanticsFactory.list().success(function(semanticsList) {
        $scope.dataSeriesSemantics = semanticsList;

        if (configuration.dataSeries.semantics) {
          semanticsList.forEach(function(semantics) {
            if (semantics.name === configuration.dataSeries.semantics) {
              $scope.dataSeries.semantics = semantics;
              $scope.onDataSemanticsChange();
            }
          })
        } else if (semanticsList.length > 0) {
          $scope.dataSeries.semantics = semanticsList[0];
          $scope.onDataSemanticsChange();
        }

      }).error(function(err) {
        console.log(err);
      });

      DataProviderFactory.get().success(function(dataProviders) {
        $scope.dataProviders = dataProviders;
      }).error(function(err) {
        console.log(err);
      });

      $scope.i18n = i18n;
      $scope.isWizard = true;
      $scope.projection = "";

      // change form: advanced or wizard
      $scope.onFormView = function() {
        $scope.isWizard = !$scope.isWizard;
      };

      // parameters to handle fields display
      $scope.params = params;
      $scope.dataSets = [];

      $scope.initializing = true;
      // detect if is processing or collect
      $scope.$watch("storager.format", function(val) {
        if ($scope.initializing) {
          $scope.initializing = false;
        } else {
          if (val && Object.keys(val).length == 0) {
            $scope.dataSeries.access = 'PROCESSING';
            // $scope.alertLevel = "alert-warning";
            // $scope.alertBox.title = "Data Series";
            // $scope.alertBox.message = "Note: Tha data will be acquired when it has been accessed";
            // $scope.display = true;
          } else {
            $scope.dataSeries.access = 'COLLECT';

            // $scope.display = false;
          }
        }
      });

      // it defines when data change combobox has changed and it will adapt the interface
      $scope.onDataSemanticsChange = function() {
        $scope.semantics = $scope.dataSeries.semantics.data_series_type_name.toLowerCase();
        $scope.storager.format = {};
        $scope.storagerFormats = [];
        $scope.showStoragerForm = false;

        // if is dcp postgis, it shouldn't have a storager and it is processing
        // if ($scope.dataSeries.semantics.data_series_type_name == 'Dcp' && $scope.dataSeries.semantics.name != 'DCP-POSTGIS') {
        //   $scope.storagerFormats = [{name: 'DCP-postgis', data_series_type_name: 'Dcp'}];
        // } else {
        //   $scope.dataSeriesSemantics.forEach(function(dSemantics) {
        //     if (dSemantics.data_series_type_name === "Occurrence") {
        //       $scope.storagerFormats.push(Object.assign({}, dSemantics));
        //     }
        //   });
        // }

        $scope.dataSeriesSemantics.forEach(function(dSemantics) {
          if (dSemantics.data_series_type_name === $scope.dataSeries.semantics.data_series_type_name) {
            $scope.storagerFormats.push(Object.assign({}, dSemantics));
          }
        });

        DataSeriesSemanticsFactory.get($scope.dataSeries.semantics.name, {metadata:true}).success(function(data) {
          $scope.tableFields = [];
          // building table fields. Check if form is for all ('*')
          if (data.metadata.form.indexOf('*') != -1) {
            // ignore form and make it from properties
            var properties = data.metadata.schema.properties;
            for(var key in properties) {
              if (properties.hasOwnProperty(key)) {
                $scope.tableFields.push(key);
              }
            }
          } else {
            // form is mapped
            data.metadata.form.forEach(function(element) {
              $scope.tableFields.push(element.key);
            })
          }

          // resetting
          $scope.dcps = [];
          $scope.$broadcast("resetStoragerDataSets");

          $scope.model = {};
          $scope.form = data.metadata.form;
          $scope.schema = {
            type: 'object',
            properties: data.metadata.schema.properties,
            required: data.metadata.schema.required
          };
          $scope.$broadcast('schemaFormRedraw');
        }).error(function(err) {
          console.log("Error in semantics change: ", err);
          $scope.model = {};
          $scope.form = [];
          $scope.schema = {};
          $scope.$broadcast('schemaFormRedraw');
        })
      };

      $scope.onDataProviderClick = function(index) {
        var url = $window.location.pathname + "&type=" + params.state;
        var semanticsName = $scope.dataSeries["semantics"].name || "";
        var output = Object.assign({}, $scope.dataSeries);
        output.semantics = semanticsName;
        output.parametersData = $scope.parametersData;

        $window.location.href = "/configuration/providers/new?redirectTo=" + url + "&" + $httpParamSerializer(output);
      };

      $scope.removePcd = function(dcpItem) {
        $scope.dcps.forEach(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (dcp.mask === dcpItem.mask) {
            var data = Object.assign({}, dcpItem);
            data.id = pcdIndex + 1;
            $scope.$broadcast("dcpOperation", {action: "remove", dcp: data});
            array.splice(pcdIndex, 1);
          }
        });
      };

      var isValidParametersForm = function(form) {
        $scope.$broadcast('schemaFormValidate');

        return form.$valid;
      };

      $scope.addDcp = function() {
        if (isValidParametersForm(this.parametersForm)) {
          $scope.dcps.push(Object.assign({}, $scope.model));
          var data = Object.assign({}, $scope.model);
          data.id = $scope.dcps.length;
          $scope.$broadcast("dcpOperation", {action: "add", dcp: data});
          $scope.model = {};

          // reset form to do not display feedback class
          this.parametersForm.$setPristine();
        }
      };

      $scope.changeDataProvider = function() {
        console.log($scope.dataSeries);
      };

      var errorHelper = function(form) {
        angular.forEach(form.$error, function (field) {
          angular.forEach(field, function(errorField){
            errorField.$setDirty();
          })
        });
      };

      $scope.save = function() {
        var generalDataForm = angular.element('form[name="generalDataForm"]').scope().generalDataForm;
        if(generalDataForm.$invalid) {
          errorHelper(generalDataForm);
          return;
        }
        // checking parameters form (semantics) is invalid
        var parametersForm = angular.element('form[name="parametersForm"]').scope().parametersForm;
        if ($scope.dcps.length === 0 && !isValidParametersForm(parametersForm)) {
          errorHelper(parametersForm);
          return;
        }

        var scheduleForm = angular.element('form[name="scheduleForm"]').scope().scheduleForm;
        if (scheduleForm.$invalid) {
          errorHelper(scheduleForm);
          return;
        }

        $scope.alertBox.title = "Data Series Registration";

        // it prepares dataseries object, schedule and filter object
        var _save = function() {

          var dataToSend = Object.assign({}, $scope.dataSeries);
          dataToSend.data_series_semantic_name = $scope.dataSeries.semantics.name;

          var semantics = dataToSend.semantics;
          delete dataToSend.semantics;

          dataToSend.dataSets = [];

          $scope.errorFound = false;

          switch(semantics.data_series_type_name.toLowerCase()) {
            case "dcp":
            case "pcd":
              $scope.dcps.forEach(function(dcp) {
                var format = {};
                for(var key in dcp) {
                  if (dcp.hasOwnProperty(key))
                    if (key !== "latitude" && key !== "longitude" && key !== "active")
                      format[key] = dcp[key];
                }

                var dataSetStructure = {
                  active: $scope.dataSeries.active,
                  format: format,
                  position: {
                    type: 'Point',
                    coordinates: [dcp.latitude, dcp.longitude],
                    crs: {
                      type: 'name',
                      properties : {
                        name: "EPSG:" + dcp.projection
                      }
                    }
                  }
                };

                dataToSend.dataSets.push(dataSetStructure);
              });

              break;

            case "occurrence":
              var format = $scope.model;

              var dataSet = {
                semantics: semantics,
                active: $scope.dataSeries.active,
                format: format
              };

              dataToSend.dataSets.push(dataSet);
              break;

            case "grid":
              break;

            default:
              break;
          }

          // adjusting time without timezone
          var filterValues = Object.assign({}, $scope.filter);
          if (filterValues.area) {
            filterValues.area.afterDate = new Date(filterValues.area.afterDate.getTime() - filterValues.area.afterDate.getTimezoneOffset()).toString();
            filterValues.area.beforeDate = new Date(filterValues.area.beforeDate.getTime() - filterValues.area.beforeDate.getTimezoneOffset()).toString();
          }

          var scheduleValues = Object.assign({}, $scope.schedule);
          switch(scheduleValues.scheduleHandler) {
            case "seconds":
            case "minutes":
            case "hours":
              scheduleValues.frequency_unit = scheduleValues.scheduleHandler;
              break;

            case "weekly":
            case "monthly":
            case "yearly":
              // todo: verify
              scheduleValues.schedule_unit = scheduleValues.scheduleHandler;
              break;

            default:
              break;
          }

          console.log(dataToSend);

          return {
            dataSeries: dataToSend,
            schedule: scheduleValues,
            filter: filterValues
          };
        };

        // it dispatches post operation to nodejs
        var _sendRequest = function(object) {
        // var _sendRequest = function(dataToSend, scheduleValues, filterValues, serviceOutput) { 
          DataSeriesFactory.post({
            dataSeries: object.dataToSend,
            schedule: object.scheduleValues,
            filter: object.filterValues,
            service: object.serviceOutput
          }).success(function(data) {
            console.log(data);
            $window.location.href = "/configuration/dynamic/dataseries";
          }).error(function(err) {
            $scope.alertLevel = "alert-danger";
            $scope.alertBox.message = err.message;
            $scope.display = true;
            $scope.extraProperties = {};
            console.log(err);
          });
        };

        if ($scope.dataSeries.access == 'COLLECT') {
          $scope.$on("storageValuesReceive", function(event, values) {
          //  todo: improve

            var dataObject = _save();
            var dSeriesName = dataObject.dataSeries.name;
            // setting _input in data series
            dataObject.dataSeries.name += "_input";
            
            var dSets = values.data;

            // it makes data set format
            var _makeFormat = function(dSetObject) {
              var format_ = {};
              for(var key in dSetObject) {
                if (dSetObject.hasOwnProperty(key) && key.toLowerCase() !== "id")
                  format_[key] = dSetObject[key];
              }
              return format_;
            };

            var out;
            if (dSets instanceof Object) {
              dSets.format = _makeFormat(dSets);
              dSets.active = $scope.dataSeries.active;
              out = [dSets];
            } else {
              // setting to active
              dSets.forEach(function(dSet) {
                dSet.format = _makeFormat(dSet);
                dSet.active = $scope.dataSeries.active;
              });
              out = dSets;
            }
            
            var outputDataSeries = {
              name: dSeriesName,
              description: dataObject.dataSeries.description,
              data_series_semantic_name: values.semantics.name,//$scope.dataSeries.semantics.name,
              data_provider_id: values.data_provider,
              dataSets: out
            };

            _sendRequest({
              dataToSend: {input: dataObject.dataSeries, output: outputDataSeries},
              scheduleValues: dataObject.schedule,
              filterValues: dataObject.filter,
              serviceOutput: values.service
            });

          });
          // getting values from another controller
          $scope.$broadcast("requestStorageValues");
        } else {
          // processing
          var dataObject = _save();

          //  display alert box
          $scope.alertLevel = "alert-warning";
          $scope.alertBox.title = "Data Series";
          $scope.alertBox.message = "Note: Tha data will be acquired when it has been accessed";
          $scope.display = true;
          $scope.extraProperties.object = {
            dataToSend: dataObject.dataSeries,
            scheduleValues: dataObject.schedule,
            filterValues: dataObject.filter
          };
          $scope.extraProperties.confirmButtonFn = _sendRequest;

          // _sendRequest(dataObject.dataSeries, dataObject.schedule, dataObject.filter);
        }

      };
    }
  ]);