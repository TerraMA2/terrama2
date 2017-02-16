define([], function(){
  /**
   * It defines a Component structure for handling Store form on Dynamic data registration
   * 
   * @property {Object} bindings - Defines component bindings to work
   * @type {angular.IComponent}
   */
    var terrama2StoragerComponent = {
        bindings: {
            providersList: "<", 
            storager: "<",
            series: "<",
            filter: "<",
            prepareFormatToForm: "<",
            forms: "<",
            onStoragerFormatChange: "<",
            model: "<",
            options: "=",
            schedule: "<"
        },
        templateUrl: "/dist/templates/data-series/templates/storager.html",
        controller: StoragerController
    };

  /**
   * It handles component behavior
   * 
   * @param {any} i18n - TerraMA² Internationalization module
   */
    function StoragerController($scope, i18n, DataSeriesSemanticsService, GeoLibs, SemanticsParserFactory, $timeout, $window, Service){
      var self = this;
      self.i18n = i18n;
      self.formStorager = [];
      self.modelStorager = {};
      self.schemaStorager = {};
      self.tableFieldsStorager = [];
      self.formatSelected = {};
      self.dcpsStorager = [];
      self.inputDataSets = [];
      self.storage = {};
      self.dataProvidersStorager = [];

      self.services = [];
      
      self.services = Service.list({service_type_id: globals.enums.ServiceType.COLLECTOR});

      function onInputSemanticsChange(){
        self.semantics = self.series.semantics.data_series_type_name;
        self.storager.format = null;
        self.storagerFormats = [];
        self.options.showStoragerForm = false;
        delete self.options.wizard.store.error;

        // checking if store form must be required
        if (self.series.semantics.allow_direct_access === false){
          self.options.wizard.store.required = true;
          self.options.wizard.store.optional = false;
          self.options.advanced.store.disabled = false;
          self.options.advanced.store.optional = false;
        }
        else {
          self.options.wizard.store.required = false;
          self.options.wizard.store.optional = true;
          self.options.advanced.store.optional = true;
        }

        self.dataSeriesSemantics = DataSeriesSemanticsService.list();

        // Setting storage formats based in input data series
        self.dataSeriesSemantics.forEach(function(dSemantics) {
          if (dSemantics.data_series_type_name === self.series.semantics.data_series_type_name) {
            if (self.series.semantics.data_series_type_name == "OCCURRENCE" && dSemantics.code == "OCCURRENCE-wfp"){
              return;
            }
            if (self.series.semantics.data_series_type_name == "DCP" && dSemantics.data_format_name !== "POSTGIS"){
              return;
            }
            self.storagerFormats.push(Object.assign({}, dSemantics));
          }
        });
        if (self.options.isUpdating && self.options.hasCollector){

            self.storagerFormats.some(function(storagerFmt) {
              if (storagerFmt.id == $window.configuration.dataSeries.output.data_series_semantics.id) {
                self.storager.format = storagerFmt;
                self.onStoragerFormatChange();
                return true;
              }
            });
        }

        //Checking if is updating to change output when changed the parameters in Grads data series type
        if (self.options.isUpdating && self.series.semantics.code == 'GRID-grads'){

            // function to update model storager properties
            var updateModelStorage = function(inputModel){
                self.modelStorager.binary_file_mask = inputModel.binary_file_mask;
                self.modelStorager.bytes_after = inputModel.bytes_after;
                self.modelStorager.bytes_before = inputModel.bytes_before;
                self.modelStorager.ctl_filename = inputModel.ctl_filename;
                self.modelStorager.data_type = inputModel.data_type;
                self.modelStorager.number_of_bands = inputModel.number_of_bands;
                self.modelStorager.srid = inputModel.srid;
                self.modelStorager.temporal = inputModel.temporal;
                if (inputModel.time_interval){
                    self.modelStorager.time_interval = inputModel.time_interval;
                }
                if (inputModel.time_interval_unit){
                    self.modelStorager.time_interval_unit = inputModel.time_interval_unit;
                }
                if (inputModel.value_multiplier){
                    self.modelStorager.value_multiplier = inputModel.value_multiplier;
                }
            }

            // watch model to update modelStorage
            var timeoutPromise;
            $scope.$watch(function(){
                return self.model;
            }, function(modelValue){
                $timeout.cancel(timeoutPromise);
                timeoutPromise = $timeout(function(){
                    if (modelValue) updateModelStorage(modelValue);
                }, 700);
            }, true);
        }
      }

      $scope.$watch(function(){
          return self.series.semantics.data_series_type_name;
      }, onInputSemanticsChange, true);

      $scope.$on("changeDataSemantics", onInputSemanticsChange);

      var removeInput = function(dcpMask) {
        self.inputDataSets.some(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (dcp.mask === dcpMask) {
            array.splice(pcdIndex, 1);
            return true;
          }
        });
      };

      self.removePcdStorager = function(dcpItem) {
        self.dcpsStorager.some(function(dcp, pcdIndex, array) {
          // todo: which fields should compare to remove?
          if (parseInt(dcp._id) === parseInt(dcpItem._id)) {
            array.splice(pcdIndex, 1);
            return true;
          }
        });
      };

      self.editDcpStorager = function(dcpItem){
        self.dcpsStorager.some(function(dcp, dcpIndex, array){
          if (parseInt(dcp._id) === parseInt(dcpItem._id)){
            var table_name = dcp.table_name;
            array[dcpIndex] = dcpItem;
            array[dcpIndex].table_name = table_name;
            return true;
          }
        });
      }

      self.addDcpStorager = function() {
        $scope.$broadcast('schemaFormValidate');
        var form = angular.element('form[name="storagerForm"]').scope()['storagerForm'];
        var inputDataSetForm = angular.element('form[name="inputDataSetForm"]').scope()['inputDataSetForm'];
        if (form.$valid && inputDataSetForm.$valid) {
          self.model['inputDataSet'] = self.storage.inputDataSet.mask;
          self.dcpsStorager.push(Object.assign({}, self.model));
          self.model = {};

          // remove it from input list
          removeInput(self.storage.inputDataSet.mask);

          // reset form to do not display feedback class
          form.$setPristine();
          inputDataSetForm.$setPristine();
        }
      };

      $scope.$on("requestStorageValues", function() {
        // apply a validation
        $scope.$broadcast('schemaFormValidate');

        if (self.forms.storagerForm.$valid && self.forms.storagerDataForm.$valid) {
          // checking if it is a dcp
          switch (self.formatSelected.data_series_type_name) {
            case "DCP":
              $scope.$emit("storageValuesReceive", {
                data: self.dcpsStorager,
                data_provider: self['storager_data_provider_id'],
                service: self["storager_service"],
                type: self.formatSelected.data_series_type_name,
                semantics: self.formatSelected
              });
              break;
            case "GRID":
            case "OCCURRENCE":
              $scope.$emit("storageValuesReceive", {
                data: self.modelStorager,
                data_provider: self['storager_data_provider_id'],
                service: self["storager_service"],
                type: self.formatSelected.data_series_type_name,
                semantics: self.formatSelected
              });
              break;
            default:
              $scope.$emit("storageValuesReceive", {data: null, type: null});
              break;
          }
        } else {
          angular.forEach(self.forms.storagerDataForm.$error, function (field) {
            angular.forEach(field, function(errorField){
              errorField.$setDirty();
            });
          });
        }
      });

      $scope.$on("dcpOperation", function(event, args) {
        if (args.action === "remove") {
          self.removePcdStorager(args.dcp);
        //  todo: remove it from list
          removeInput(args.dcp.mask);
        } else if (args.action === "add") {
          if (self.storager.format && self.storager.format.data_format_name === globals.enums.DataSeriesFormat.POSTGIS) {
            // postgis
            var copyFormat = angular.merge({}, self.series.semantics.metadata.metadata);
            angular.merge(copyFormat, args.dcp);
            var obj = SemanticsParserFactory.parseKeys(copyFormat);
            obj.table_name = obj.alias;
            self.dcpsStorager.push(obj);
          } else {
            self.dcpsStorager.push(args.dcp);
          }
        } else if (args.action === "edit"){
          self.editDcpStorager(args.dcp);
        }
      });

      $scope.$on("resetStoragerDataSets", function(event) {
        self.dcpsStorager = [];
      });

      $scope.$on("clearStoreForm", function(event){
          self.modelStorager = {};
          self.formStorager = [];
          self.schemaStorager = {};
          self.storager.format = null;
          self.storager_service = undefined;
          self.dcpsStorager = [];
          self.storager_data_provider_id = undefined;
          $scope.$broadcast("clearSchedule");
      });

      $scope.$on('storagerFormatChange', function(event, args) {
        self.formatSelected = args.format;
        // todo: fix it. It is hard code
        self.tableFieldsStorager = [];

        var dataSeriesSemantics = DataSeriesSemanticsService.get({code: args.format.code});

        self.dataProvidersStorager = [];
        self.dcpsStorager = [];
        self.providersList.forEach(function(dataProvider) {
          dataSeriesSemantics.data_providers_semantics.forEach(function(demand) {
            if (dataProvider.data_provider_type.id == demand.data_provider_type_id){
              if (self.storager.format.data_series_type_name == 'GRID' && dataProvider.data_provider_type.id != 1 )
                return;
              self.dataProvidersStorager.push(dataProvider);
            }
          })
        });

        if (self.dataProvidersStorager.length > 0){
          self.forms.storagerDataForm.storager_data_provider_id.$setViewValue(self.dataProvidersStorager[0]);
          self.storager_data_provider_id = self.dataProvidersStorager[0].id;
        }

        if (self.services.length > 0) {
          self.forms.storagerDataForm.service.$setViewValue(self.services[0]);
          self.storager_service = self.services[0].id;
        }

        $scope.$broadcast('formFieldValidation');
        var metadata = dataSeriesSemantics.metadata;
        var properties = metadata.schema.properties;

        if (self.options.isUpdating) {
          if (self.formatSelected.data_series_type_name === globals.enums.DataSeriesType.DCP) {
            // todo:
          } else {
            if (configuration.dataSeries.output){
              self.modelStorager = self.prepareFormatToForm(configuration.dataSeries.output.dataSets[0].format);
            } else {
              var copyFormat = angular.merge({}, self.series.semantics.metadata.metadata);
              angular.merge(copyFormat, self.model);
              self.modelStorager = SemanticsParserFactory.parseKeys(copyFormat);
              self.filter.area = {
                srid: 4326
              };
            }
          }
        } else {
          var copyFormat = angular.merge({}, self.series.semantics.metadata.metadata);
          angular.merge(copyFormat, self.model);
          self.modelStorager = SemanticsParserFactory.parseKeys(copyFormat);
          self.filter.area = {
            srid: 4326
          };
        }

        var outputDataseries = $window.configuration.dataSeries.output;

        if (self.options.hasCollector) {
          var collector = $window.configuration.collector;
          self.storager_service = collector.service_instance_id;
          self.storager_data_provider_id = outputDataseries.data_provider_id;

          var schedule = collector.schedule;
          if (schedule && (schedule.frequency_unit || schedule.schedule_unit)){
            self.schedule.scheduleType = globals.enums.ScheduleType.SCHEDULE;
          }
          else {
            self.schedule.scheduleType = globals.enums.ScheduleType.MANUAL;
          }

          $timeout(function() {
            $scope.$broadcast("updateSchedule", schedule);
          }, 1000);
        }

        if (self.formatSelected.data_series_type_name === globals.enums.DataSeriesType.DCP) {
          Object.keys(properties).forEach(function(key) {
            self.tableFieldsStorager.push(key);
          });

          if (self.options.hasCollector) {
            outputDataseries.dataSets.forEach(function(dataset) {
              self.dcpsStorager.push(angular.merge(dataset.format, {active: dataset.active}));
            });
          } else {
            (args.dcps || []).forEach(function(dataSetDcp) {
              $scope.$broadcast("dcpOperation", {action: "add", dcp: dataSetDcp});
            });
          }

          self.modelStorager = {};
          self.formStorager = [];
          self.schemaStorager = {};
          $scope.$broadcast('schemaFormRedraw');
        } else {
          // occurrence
          self.formStorager = metadata.form;
          self.schemaStorager = {
            type: 'object',
            properties: metadata.schema.properties,
            required: metadata.schema.required
          };
          $scope.$broadcast('schemaFormRedraw');

          if (!outputDataseries)
            return;

          // fill out default
          if (self.formatSelected.data_series_type_name != globals.enums.DataSeriesType.DCP) {
            self.modelStorager = self.prepareFormatToForm(outputDataseries.dataSets[0].format);
            if(typeof self.modelStorager.timezone === "number") {
              self.modelStorager.timezone = self.modelStorager.timezone.toString();
            }
          }
        }
      });
    }
    
    StoragerController.$inject = ['$scope', 'i18n', 'DataSeriesSemanticsService', 'GeoLibs', 'SemanticsParserFactory', '$timeout', '$window', 'Service']; 
    return terrama2StoragerComponent;
});