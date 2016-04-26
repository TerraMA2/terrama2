angular.module('terrama2.dataseries.registration', [
    'terrama2',
    'terrama2.projection',
    'terrama2.services',
    'terrama2.components.messagebox', // handling alert box
    'ui.router',
    'mgo-angular-wizard', // wizard
    'ui.bootstrap.datetimepicker',
    'ui.dateTimeInput',
    'schemaForm'
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

      // schedule
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
        // resetting
        if (value == 1) {
          delete $scope.schedule.schedule;
          delete $scope.schedule.schedule_retry;
          delete $scope.schedule.schedule_retry_unit;
          delete $scope.schedule.schedule_timeout;
          delete $scope.schedule.schedule_timeout_unit;
          $scope.isFrequency = true;
          $scope.isSchedule = false;
        } else if (value == 2) {
          delete $scope.frequency;
          delete $scope.frequency_unit;
          $scope.isFrequency = false;
          $scope.isSchedule = true;
        }
      };
      
      // Wizard validations
      $scope.isFirstStepValid = function(obj) {
        return isWizardStepValid("generalDataForm");
      };
      
      $scope.isSecondStepValid = function(obj) {
        if ($scope.dataSeries.semantics.data_series_type_name === "Dcp")
          if ($scope.dcps.length === 0) {
            // todo: display alert box
            console.log("it should have at least one dcp");
            return isWizardStepValid("parametersForm", true) && false;
          } else {
            return true;
          }
        return isWizardStepValid("parametersForm", true);
      };

      $scope.isThirdStepValid = function(obj) {
        return isWizardStepValid("storagerForm");
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
        $scope.semantics = $scope.dataSeries.semantics.data_format_name.toLowerCase();
        
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
            array.splice(pcdIndex, 1);
            return;
          }
        });
      };

      var isValidParametersForm = function(form) {
        $scope.$broadcast('schemaFormValidate');

        return form.$valid;
      };
      
      $scope.addDcp = function(aaaa) {
        if (isValidParametersForm(this.parametersForm)) {
          $scope.dcps.push(Object.assign({}, $scope.model));
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

        var dataToSend = Object.assign({}, $scope.dataSeries);
        dataToSend.data_series_semantic_name = $scope.dataSeries.semantics.name;

        var semantics = dataToSend.semantics;
        delete dataToSend.semantics;

        dataToSend.dataSets = [];

        $scope.alertBox.title = "Data Series Registration";
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
                      name: dcp.projection
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
    }
  ]);