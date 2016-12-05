angular.module('terrama2.dataseries.registration', [
    'terrama2',
    'terrama2.services',
    'terrama2.dataseries.services',
    'terrama2.components.messagebox', // handling alert box
    'ui.router',
    'mgo-angular-wizard', // wizard
    'schemaForm',
    'xeditable',
    'terrama2.schedule',
    'terrama2.datetimepicker',
    'terrama2.components.geo',
    'treeControl',
  ])
  .config(["$stateProvider", "$urlRouterProvider", function($stateProvider, $urlRouterProvider) {
    $stateProvider.state('main', {
      abstract: true,
      template: '<div ui-view=""></div>',
      resolve: {
        "i18nData": ["i18n", function (i18n) {
          return i18n.ensureLocaleIsLoaded();
        }]
      },
      controller: 'RegisterDataSeries'
    });

    $urlRouterProvider.otherwise('wizard');

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

  .controller('StoragerController', [
    '$scope', 'i18n', 'DataSeriesSemanticsFactory', 'UniqueNumber', 'Polygon', 'DateParser', 'SemanticsParserFactory',
    function($scope, i18n, DataSeriesSemanticsFactory, UniqueNumber, Polygon, DateParser, SemanticsParserFactory) {
      $scope.formStorager = [];
      $scope.modelStorager = {};
      $scope.schemaStorager = {};
      $scope.tableFieldsStorager = [];
      $scope.formatSelected = {};
      $scope.dcpsStorager = [];
      $scope.inputDataSets = [];
      $scope.storage = {};
      $scope.dataProvidersStorager = [];

      var removeInput = function(dcpMask) {
        $scope.inputDataSets.some(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (dcp.mask === dcpMask) {
            array.splice(pcdIndex, 1);
            return true;
          }
        });
      };

      $scope.removePcdStorager = function(dcpItem) {
        $scope.dcpsStorager.some(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (dcp._id === dcpItem._id) {
            array.splice(pcdIndex, 1);
            return true;
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

      $scope.$on("requestStorageValues", function() {
        // apply a validation
        $scope.$broadcast('schemaFormValidate');

        if ($scope.forms.storagerForm.$valid && $scope.forms.storagerDataForm.$valid) {
          // checking if it is a dcp
          switch ($scope.formatSelected.data_series_type_name) {
            case "DCP":
              $scope.$emit("storageValuesReceive", {
                data: $scope.dcpsStorager,
                data_provider: $scope['storager_data_provider_id'],
                service: $scope["storager_service"],
                type: $scope.formatSelected.data_series_type_name,
                semantics: $scope.formatSelected
              });
              break;
            case "GRID":
            case "OCCURRENCE":
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
          angular.forEach($scope.forms.storagerDataForm.$error, function (field) {
            angular.forEach(field, function(errorField){
              errorField.$setDirty();
            });
          });
        }
      });

      $scope.$on("dcpOperation", function(event, args) {
        if (args.action === "remove") {
          $scope.removePcdStorager(args.dcp);
        //  todo: remove it from list
          removeInput(args.dcp.mask);
        } else if (args.action === "add") {
          if ($scope.storager.format && $scope.storager.format.data_format_name === globals.enums.DataSeriesFormat.POSTGIS) {
            // postgis
            var copyFormat = angular.merge({}, $scope.dataSeries.semantics.metadata.metadata);
            angular.merge(copyFormat, args.dcp);
            var obj = SemanticsParserFactory.parseKeys(copyFormat);
            obj.table_name = obj.mask;
            $scope.dcpsStorager.push(obj);
          } else {
            $scope.dcpsStorager.push(args.dcp);
          }
        }
      });

      $scope.$on("resetStoragerDataSets", function(event) {
        $scope.dcpsStorager = [];
      });

      $scope.$on("clearStoreForm", function(event){
          $scope.modelStorager = {};
          $scope.formStorager = [];
          $scope.schemaStorager = {};
          $scope.storager.format = null;
          $scope.storager_service = undefined;
          $scope.dcpsStorager = [];
          $scope.storager_data_provider_id = undefined;
          $scope.$broadcast("clearSchedule");
      });

      $scope.$on('storagerFormatChange', function(event, args) {
        $scope.formatSelected = args.format;
        // todo: fix it. It is hard code
        $scope.tableFieldsStorager = [];

        var queryParams = {
          metadata: true
        };

        if ($scope.isDynamic) {
          queryParams['type'] = "dynamic";
        } else {
          queryParams['type'] = "static";
        }

        DataSeriesSemanticsFactory.get(args.format.code, queryParams).success(function(data) {
          $scope.dataProvidersStorager = [];
          $scope.dcpsStorager = [];
          $scope.dataProvidersList.forEach(function(dataProvider) {
            data.data_providers_semantics.forEach(function(demand) {
              if (dataProvider.data_provider_type.id == demand.data_provider_type_id){
                if ($scope.storager.format.data_series_type_name == 'GRID' && dataProvider.data_provider_type.id != 1 )
                  return;
                $scope.dataProvidersStorager.push(dataProvider);
              }
            })
          });

          if ($scope.dataProvidersStorager.length > 0)
            $scope.storager_data_provider_id = $scope.dataProvidersStorager[0].id;

          var metadata = data.metadata;
          var properties = metadata.schema.properties;

          if ($scope.isUpdating) {
            if ($scope.formatSelected.data_series_type_name === globals.enums.DataSeriesType.DCP) {
              // todo:
            } else {
              $scope.modelStorager = $scope.prepareFormatToForm(configuration.dataSeries.output.dataSets[0].format);
            }
          } else {
            var copyFormat = angular.merge({}, $scope.dataSeries.semantics.metadata.metadata);
            angular.merge(copyFormat, $scope.model);
            $scope.modelStorager = SemanticsParserFactory.parseKeys(copyFormat);
            $scope.filter.area = {
              srid: 4326
            };
          }

          var outputDataseries = configuration.dataSeries.output;

          if ($scope.hasCollector) {
            var collector = configuration.collector;
            $scope.storager_service = collector.service_instance_id;
            $scope.storager_data_provider_id = outputDataseries.data_provider_id;

            // fill schedule
            var schedule = collector.schedule;
            $scope.$broadcast("updateSchedule", schedule);

            // fill filter
            var filter = collector.filter || {};

            if (filter.discard_before || filter.discard_after || filter.region){
              $scope.advanced.filter.disabled = false;
              $scope.wizard.filter.disabled = false;
              $scope.wizard.filter.error = false;
            }

            if (filter.discard_before) {
              $scope.filter.date.beforeDate = DateParser(filter.discard_before);
            }
            if (filter.discard_after) {
              $scope.filter.date.afterDate = DateParser(filter.discard_after);
            }

            // filter geometry field
            if (filter.region) {
              $scope.$emit('updateFilterArea', "2");
              $scope.filter.area = Polygon.read(filter.region);
              if (filter.crop_raster){
                $scope.filter.area.crop_raster = true;
              }
            }
            $scope.filter.area.showCrop = $scope.dataSeries.semantics.data_series_type_name == "GRID";
          }

          if ($scope.formatSelected.data_series_type_name === globals.enums.DataSeriesType.DCP) {
            Object.keys(properties).forEach(function(key) {
              $scope.tableFieldsStorager.push(key);
            });

            if ($scope.hasCollector) {
              outputDataseries.dataSets.forEach(function(dataset) {
                $scope.dcpsStorager.push(dataset.format);
              });
            } else {
              (args.dcps || []).forEach(function(dataSetDcp) {
                $scope._addDcpStorager(Object.assign({}, dataSetDcp));
              });
            }

            $scope.modelStorager = {};
            $scope.formStorager = [];
            $scope.schemaStorager = {};
            $scope.$broadcast('schemaFormRedraw');
          } else {
          //   occurrence
            $scope.formStorager = metadata.form;
            $scope.schemaStorager = {
              type: 'object',
              properties: metadata.schema.properties,
              required: metadata.schema.required
            };
            $scope.$broadcast('schemaFormRedraw');

            if (!outputDataseries)
              return;

            // fill out default
            if ($scope.formatSelected.data_series_type_name != globals.enums.DataSeriesType.DCP) {
              $scope.modelStorager = $scope.prepareFormatToForm(outputDataseries.dataSets[0].format);
              if(typeof $scope.modelStorager.timezone === "number") {
                $scope.modelStorager.timezone = $scope.modelStorager.timezone.toString();
              }
            }
          }

        }).error(function(err) {

        });
      });
    }
  ])

  .controller('RegisterDataSeries', [
    '$scope',
    '$http',
    'i18n',
    "$window",
    "$state",
    "$httpParamSerializer",
    "DataSeriesSemanticsFactory",
    "DataProviderFactory",
    "DataSeriesFactory",
    "ServiceInstanceFactory",
    "$timeout",
    'FormHelper',
    "WizardHandler",
    'UniqueNumber',
    "Polygon",
    "FilterForm",
    "$q",
    "Point",
    function($scope, $http, i18n, $window, $state, $httpParamSerializer,
             DataSeriesSemanticsFactory, DataProviderFactory, DataSeriesFactory,
             ServiceInstanceFactory, $timeout, FormHelper, WizardHandler, UniqueNumber, Polygon, FilterForm, $q, Point) {
      // definition of schema form
      $scope.schema = {};
      $scope.form = [];
      $scope.model = {};

      // defining box
      $scope.cssBoxSolid = {
        boxType: "box-solid"
      };

      // consts
      $scope.filterTypes = {
        NO_FILTER: {
          name: i18n.__("Do not filter"),
          value: "1"
        },
        AREA: {
          name: i18n.__("Filter by limits"),
          value: "2"
        }
      };

      // clear optional forms
      var clearStoreForm = function(){
        $scope.showStoragerForm = false;
        $scope.schedule = {};
        $scope.scheduleOptions = {};
        $scope.advanced.store.disabled = true;
        $scope.$broadcast('clearStoreForm');
        var enableStore = angular.element('#store-collapse');
        var storebox = angular.element('#store-box');
        if (!storebox.hasClass('collapsed-box')){
          enableStore.click();
        }
        //disable option to crop on filter
        $scope.filter.area.showCrop = false;
        $scope.filter.area.crop_raster = false;
      }

      var clearFilterForm = function(){
        $scope.filter.date = {};
        $scope.filter.filterArea = "1";
        $scope.advanced.filter.disabled = true;
        var enableFilter = angular.element('#filter-collapse');
        var filterbox = angular.element('#filter-box');
        if (!filterbox.hasClass('collapsed-box')){
          enableFilter.click();
        }
      }

      var clearIntersectionForm = function(){
        for (var key in $scope.intersection) {
          $scope.removeDataSeries(key);
        }
        $scope.advanced.intersection.disabled = true;
        var enableIntersection = angular.element('#intersection-collapse');
        var intersectionbox = angular.element('#intersection-box');
        if (!intersectionbox.hasClass('collapsed-box')){
          enableIntersection.click();
        }
      }

      // open optional form in advanced mode
      var openStoreForm = function(){
        $scope.advanced.store.disabled = false;
        var enableStore = angular.element('#store-collapse');
        // set disabled to false, to open form
        enableStore.attr("disabled", false);
        enableStore.click();
      }

      var openFilterForm = function(){
        $scope.advanced.filter.disabled = false;
        var enableFilter = angular.element('#filter-collapse');
        // set disabled to false, to open form
        enableFilter.attr("disabled", false);
        enableFilter.click();
      }

      var openIntersectionForm = function(){
        $scope.advanced.intersection.disabled = false;
        var enableIntersection = angular.element('#intersection-collapse');
        // set disabled to false, to open form
        enableIntersection.attr("disabled", false);
        enableIntersection.click();
      }

      // advanced global properties
      $scope.advanced = {
        store: {
          disabled: true,
          clearForm: clearStoreForm,
          openForm: openStoreForm,
          optional: true
        },
        filter: {
          disabled: true,
          clearForm: clearFilterForm,
          openForm: openFilterForm,
          optional: true
        },
        intersection: {
          disabled: true,
          clearForm: clearIntersectionForm,
          openForm: openIntersectionForm,
          optional: true
        }
      }

      // wizard global properties
      $scope.wizard = {
        general: {
          required: true,
          formName: 'generalDataForm'
        },
        parameters: {
          required: true,
          formName: 'parametersForm',
          disabled: true
        },
        store: {
          required: false,
          formName: 'storagerForm',
          secondForm: 'storagerDataForm',
          disabled: true,
          optional: true,
          clearForm: clearStoreForm
        },
        filter: {
          required: false,
          formName: 'filterForm',
          disabled: true,
          optional: true,
          clearForm: clearFilterForm
        },
        intersection: {
          required: false,
          formName: 'intersectionForm',
          disabled: true,
          optional: true,
          clearForm: clearIntersectionForm
        }
      };

      $scope.forms = {};
      $scope.isDynamic = configuration.dataSeriesType === "dynamic";

      $scope.service = {};

      $scope.alertLevel = "";

      // filter values
      $scope.filter = {date: {}, area: {srid: 4326, showCrop: false}};
      $scope.radioPreAnalysis = {};
      $scope.handlePreAnalysisFilter = function(selected) {
        $scope.filter.pre_analysis = {};
        $scope.radioPreAnalysis = selected;
      };

      $scope.filter.filterArea = $scope.filterTypes.NO_FILTER.value;
      $scope.$on('updateFilterArea', function(event, filterValue) {
        $scope.filter.filterArea = filterValue;
      });

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

      // data series used for intersection
      $scope.dataSeriesList = [];

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

      $scope.prepareFormatToForm = function(fmt) {
        var output = {};
        for(var k in fmt) {
          if (fmt.hasOwnProperty(k)) {
            // checking if a number
            if (isNaN(fmt[k]) || typeof fmt[k] == "boolean") {
              output[k] = fmt[k];
            } else {
              output[k] = parseInt(fmt[k]);
            }
          }
        }
        return output;
      };

      $scope.tryParseInt = function(value) {
        if (isNaN(value))
          return value;
        return parseInt(value);
      };


      // wizard helper
      var isWizardStepValid = function() {
        $scope.$broadcast('formFieldValidation');
        $scope.$broadcast('schemaFormValidate');
        var w = WizardHandler.wizard();

        var response = false;
        w.getEnabledSteps().forEach(function(wizardStep) {
          var data = wizardStep.wzData || {};
          var name = data.formName || "";
          var disabled = data.disabled;

          if (disabled){
            delete wizardStep.wzData.error;
            return;
          }

          //validating filter form
          if (name === 'filterForm') {
            if (FilterForm.boundedForm){
              var condition = FilterForm.boundedForm.$invalid;
              wizardStep.wzData.error = condition;
            }
            else {
              delete wizardStep.wzData.error;
            }
            return;
          }

          // validating store when form is enabled
          if (name == 'storagerForm' && !$scope.storager.format){
            wizardStep.wzData.error = true;
            return;
          }

          var condition = $scope.forms[name].$invalid;
          var secondName = wizardStep.wzData.secondForm;

          if (secondName)
            condition = condition || $scope.forms[secondName].$invalid;

          if (name === "parametersForm" && $scope.dcps.length > 0) {
            // reset form to initial state
            $scope.forms[name].$setPristine();
            condition = false;
          }
          wizardStep.wzData.error = condition;

        });
      };

      // intersection
      // components: data series tree modal
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
        {name: "Static", children: []}
        //Remove comment when its possible to do intersection with dynamic data - change to Dynamic
        //{name: "Grid", children: []}
      ];

      // adding data series in intersection list
      $scope.addDataSeries = function(ds) {
        var _helper = function(index, target) {
          $scope.dataSeriesGroups[index].children.some(function(element, indexArr, arr) {
            if (element.id === target.id) {
              arr.splice(indexArr, 1);
              return true;
            }
            return false;
          });
        };

        var _handleList = function(ds) {
          $scope.intersection[ds.id] = $scope.intersection[ds.id] || {
            data_series: ds,
            attributes: [],
            selected: true
          };

          if (ds.data_series_semantics.data_series_type_name === globals.enums.DataSeriesType.GRID) {
            ds.isGrid = true;
            _helper(1, ds);
          } else {
            ds.isGrid = false;
            _helper(0, ds);
          }
        };

        if (ds) {
          _handleList(ds);
          return;
        }

        $scope.nodesDataSeries.forEach(function(target) {
          if (!target || !target.id)
            return;

          _handleList(target);
        });

        $scope.nodesDataSeries = [];
      };

      // removing data series from intersection list
      $scope.removeDataSeries = function(dataSeriesId) {
        if (!dataSeriesId) { return; }

        var dataSeries = $scope.intersection[dataSeriesId].data_series;

        var _helper = function(target) {
          var newList = [dataSeries];
          target.forEach(function(el) {
            newList.push(el);
          });
          return newList;
        };

        var dataSeriesType = dataSeries.data_series_semantics.data_series_type_name;
        if (dataSeriesType === globals.enums.DataSeriesType.GRID) {
          $scope.dataSeriesGroups[1].children = _helper($scope.dataSeriesGroups[1].children);
        } else {
          $scope.dataSeriesGroups[0].children = _helper($scope.dataSeriesGroups[0].children);
        }

        // removing ds attributes
        delete $scope.intersection[dataSeries.id];
      };

      // syntax: {data_series_id: {data_series: DataSeries, attributes: []}}
      $scope.intersection = {};
      // Selected nodes in modal
      $scope.nodesDataSeries = [];
      $scope.intersectionDataSeriesList = [];
      $scope.selectedIntersection = null;

      $scope.onIntersectionDataSeriesClick = function(dataSeries) {
        $scope.selectedIntersection = dataSeries;
        if (!$scope.intersection[dataSeries.id])
          $scope.intersection[dataSeries.id] = {};

        if (!dataSeries.isGrid){
          var dataProvider = $scope.dataProvidersList.filter(function(element) {
            return element.id == dataSeries.data_provider_id;
          });
          if (dataProvider.length > 0 && dataProvider[0].data_provider_type.id == 4){
            var table_name = dataSeries.dataSets[0].format.table_name;
            listColumns(dataProvider[0], table_name);
          }
        }

        var intersection = $scope.intersection[dataSeries.id];
        if (!intersection.attributes) {
          intersection.data_series = dataSeries;
          intersection.attributes = [];
          intersection.selected = false;
        }

        $scope.forms.intersectionForm.$setPristine();
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
          $scope.columnsList = data.data.map(function(item, index){
            return item.column_name;
          });
          result.resolve(data);
        });

        httpRequest.error(function(err) {
          result.reject(err);
        });

        return result.promise;

      }

      $scope.isIntersectionEmpty = function() {
        return Object.keys($scope.intersection).length === 0;
      };

      var canAddAttribute = function(selected, attributeValue, attributes) {
        if (!selected || !attributeValue)
          return;

        var attrs = $scope.intersection[selected.id].attributes;

        var found = attrs.filter(function(elm) {
          return elm === attributeValue;
        });

        return found.length === 0;
      };

      $scope.addAttribute = function(form, selected, attributeValue) {
        if (form.$invalid) {
          FormHelper(form);
          return;
        }

        if (canAddAttribute(selected, attributeValue, $scope.intersection[selected.id].attributes)) {
          // reset form to the default state
          $scope.intersection[selected.id].attributes.push(attributeValue);
          form.$setPristine();
        } else {
          // TODO: throw error message

        }
      };

      $scope.removeAttribute = function(selected, attributeValue) {
        var intersection = $scope.intersection[selected.id];

        intersection.attributes.some(function(attr, index, arr) {
          if (attr === attributeValue) {
            arr.splice(index, 1);
            return true;
          }
        });

        if (intersection.attributes.length === 0) {
          $scope.intersection[selected.id].selected = true;
        }

        // delete $scope.intersection[selected.id];
      };

      $scope.onIntersectionCheck = function(dataSeries, boolFlag) {
        if (!boolFlag) {
          $scope.selectedIntersection = null;
          delete $scope.intersection[dataSeries.id];
          return;
        }

        // emit row click
        $scope.onIntersectionDataSeriesClick(dataSeries);
      };

      // filters
      $scope.intersectionDataSeries = function(dataSeries) {
        return (dataSeries.data_series_semantics.data_series_type_name == globals.enums.DataSeriesType.GEOMETRIC_OBJECT ||
                dataSeries.data_series_semantics.data_series_type_name == globals.enums.DataSeriesType.GRID);
      };

      $scope.onFilterRegion = function() {
        if ($scope.filter.filterArea === $scope.filterTypes.NO_FILTER.value) {
          $scope.filter.area = {};
        } else {
          if ($scope.filter.area){
            $scope.filter.area.srid = 4326;
          } else {
            $scope.filter.area={srid: 4326};
          }
        }
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

        if ($scope.services.length > 0) {
          $scope.storager_service = $scope.services[0].id;
        }
        $timeout(function() {
          $scope.$broadcast('storagerFormatChange', {format: $scope.storager.format, dcps: $scope.dcps});
        });
      };

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

      // Wizard validations
      $scope.isFirstStepValid = function(obj) {
        isWizardStepValid();
        var firstStepValid = $scope.forms.generalDataForm.$valid;
        if (firstStepValid){
          $scope.wizard.parameters.disabled = false;
          if ($scope.dataSeries.semantics.allow_direct_access === false){
            $scope.wizard.store.disabled = false;
            $scope.advanced.store.disabled = false;
            $scope.advanced.store.optional = false;
          }
        } 
        else {
          $scope.wizard.parameters.disabled = true;
          $scope.wizard.store.disabled = true;
          $scope.advanced.store.disabled = true;
          $scope.advanced.store.optional = true;
        }
        return firstStepValid;
      };

      $scope.isSecondStepValid = function(obj) {
        if ($scope.dataSeries.semantics.data_series_type_name === "DCP")
          if ($scope.dcps.length === 0) {
            // todo: display alert box
            console.log("it should have at least one dcp");
            $scope.alertBox.message = i18n.__("It should have at least one dcp");
            $scope.display = true;
            this["wzData"].error = true;
            return true;
          } else {
            this["wzData"].error = false;
            return true;
          }
        isWizardStepValid();
        return true;
      };

      $scope.validateSteps = function(obj) {
        isWizardStepValid();
        if ($scope.forms.storagerForm.$valid && $scope.forms.storagerDataForm.$valid && $scope.dataSeries.semantics.data_series_type_name == "GRID"){
          $scope.filter.area.showCrop = true;
        } else {
          $scope.filter.area.showCrop = false;
        }
        return true;
      };
      //. end wizard validations

      $scope.semantics = "";
      $scope.dcps = [];

      $scope.updatingDcp = false;

      // fill out interface with values
      $scope.parametersData = configuration.parametersData || {};

      var inputDataSeries = configuration.dataSeries.input || {};
      var outputDataseries = configuration.dataSeries.output || {};

      var inputSemantics = inputDataSeries.data_series_semantics || {};

      // update mode
      $scope.isUpdating = Object.keys(inputDataSeries).length > 0;
      $scope.hasCollector = Object.keys(outputDataseries).length > 0;

      $scope.scheduleOptions = { };

      var inputName = "";

      if ($scope.isUpdating) {
        $scope.options = {};
        // checking input dataseries is static
        if (inputDataSeries.data_series_semantics.temporality === globals.enums.TemporalityType.STATIC ||
            !outputDataseries || Object.keys(outputDataseries).length === 0) {
          inputName = inputDataSeries.name;
        } else {
          inputName = inputDataSeries.name.slice(0, inputDataSeries.name.lastIndexOf('_input'));
        }
      } else {
        $scope.options = {};
      }

      $scope.dataSeries = {
        data_provider_id: (inputDataSeries.data_provider_id || "").toString(),
        name: inputName,
        description: inputDataSeries.description,
        access: $scope.hasCollector ? "COLLECT" : "PROCESSING",
        semantics: inputSemantics.code || "",
        active: inputDataSeries.active
      };

      // getting semantics
      var queryParams = {
        metadata: true
      };

      if ($scope.isDynamic) {
        queryParams["type"] = "dynamic";
      } else {
        queryParams["type"] = "static";
      }

      // list data series
      DataSeriesFactory.get({schema: 'all'}).success(function(dataSeriesList) {
        $scope.dataSeriesList = dataSeriesList;

        // fill intersection data series
        $scope.dataSeriesList.forEach(function(dSeries) {
          var temporality = dSeries.data_series_semantics.temporality;
          switch(temporality) {
            //Remove comment when its possible to do intersection with dynamic data
            /*
            case globals.enums.TemporalityType.DYNAMIC:
              if (dSeries.data_series_semantics.data_series_type_name === globals.enums.DataSeriesType.GRID)
                $scope.dataSeriesGroups[1].children.push(dSeries);
              break;
            */
            case globals.enums.TemporalityType.STATIC:
              $scope.dataSeriesGroups[0].children.push(dSeries);
              break;
            default:
              break;
          }
        });

        if ($scope.isUpdating) {
          // setting intersection values
          var collector = configuration.collector || {};
          var intersection = collector.intersection || [];

          if (intersection.length === 0) {
            return;
          }

          var attrs = [];
          intersection.forEach(function(element) {
            attrs.push(element.attribute);
            $scope.dataSeriesList.some(function(ds) {
              if (ds.id === element.dataseries_id) {
                $scope.addDataSeries(ds);

                var target = $scope.intersection[ds.id];
                target.selected = true;

                if (canAddAttribute(target.data_series, element.attribute, target.attributes)) {
                  target.attributes.push(element.attribute);
                }

                return true;
              }
            });
          });
        }
      }).error(function(err) {
        console.log("Could not list data series ", err);
      });

      DataSeriesSemanticsFactory.list(queryParams).success(function(semanticsList) {
        $scope.dataSeriesSemantics = semanticsList;

        if ($scope.dataSeries.semantics) {
          semanticsList.forEach(function(semantics) {
            if (semantics.code === $scope.dataSeries.semantics) {
              $scope.dataSeries.semantics = semantics;
              $scope.onDataSemanticsChange();
            }
          })
        }

      }).error(function(err) {
        console.log(err);
      });

      $scope.dataProviders = [];

      DataProviderFactory.get().success(function(dataProviders) {
        $scope.dataProvidersList = dataProviders;
      }).error(function(err) {
        console.log(err);
      });

      $scope.i18n = i18n;
      $scope.isWizard = $scope.stateApp.current.name === "wizard";
      $scope.projection = "";

      // change form: advanced or wizard
      $scope.onFormView = function() {
        $scope.isWizard = !$scope.isWizard;
        if ($scope.isUpdating) {
          // fixing storager loading
          $timeout(function() {
            $scope.onStoragerFormatChange();
          }, 1000);
        }
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
          if ((val && Object.keys(val).length == 0) || val == null) {
            $scope.dataSeries.access = 'PROCESSING';
          } else {
            $scope.dataSeries.access = 'COLLECT';
          }
        }
      });

      // function to fill out parameters data and storager data
      var _processParameters = function() {
        $scope.dataSeriesSemantics.forEach(function(dSemantic) {
          if (dSemantic.name == outputDataseries.data_series_semantic_name) {
            $scope.storager.format = dSemantic;
            $scope.onStoragerFormatChange();
          }
        });
      }

      // it defines when data change combobox has changed and it will adapt the interface
      $scope.onDataSemanticsChange = function() {
        $scope.semantics = $scope.dataSeries.semantics.data_series_type_name;
        $scope.storager.format = null;
        $scope.storagerFormats = [];
        $scope.showStoragerForm = false;
        delete $scope.wizard.store.error;
        clearStoreForm();

        if ($scope.dataSeries.semantics.allow_direct_access === false){
          $scope.wizard.store.required = true;
          $scope.wizard.store.optional = false;
          $scope.advanced.store.disabled = false;
          $scope.advanced.store.optional = false;
        }
        else {
          $scope.wizard.store.required = false;
          $scope.wizard.store.optional = true;
          $scope.advanced.store.disabled = true;
          $scope.advanced.store.optional = true;
        }

        $scope.dataSeriesSemantics.forEach(function(dSemantics) {
          if (dSemantics.data_series_type_name === $scope.dataSeries.semantics.data_series_type_name) {
            if ($scope.dataSeries.semantics.data_series_type_name == "OCCURRENCE" && dSemantics.code == "OCCURRENCE-wfp"){
              return;
            }
            if ($scope.dataSeries.semantics.data_series_type_name == "DCP" && dSemantics.data_format_name !== "POSTGIS"){
              return;
            }
            $scope.storagerFormats.push(Object.assign({}, dSemantics));
          }
        });

        var qParams = {
          metadata: queryParams.metadata,
          type: queryParams.type
        };

        DataSeriesSemanticsFactory.get($scope.dataSeries.semantics.code, qParams).success(function(data) {
          // TODO: filter provider type: FTP, HTTP, etc
          $scope.dataProviders = [];
          $scope.dataProvidersList.forEach(function(dataProvider) {
            data.data_providers_semantics.forEach(function(demand) {
              if (dataProvider.data_provider_type.id == demand.data_provider_type_id)
                $scope.dataProviders.push(dataProvider);
            });
          });

          if (!$scope.isUpdating)
            if ($scope.dataProviders.length > 0) {
              $scope.dataSeries.data_provider_id = $scope.dataProviders[0].id.toString();
            }

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
            });
          }

          // fill out
          if ($scope.isUpdating) {
            $scope.wizard.parameters.disabled = false;
            $scope.wizard.parameters.error = false;
            $scope.wizard.general.error = false;
            if ($scope.semantics === globals.enums.DataSeriesType.DCP) {
              // TODO: prepare format as dcp item

              $scope.dcps = [];
              inputDataSeries.dataSets.forEach(function(dataset) {
                if (dataset.position) {
                  var lat;
                  var long;
                  if (dataset.position.type) {
                    // geojson
                    lat = dataset.position.coordinates[0];
                    long = dataset.position.coordinates[1];
                  } else {
                    var first = dataset.position.indexOf("(");
                    var firstSpace = dataset.position.indexOf(" ", first);
                    lat = parseInt(dataset.position.slice(first+1, firstSpace));

                    var last = dataset.position.indexOf(")", firstSpace);
                    long = dataset.position.slice(firstSpace + 1, last);

                  }
                  dataset.format["latitude"] = lat;
                  dataset.format["longitude"] = long;
                }
                $scope.dcps.push($scope.prepareFormatToForm(dataset.format));
              });
            } else {
              $scope.model = $scope.prepareFormatToForm(inputDataSeries.dataSets[0].format);
              $scope.model.temporal = ($scope.model.temporal == 'true' || $scope.model.temporal == true ? true : false);

              if(typeof $scope.model.timezone === "number") {
                $scope.model.timezone = $scope.model.timezone.toString();
              }
            }

            if ($scope.hasCollector) {
              $scope.wizard.store.disabled = false;
              $scope.wizard.store.error = false;
              $scope.advanced.store.disabled = false;
              $scope.storagerFormats.some(function(storagerFmt) {
                if (storagerFmt.id == outputDataseries.data_series_semantics.id) {
                  $scope.storager.format = storagerFmt;
                  $scope.onStoragerFormatChange();
                  return true;
                }
              });
            }
            if (Object.keys($scope.intersection).length > 0) {
              $scope.wizard.intersection.disabled = false;
              $scope.advanced.intersection.disabled = false;
            }

          } else {
            $scope.dcps = [];
            $scope.model = {};
            $scope.$broadcast("resetStoragerDataSets");
          }

          // $scope.model = {};
          $scope.form = data.metadata.form;
          $scope.schema = {
            type: 'object',
            properties: data.metadata.schema.properties,
            required: data.metadata.schema.required
          };
          $scope.$broadcast('schemaFormRedraw');

          _processParameters();

          $timeout(function(){
            console.log($scope.dataSeriesSemantics);
            if (!$scope.dataSeries.semantics || $scope.dataSeries.semantics.data_format_name != 'POSTGIS'){
              return;
            } else {
              var tableInput = angular.element('#table_name');
              tableInput.attr('list', 'databaseTableList');
            }
          });
        }).error(function(err) {
          console.log("Error in semantics change: ", err);
          $scope.model = {};
          $scope.form = [];
          $scope.schema = {};
          $scope.$broadcast('schemaFormRedraw');
        });
      };

      // Reset model values when change number of bands propertie to 1
      $scope.$watch("model.number_of_bands", function(val){
        if (!$scope.model.number_of_bands){
          return;
        } else {
          if (val == 1){
            $scope.model.bytes_before = 0;
            $scope.model.bytes_after = 0;
            $scope.model.temporal = false;
          }
        }
      });

      $scope.$watch("dataSeries.data_provider_id", function(val) {
        if (!$scope.dataSeries.data_provider_id) {
          return;
        } else {
          var dataProvider = $scope.dataProviders.filter(function(elem){
            return elem.id == $scope.dataSeries.data_provider_id;
          });
          // Provider type of PostGIS is 4
          if (dataProvider.length > 0 && dataProvider[0].data_provider_type.id == 4){
            listTables(dataProvider[0]);
          }
        }
      });

      var listTables = function(dataProvider){
        var result = $q.defer();

        var params = getPostgisUriInfo(dataProvider.uri);
        params.objectToGet = "table";

        var httpRequest = $http({
          method: "GET",
          url: "/uri/",
          params: params
        });

        httpRequest.success(function(data) {
          $scope.tableList = data.data.map(function(item, index){
            return item.table_name;
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
            $scope.$broadcast("dcpOperation", {action: "remove", dcp: data});
            array.splice(pcdIndex, 1);
          }
        });
      };

      var isValidParametersForm = function(form) {
        $scope.$broadcast('schemaFormValidate');

        return form.$valid;
      };

      $scope._addDcpStorager = function(dcpItem) {
        $scope.$broadcast("dcpOperation", {action: "add", dcp: dcpItem});
      };

      $scope.addDcp = function() {
        if (isValidParametersForm($scope.forms.parametersForm)) {
          var data = Object.assign({}, $scope.model);
          data._id = UniqueNumber();
          $scope.dcps.push(Object.assign({}, data));
          $scope._addDcpStorager(data);
          $scope.model = {};

          // reset form to do not display feedback class
          $scope.forms.parametersForm.$setPristine();
        }
      };

      $scope.changeDataProvider = function() {  
        console.log($scope.dataSeries);
      };

      var makeDialog = function(level, bodyMessage, show, title) {
        $scope.alertBox.title = title || "Data Registration";
        $scope.alertBox.message = bodyMessage;
        $scope.alertLevel = level;
        $scope.display = show;
      };

      $scope.close = function() {
        $scope.display = false;
      };

      // it dispatches post operation to nodejs
      var _sendRequest = function(object) {
        var request = null;
        var data = {
          dataSeries: object.dataToSend,
          schedule: object.scheduleValues,
          filter: object.filterValues,
          service: object.serviceOutput,
          intersection: object.intersection,
          active: object.active,
          run: $scope.shouldRun
        };
        if ($scope.isUpdating) {
          request = DataSeriesFactory.put(configuration.dataSeries.input.id, data);
        } else {
          request = DataSeriesFactory.post(data);
        }

        request.then(function(data) {
          console.log(data);
          $window.location.href = "/configuration/" + configuration.dataSeriesType + "/dataseries?token=" + (data.token || data.data.token);
        }).catch(function(err) {
          $scope.alertLevel = "alert-danger";
          $scope.alertBox.message = err.message || err.data.message;
          $scope.display = true;
          $scope.extraProperties = {};
          console.log(err);
        });
      };

      $scope.$on("storageValuesReceive", function(event, values) {
        //  todo: improve
        var dSemantics = Object.assign({}, $scope.dataSeries.semantics);
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

          // adding extra metadata
          // if (dSemantics.metadata.metadata && Object.keys(dSemantics.metadata.metadata).length > 0)
          //   Object.assign(format_, dSemantics.metadata.metadata);

          return format_;
        };

        var out;
        if (dSets instanceof Array) {
          // setting to active
          var dSetsLocal = [];
          dSets.forEach(function(dSet) {
            var outputDcp = {
              active: true,
              format: _makeFormat(dSet)
            };

            if ($scope.dataSeries.semantics.data_format_name !== "POSTGIS") {
              outputDcp.position = Point.build({x: dSet.longitude, y: dSet.latitude, srid: dSet.projection});
            }
            dSetsLocal.push(outputDcp);
          });
          out = dSetsLocal;
        } else {
          dSets.format = _makeFormat(dSets);
          dSets.active = true,//$scope.dataSeries.active;
          out = [dSets];
        }

        // preparing intersection
        var intersectionValues = [];
        for(var k in $scope.intersection) {
          if ($scope.intersection.hasOwnProperty(k)) {
            if (!$scope.intersection[k].selected) {
              continue;
            }
            var attributes = $scope.intersection[k].attributes;
            var dataseries_id = $scope.intersection[k].data_series.id;

            // grid
            if (attributes.length === 0) {
              intersectionValues.push({
                dataseries_id: dataseries_id
              });
            }

            for(var i = 0; i < attributes.length; ++i) {
              var attribute = attributes[i];
              intersectionValues.push({
                attribute: attribute,
                dataseries_id: dataseries_id
              });
            }
          }
        }

        var outputDataSeries = {
          name: dSeriesName,
          description: dataObject.dataSeries.description,
          active: dataObject.dataSeries.active,
          data_series_semantics_id: values.semantics.id,
          data_provider_id: values.data_provider,
          dataSets: out
        };

        _sendRequest({
          dataToSend: {input: dataObject.dataSeries, output: outputDataSeries},
          scheduleValues: dataObject.schedule,
          filterValues: dataObject.filter,
          serviceOutput: values.service,
          intersection: intersectionValues,
          active: dataObject.dataSeries.active
        });
      });

      // it prepares dataseries object, schedule and filter object
      var _save = function() {
        var dataToSend = Object.assign({}, $scope.dataSeries);
        dataToSend.data_series_semantics_id = $scope.dataSeries.semantics.id;

        var semantics = Object.assign({}, dataToSend.semantics);
        delete dataToSend.semantics;

        if(dataToSend.active === undefined)
          dataToSend.active = false;

        dataToSend.dataSets = [];

        $scope.errorFound = false;

        switch(semantics.data_series_type_name) {
          case "DCP":
            $scope.dcps.forEach(function(dcp) {
              var format = {};
              for(var key in dcp) {
                if (dcp.hasOwnProperty(key))
                  if (key !== "latitude" && key !== "longitude" && key !== "active")
                    format[key] = dcp[key];
              }
              var dataSetStructure = {
                active: true,//$scope.dataSeries.active,
                format: format,
                position: Point.build({x: dcp.longitude, y: dcp.latitude, srid: dcp.projection})
              };

              dataToSend.dataSets.push(dataSetStructure);
            });

            break;
          case "OCCURRENCE":
          case "GRID":
          case "GEOMETRIC_OBJECT":
            var format = $scope.model;

            var dataSet = {
              semantics: semantics,
              active: true,//$scope.dataSeries.active,
              format: format
            };
            dataToSend.dataSets.push(dataSet);
            break;

          default:
            break;
        }

        var filterValues = Object.assign({}, $scope.filter);
        if ($scope.filter.filterArea === $scope.filterTypes.AREA.value) {
          filterValues.region = Polygon.build($scope.filter.area || {});
        }

        var scheduleValues = Object.assign({}, $scope.schedule);
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
            break;
        }

        console.log(dataToSend);

        return {
          dataSeries: dataToSend,
          schedule: scheduleValues,
          filter: filterValues
        };
      };

      $scope.save = function(shouldRun) {
        $scope.shouldRun = shouldRun;
        $scope.extraProperties = {};
        $scope.$broadcast('formFieldValidation');
        $scope.display = false;
        $scope.alertBox.title = "Data Series";
        $scope.alertBox.message = "";

        if ($scope.isWizard) {
          isWizardStepValid();
        }

        if($scope.forms.generalDataForm.$invalid) {
          makeDialog("alert-danger", "There are invalid fields on form", true);
          return;
        }
        // checking parameters form (semantics) is invalid
        if ($scope.dcps.length === 0 && !isValidParametersForm($scope.forms.parametersForm)) {
          makeDialog("alert-danger", "There are invalid fields on form", true);
          return;
        }

        if ($scope.isDynamic) {
          var scheduleForm = angular.element('form[name="scheduleForm"]').scope()['scheduleForm'];
          if (scheduleForm.$invalid) {
            makeDialog("alert-danger", "There are invalid fields on form", true);
            return;
          }
        }

        if ($scope.filter.filterArea == $scope.filterTypes.AREA.value) {
          if (FilterForm.boundedForm.$invalid){
            $scope.alertBox.message = "Invalid filter area";
            $scope.alertLevel = "alert-danger";
            $scope.display = true;
            return;
          }
        }

        // checking intersection
        if (Object.keys($scope.intersection).length > 0) {
          for(var k in $scope.intersection) {
            if ($scope.intersection.hasOwnProperty(k)) {
              if (!$scope.intersection[k].selected) {
                continue;
              }

              var dsIntersection = $scope.intersection[k].data_series;

              // checking GRID. Grid does not need attribute
              if (dsIntersection.data_series_semantics.data_series_type_name !== globals.enums.DataSeriesType.GRID) {
                if ($scope.intersection[k].attributes.length === 0) {
                  $scope.alertBox.message = "Invalid intersection. Static data series must have at least a attribute.";
                  $scope.alertLevel = "alert-danger";
                  $scope.display = true;
                  return;
                }
              }
            }
          }
        }

        $scope.alertBox.title = "Data Series Registration";

        if ($scope.dataSeries.access == 'COLLECT') {
          // getting values from another controller
          $scope.$broadcast("requestStorageValues");
        } else {
          if ($scope.dataSeries.semantics.data_format_name === globals.enums.DataSeriesFormat.GRADS) {
            $scope.alertLevel = "alert-danger";
            $scope.alertBox.title = "Data Series";
            $scope.alertBox.message = i18n.__("Unconfigured GraDs Data Series storage");
            $scope.display = true;
            return;
          }

          var dataObject = _save();

          if ($scope.isDynamic) {
            //  display alert box
            $scope.alertLevel = "alert-warning";
            $scope.alertBox.title = "Data Series";
            $scope.alertBox.message = i18n.__("Note: No storager configuration, this data will be accessed when needed.");
            $scope.display = true;
            $scope.extraProperties.object = {
              dataToSend: dataObject.dataSeries,
              scheduleValues: dataObject.schedule,
              filterValues: dataObject.filter
            };
            $scope.extraProperties.confirmButtonFn = _sendRequest;
          } else {
            _sendRequest({
              dataToSend: dataObject.dataSeries,
              scheduleValues: {},
              filterValues: dataObject.filter,
              serviceOutput: {},
              active: dataObject.dataSeries.active || true
            });
          }
        }
      };
    }
  ]);
