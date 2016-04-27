angular.module('terrama2.dataseries.registration', [
    'terrama2',
    'terrama2.projection',
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

  .directive('terrama2DcpTable', function(i18n) {
    return {
      restrict: 'E',
      templateUrl: '/javascripts/angular/data-series/templates/dcpTable.html',
      scope: {
        tableFields: '=tableFields',
        dcps: '=dcps',
        restrictionDisplay: '=restrictionDisplay'
      },
      link: function(scope, elem, attr) {
        var element = elem[0];
        var original = element.cloneNode;
        element.cloneNode = patch;

        function patch(deep) {
          var clone = original.call(element, deep);

          // You can remove this two lines and the result
          //   will be more or less the same.
          // In my case I need it for other reasons
          clone.removeAttribute('mq-allow-external-clone');
          clone.cloneNode = patch;

          $compile(clone)(scope);
          return clone;
        }
      },
      controller: function($scope, i18n) {
        $scope.i18n = i18n;
        $scope.isBoolean = function(value) {
          return typeof value === 'boolean';
        };

        $scope.removeDcp = function(dcpItem) {
          $scope.dcps.forEach(function(dcp, pcdIndex, array) {
            // todo: which fields should compare to remove?
            if (dcp.mask === dcpItem.mask) {
              array.splice(pcdIndex, 1);
              return;
            }
          });
        }
      }
    };
  })

  .controller('StoragerController', ['$scope', 'i18n', 'DataSeriesSemanticsFactory', function($scope, i18n, DataSeriesSemanticsFactory) {
    $scope.form = [];
    $scope.model = {};
    $scope.schema = {};
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
        if (dcp.inputDataSet === dcpItem.inputDataSet) {
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
      if (form.$valid) {
        // checking if it is a dcp
        switch ($scope.formatSelected.data_series_type_name) {
          case "Dcp":
            $scope.$emit("storageValuesReceive", {
              data: $scope.dcpsStorager,
              type: $scope.formatSelected.data_series_type_name
            });
            break;
          case "Occurrence":
            $scope.$emit("storageValuesReceive", {
              data: $scope.model,
              type: $scope.formatSelected.data_series_type_name
            });
            break;
          default:
            $scope.$emit("storageValuesReceive", {data: null, type: null});
            break;
        }
      }
    });

    $scope.$on("dcpOperation", function(event, args) {
      if (args.action === "remove") {
        $scope.removePcdStorager(args.dcp);
      //  todo: remove it from list
        removeInput(args.dcp.mask);
      } else if (args.action === "add") {
        $scope.dcpsStorager.push({tableName: args.dcp.mask, inputDataSet: args.dcp.mask});
        // $scope.inputDataSets.push({tableName: args.dcp.mask, inputDataSet: args.dcp.mask});
      }
    });

    $scope.$on('storagerFormatChange', function(event, args) {
      $scope.formatSelected = args.format;
      // todo: fix it. It is hard code
      $scope.tableFieldsStorager.push("tableName");
      $scope.tableFieldsStorager.push("inputDataSet");
      // args.dcps.forEach(function(dcp) {
      //   $scope.dcpsStorager.push({tableName: dcp.mask, inputDataSet: dcp.mask});
      // });

      // DataSeriesSemanticsFactory.get(args.format.name, {metadata:true}).success(function(data) {
      //   $scope.tableFieldsStorager = [];
      //   var form = data.metadata.form;
      //   // building table fields. Check if form is for all ('*')
      //   if (data.metadata.form.indexOf('*') != -1) {
      //     // ignore form and make it from properties
      //     var properties = data.metadata.schema.properties;
      //     for(var key in properties) {
      //       if (properties.hasOwnProperty(key)) {
      //         $scope.tableFieldsStorager.push(key);
      //       }
      //     }
      //   } else {
      //     // form.push({key: 'InputDataSet', htmlClass: "col-md-2"});
      //     // form is mapped
      //     data.metadata.form.forEach(function(element) {
      //       $scope.tableFieldsStorager.push(element.key);
      //     })
      //   }
      //
      //   $scope.tableFieldsStorager.push("inputDataSet");
      //
      //   var schema = data.metadata.schema.properties;
      //
      //   $scope.model = {};
      //   // $scope.form = data.metadata.form;
      //   // $scope.schema = {
      //   //   type: 'object',
      //   //   properties: schema,
      //   //   required: data.metadata.schema.required
      //   // };
      //   // $scope.$broadcast('schemaFormRedraw');
      // }).error(function(err) {
      //   console.log("Error in semantics change: ", err);
      //   $scope.form = [];
      //   $scope.model = {};
      //   $scope.schema = {};
      //   $scope.$broadcast('schemaFormRedraw');
      // })
    });

  }])

  .controller('RegisterDataSeries', ['$scope', '$http', 'i18n', "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsFactory", "DataProviderFactory", "DataSeriesFactory", "ServiceInstanceFactory",
    function($scope, $http, i18n, $window, $state, $httpParamSerializer, DataSeriesSemanticsFactory, DataProviderFactory, DataSeriesFactory, ServiceInstanceFactory) {
      // definition of schema form
      $scope.schema = {};
      $scope.form = [];
      $scope.model = {};

      $scope.service = {};

      // terrama2 messagebox
      $scope.errorFound = false;
      $scope.alertBox = {};
      $scope.resetState = function() {
        $scope.errorFound = false;
        $scope.alertBox.message = "";
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
      $scope.storager = {};
      $scope.formStorager = [];
      $scope.modelStorager = {};
      $scope.schemaStorager = {};
      $scope.onStoragerFormatChange = function() {
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
        this.wzData.error = !isWizardStepValid("generalDataForm");
        return true;
      };

      $scope.isSecondStepValid = function(obj) {
        if ($scope.dataSeries.semantics.data_series_type_name === "Dcp")
          if ($scope.dcps.length === 0) {
            // todo: display alert box
            console.log("it should have at least one dcp");
            this.wzData.error = true;
            return true;
          } else {
            this.wzData.error = false;
            return true;
          }
        this.wzData.error = !isWizardStepValid("parametersForm", true);
        return true;
      };

      $scope.isThirdStepValid = function(obj) {
        this.wzData.error = !isWizardStepValid("storagerForm");
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
              return;
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

      // it defines when data change combobox has changed and it will adapt the interface
      $scope.onDataSemanticsChange = function() {
        $scope.semantics = $scope.dataSeries.semantics.data_series_type_name.toLowerCase();

        // if is dcp postgis, it shouldn't have a storager and it is processing
        if ($scope.dataSeries.semantics.data_series_type_name == 'Dcp' && $scope.dataSeries.semantics.name != 'DCP-POSTGIS') {
          $scope.dataSeries.access = 'COLLECT';

          $scope.storagerFormats = [{name: 'DCP-POSTGIS'}];
        } else {
          $scope.dataSeries.access = 'PROCESSING';
          $scope.storagerFormats = [];
        }

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
        var semanticsName = $scope.dataSeries.semantics.name || "";
        var output = Object.assign({}, $scope.dataSeries);
        output.semantics = semanticsName;
        output.parametersData = $scope.parametersData;

        $window.location.href = "/configuration/providers/new?redirectTo=" + url + "&" + $httpParamSerializer(output);
      };

      $scope.removePcd = function(dcpItem) {
        $scope.dcps.forEach(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (dcp.mask === dcpItem.mask) {
            $scope.$broadcast("dcpOperation", {action: "remove", dcp: Object.assign({}, dcpItem)});
            array.splice(pcdIndex, 1);
            return;
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
          $scope.$broadcast("dcpOperation", {action: "add", dcp: Object.assign({}, $scope.model)});
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

        var _makeDataSets = function(dataSetList, data_series_semantics) {
          var dataSets = [];

          switch(data_series_semantics.toLowerCase()) {
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
                  active: dcp.active,
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
                active: $scope.parametersData.active,
                format: format
              };

              dataToSend.dataSets.push(dataSet);
              break;

            case "grid":
              break;

            default:
              break;
          }

          return dataSets;
        };

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
                  active: dcp.active,
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
                active: $scope.parametersData.active,
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
          if (scheduleValues.schedule) {
            scheduleValues.schedule = new Date(scheduleValues.schedule.getTime() - scheduleValues.schedule.getTimezoneOffset()).toString();
          }

          console.log(dataToSend);
          DataSeriesFactory.post({
            dataSeries: dataToSend,
            schedule: scheduleValues,
            filter: filterValues,
            service: $scope.service
          }).success(function(data) {
            console.log(data);
            $window.location.href = "/configuration/dynamic/dataseries";
          }).error(function(err) {
            $scope.alertBox.message = err.message;
            $scope.errorFound = true;
            console.log(err);
            alert("Error found");
          });
        };

        if ($scope.dataSeries.access == 'COLLECT') {
          $scope.$on("storageValuesReceive", function(event, values) {
            _save();
          });
          // getting values from another controller
          $scope.$broadcast("requestStorageValues");
        } else {
          // processing
          _save();
        }

      };
    }
  ]);