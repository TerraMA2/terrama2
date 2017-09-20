define([], function() {
  function RegisterDataSeries($scope, $http, i18n, $window, $state, $httpParamSerializer,
                              DataSeriesSemanticsService, DataProviderService, DataSeriesService,
                              Service, $timeout, WizardHandler, UniqueNumber, 
                              FilterForm, MessageBoxService, $q, GeoLibs, $compile, DateParser, FormTranslator, Socket) {

    $scope.forms = {};
    $scope.isDynamic = configuration.dataSeriesType === "dynamic";
    $scope.hasProjectPermission = configuration.hasProjectPermission;
    $scope.semantics = "";
    var queryParameters = {
      metadata: true,
      type: $scope.isDynamic ? "dynamic" : "static"
    };
    $scope.csvFormatData = { fields: [{type: "DATETIME"}], convert_all: false};
    // defining box
    $scope.cssBoxSolid = {
      boxType: "box-solid"
    };
    $scope.dataSeries = {};
    $scope.dataSeriesSemantics = [];
    $scope.storeOptions = {};
    $scope.storeOptions.isDynamic = $scope.isDynamic;

    // Flag to verify if can not save if the service is not running
    var canSave = true;
    var serviceOfflineMessage = "If service is not running you can not save the data series. Start the service before create or update a data series!";

    Socket.on('statusResponse', function(response){
      if ($scope.forms.storagerDataForm.service && $scope.forms.storagerDataForm.service.$modelValue == response.service){
        if (response.checking === undefined || (!response.checking && response.status === 400)) {
          if (!response.online){
            MessageBoxService.danger(i18n.__("Data Registration"), i18n.__(serviceOfflineMessage));
            canSave = false;
          } else {
            canSave = true;
          }
        }
      }
    });

    // Functions to enable and disable forms
    // clear optional forms
    var clearStoreForm = function(){
      clearIntersectionForm();
      $scope.storeOptions.showStoragerForm = false;
      $scope.schedule = {};
      $scope.scheduleOptions = {};
      $scope.advanced.store.disabled = true;
      $scope.wizard.intersection.message = i18n.__("Must have a valid store values to create an intersection");
      $scope.wizard.store.message = i18n.__("Add store configuration");
      $scope.$broadcast('clearStoreForm');
      var enableStore = angular.element('#store-collapse');
      var storebox = angular.element('#store-box');
      if (!storebox.hasClass('collapsed-box')){
        enableStore.click();
      }
      //disable option to crop on filter
      $scope.filter.area.showCrop = false;
      $scope.filter.area.crop_raster = false;
    };

    var clearFilterForm = function(){
      $scope.filter.date = {};
      $scope.filter.filterArea = "1";
      delete $scope.filter.data_series_id;
      $scope.advanced.filter.disabled = true;
      $scope.wizard.filter.message = i18n.__("Add filter configuration");
      var enableFilter = angular.element('#filter-collapse');
      var filterbox = angular.element('#filter-box');
      if (!filterbox.hasClass('collapsed-box')){
        enableFilter.click();
      }
    };

    var clearIntersectionForm = function(){
      for (var key in $scope.intersection) {
        $scope.removeDataSeries(key);
      }
      $scope.advanced.intersection.disabled = true; 
      $scope.wizard.intersection.disabled = true;
      $scope.wizard.intersection.message = i18n.__("Add intersection configuration");
      var enableIntersection = angular.element('#intersection-collapse');
      var intersectionbox = angular.element('#intersection-box');
      if (!intersectionbox.hasClass('collapsed-box')){
        enableIntersection.click();
      }
    };

    // open optional forms in advanced mode
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
    };

    var openIntersectionForm = function(){
      if ($scope.dataSeries.access != 'PROCESSING'){
        $scope.advanced.intersection.disabled = false;
        var enableIntersection = angular.element('#intersection-collapse');
        // set disabled to false, to open form
        enableIntersection.attr("disabled", false);
        enableIntersection.click();
      }
    };

    var makeid = function(length) {
      var text = "";
      var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

      for(var i = 0; i < length; i++)
        text += possible.charAt(Math.floor(Math.random() * possible.length));

      return text;
    }

    var storedDcpsKey = makeid(30);

    var reloadData = function() {
      if($scope.dcpTable !== undefined)
        $scope.dcpTable.ajax.reload(null, false);
    }

    // advanced global properties
    $scope.advanced = {
      store: {
        disabled: true,
        clearForm: clearStoreForm,
        openForm: openStoreForm,
        optional: true
      },
      csvFormat: {
        required: true,
        disabled: false
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
        optional: true,
        message: i18n.__("Must have a valid store values to create an intersection") 
      }
    };

    // Function to enable optional forms on wizard mode
    var enableStoreForm = function(){
      $scope.wizard.store.disabled = false;
      $scope.wizard.store.message = i18n.__("Remove store configuration");
    }

    var enableFilterForm = function(){
      $scope.wizard.filter.disabled = false;
      $scope.wizard.filter.message = i18n.__("Remove filter configuration");
    }

    var enableIntersectionForm = function(){
      if ($scope.dataSeries.access != 'PROCESSING'){
        $scope.wizard.intersection.disabled = false;
        $scope.wizard.intersection.message = i18n.__("Remove intersection configuration");
      }
    }
    // wizard global properties
    $scope.wizard = {
      general: {
        required: true,
        formName: 'generalDataForm'
      },
      csvFormat: {
        required: true,
        formName: 'csvFormatForm',
        disabled: true
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
        enableForm: enableStoreForm,
        clearForm: clearStoreForm,
        message: i18n.__("Add store configuration")
      },
      filter: {
        required: false,
        formName: 'filterForm',
        disabled: true,
        optional: true,
        enableForm: enableFilterForm,
        clearForm: clearFilterForm,
        message: i18n.__("Add filter configuration")
      },
      intersection: {
        required: false,
        formName: 'intersectionForm',
        disabled: true,
        optional: true,
        enableForm: enableIntersectionForm,
        clearForm: clearIntersectionForm,
        message: i18n.__("Must have a valid store values to create an intersection")
      }
    };

    $scope.storeOptions.advanced = $scope.advanced;
    $scope.storeOptions.wizard = $scope.wizard;
    // initializing async modules
    $q.all([
      DataSeriesSemanticsService.init(queryParameters),
      DataProviderService.init(),
      Service.init({type: "COLLECT"}),
      DataSeriesService.init({schema: "all"})
    ]).then(function() {
      // definition of schema form
      $scope.schema = {};
      $scope.form = [];
      $scope.model = {};
      $scope.custom_format = false;

      // function to fill out parameters data and storager data
      var _processParameters = function() {
        $scope.dataSeriesSemantics.forEach(function(dSemantic) {
          if (dSemantic.name == outputDataseries.data_series_semantic_name) {
            $scope.storager.format = dSemantic;
            $scope.onStoragerFormatChange();
          }
        });
      };

      // fill out interface with values
      $scope.parametersData = configuration.parametersData || {};

      var inputDataSeries = configuration.dataSeries.input || {};
      var outputDataseries = configuration.dataSeries.output || {};

      var inputSemantics = inputDataSeries.data_series_semantics || {};

      // update mode
      $scope.isUpdating = Object.keys(inputDataSeries).length > 0;
      $scope.hasCollector = Object.keys(outputDataseries).length > 0;
      $scope.storeOptions.isUpdating = $scope.isUpdating;
      $scope.storeOptions.hasCollector = $scope.hasCollector;

      $scope.isChecking = {
        value: $scope.isUpdating
      };

      $scope.BASE_URL = BASE_URL;

      // consts
      $scope.filterTypes = {
        NO_FILTER: {
          name: i18n.__("Do not filter"),
          value: "1"
        },
        AREA: {
          name: i18n.__("Filter by limits"),
          value: "2"
        },
        STATIC_DATA: {
          name: i18n.__("Filter by static data"),
          value: "3"
        }
      };

      $scope.dataProviders = [];

      $scope.semanticsSelected = false;

      $scope.providersList = DataProviderService.list();

      $scope.getImageUrl = getImageUrl;

      function getImageUrl(dataSeries){
        if (typeof dataSeries != 'object'){
          return '';
        }
        switch (dataSeries.data_provider_type.name){
          case "FILE":
            return BASE_URL + "images/data-server/file/file.png";
            break;
          case "FTP":
            return BASE_URL + "images/data-server/ftp/ftp.png";
            break;
          case "SFTP":
            return BASE_URL + "images/data-server/sftp/sftp.png";
            break;
          case "HTTP":
          case "HTTPS":
            return BASE_URL + "images/data-server/http/http.png";
            break;
          case "POSTGIS":
          default:
            return BASE_URL + "images/data-server/postGIS/postGIS.png";
            break;
        }
      }

      $scope.createDataTable = function() {
        if($scope.dcpTable !== undefined)
          $scope.dcpTable.destroy();

        var dtColumns = [];

        for(var i = 0, fieldsLength = $scope.tableFields.length; i < fieldsLength; i++) {
          dtColumns.push({ "data": $scope.tableFields[i] + '_html' });
        }

        dtColumns.push({ "data": 'removeButton' });

        $scope.dcpTable = $('.dcpTable').DataTable(
          {
            "ordering": false,
            "searching": false,
            "responsive": false,
            "processing": true,
            "serverSide": true,
            "ajax": {
              "url": BASE_URL + "configuration/dynamic/dataseries/paginateDcps",
              "type": "POST",
              "data": function(data) {
                data.key = storedDcpsKey;
              }
            },
            "columns": dtColumns,
            "language": {
              "emptyTable": "<p class='text-center'>" + i18n.__("No data available in table") + "</p>",
              "info": i18n.__("Showing") + " _START_ " + i18n.__("to") + " _END_ " + i18n.__("of") + " _TOTAL_ " + i18n.__("entries"),
              "infoEmpty": i18n.__("Showing 0 to 0 of 0 entries"),
              "infoFiltered": "(" + i18n.__("filtered from") + " _MAX_ " + i18n.__("total entries") + ")",
              "lengthMenu": i18n.__("Show") + " _MENU_ " + i18n.__("entries"),
              "loadingRecords": i18n.__("Loading") + "...",
              "processing": i18n.__("Processing") + "...",
              "search": i18n.__("Search") + ":",
              "zeroRecords": "<p class='text-center'>" + i18n.__("No data available in table") + "</p>",
              "paginate": {
                "first": i18n.__("First"),
                "last": i18n.__("Last"),
                "next": i18n.__("Next"),
                "previous": i18n.__("Previous")
              }
            },
            "drawCallback": function() {
              $scope.compileTableLines();
            }
          }
        );
      };

      $scope.compileTableLines = function() {
        $('.dcpTable .dcps-table-span').css('display', 'none');

        $timeout(function() {
          if($('.dcps-table-span').text().match("{{(.*)}}") !== null)
            $compile(angular.element('.dcpTable > tbody > tr'))($scope);

          $('.dcpTable .dcps-table-span').css('display', '');
        }, 50);
      };

      $scope.countObjectProperties = function(object) {
        var count = 0;
        
        if(object !== undefined && object !== null && typeof object === "object")
          for(key in object) if(object.hasOwnProperty(key)) count++;

        return count;
      };

      $scope.storageDcps = function(dcps) {
        $http.post(BASE_URL + "configuration/dynamic/dataseries/storeDcps", {
          key: storedDcpsKey,
          dcps: dcps
        }).then(function(result) {
          reloadData();
        }, function(error) {
          console.log("Err in storing dcps");
        });
      };

      $scope.addDcpsStorager = function(dcps) {
        $scope.$broadcast("dcpOperation", { action: "addMany", dcps: dcps, storageData: true, reloadDataStore: false });
      };

      $scope.setHtmlItems = function(dcp, key, alias, _id, type) {
        if($scope.isBoolean(dcp[key]))
          dcp[key + '_html'] = "<span class=\"dcps-table-span\"><input type=\"checkbox\" ng-model=\"dcpsObject['" + alias + "']['" + key + "']\" ng-change=\"insertEditedDcp('" + _id + "')\"></span>";
        else
          dcp[key + '_html'] = "<span class=\"dcps-table-span\" editable-text=\"dcpsObject['" + alias + "']['" + key + "']\" onaftersave=\"insertEditedDcp('" + _id + "')\" onbeforesave=\"validateFieldEdition($data, '" + type + "', '" + alias + "', '" + key + "')\">{{ (dcpsObject['" + alias + "']['" + key + "'] === \"\" ? \"&nbsp;&nbsp;&nbsp;&nbsp;\" : dcpsObject['" + alias + "']['" + key + "']) }}</span>";

        return dcp;
      };

      $scope.getRemoveButton = function(alias) {
        return "<button class=\"btn btn-danger removeDcpBtn\" ng-click=\"removePcd('" + alias + "')\" style=\"height: 21px; padding: 1px 4px 1px 4px; font-size: 13px;\">" + i18n.__("Remove") + "</button>";
      };

      // it defines when data change combobox has changed and it will adapt the interface
      $scope.onDataSemanticsChange = function() {
        if(!$scope.semanticsSelected)
          $scope.semanticsSelected = true;

        $scope.semantics = $scope.dataSeries.semantics.data_series_type_name;
        if (!$scope.isUpdating){
          $scope.csvFormatData = { fields: [{type: "DATETIME"}], convert_all: false};
          clearStoreForm();
        }
        $scope.custom_format = $scope.dataSeries.semantics.custom_format;

        $scope.$broadcast("changeDataSemantics");

        if ($scope.hasCollector){
        
          // fill filter
          var filter = collector.filter || {};

          if (filter.discard_before || filter.discard_after || filter.region || filter.data_series_id){
            $scope.wizard.filter.message = i18n.__("Remove filter configuration");;
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
            $scope.filter.area = GeoLibs.polygon.read(filter.region);
            if (filter.crop_raster){
              $scope.filter.area.crop_raster = true;
            }
          }
          $scope.filter.area.showCrop = $scope.dataSeries.semantics.data_series_type_name == "GRID";

          if (filter.data_series_id){
            $scope.$emit('updateFilterArea', "3");
            $scope.filter.data_series_id = filter.data_series_id; 
            if (filter.crop_raster){
              $scope.filter.area.crop_raster = true;
            }
          }
        }

        var dataSeriesSemantics = DataSeriesSemanticsService.get({code: $scope.dataSeries.semantics.code});
        // TODO: filter provider type: FTP, HTTP, etc
        $scope.dataProviders = [];
        $scope.providersList.forEach(function(dataProvider) {
          dataSeriesSemantics.data_providers_semantics.forEach(function(demand) {
            if (dataProvider.data_provider_type.id == demand.data_provider_type_id)
              $scope.dataProviders.push(dataProvider);
          });
        });

        if(!$scope.isUpdating)
          if($scope.dataProviders.length > 0)
            $scope.dataSeries.data_provider_id = $scope.dataProviders[0].id;

        $scope.tableFields = [];
	      $scope.tableFieldsDataTable = [];
        if ($scope.dataSeries.semantics.data_series_type_name == "DCP"){
          // building table fields. Check if form is for all ('*')
          if (dataSeriesSemantics.metadata.form.indexOf('*') != -1) {
            // ignore form and make it from properties
            var properties = dataSeriesSemantics.metadata.schema.properties;
            for(var key in properties) {
              if (properties.hasOwnProperty(key)) {
                $scope.tableFields.push(key);
	              $scope.tableFieldsDataTable.push(i18n.__(properties[key].title));
              }
            }
          } else {
            // form is mapped
            for(var i = 0, formLength = dataSeriesSemantics.metadata.form.length; i < formLength; i++) {
              $scope.tableFields.push(dataSeriesSemantics.metadata.form[i].key);
              $scope.tableFieldsDataTable.push(i18n.__(dataSeriesSemantics.metadata.schema.properties[dataSeriesSemantics.metadata.form[i].key].title));
            }
          }
        }

	      $scope.tableFieldsDataTable.push('');

        // fill out
        if ($scope.isUpdating) {
          $scope.wizard.parameters.disabled = false;
          $scope.wizard.parameters.error = false;
          $scope.wizard.general.error = false;
          if ($scope.semantics === globals.enums.DataSeriesType.DCP) {
            // TODO: prepare format as dcp item
            $scope.dcpsObject = {};
            $scope.editedDcps = [];
            $scope.removedDcps = [];
            $scope.duplicatedAliasCounter = {};

            var dcps = [];
            var registersCount = 0;

            for(var i = 0, dataSetsLength = inputDataSeries.dataSets.length; i < dataSetsLength; i++) {
              if(inputDataSeries.dataSets[i].position) {
                var lat;
                var long;
                if(inputDataSeries.dataSets[i].position.type) {
                  // geojson
                  long = inputDataSeries.dataSets[i].position.coordinates[0];
                  lat = inputDataSeries.dataSets[i].position.coordinates[1];
                } else {
                  var first = inputDataSeries.dataSets[i].position.indexOf("(");
                  var firstSpace = inputDataSeries.dataSets[i].position.indexOf(" ", first);
                  lat = parseInt(inputDataSeries.dataSets[i].position.slice(first+1, firstSpace));

                  var last = inputDataSeries.dataSets[i].position.indexOf(")", firstSpace);
                  long = inputDataSeries.dataSets[i].position.slice(firstSpace + 1, last);

                }
                inputDataSeries.dataSets[i].format["latitude"] = lat;
                inputDataSeries.dataSets[i].format["longitude"] = long;
              }
              angular.merge(inputDataSeries.dataSets[i].format, {active: inputDataSeries.dataSets[i].active});

              var dcp = $scope.prepareFormatToForm(inputDataSeries.dataSets[i].format);

              for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
                var key = $scope.dataSeries.semantics.metadata.form[j].key;
                var titleMap = $scope.dataSeries.semantics.metadata.form[j].titleMap;
                var title = $scope.dataSeries.semantics.metadata.schema.properties[key].title;
                var type = $scope.dataSeries.semantics.metadata.schema.properties[key].type;
                var pattern = $scope.dataSeries.semantics.metadata.schema.properties[key].pattern;

                dcp[key + '_pattern'] = pattern;
                dcp[key + '_titleMap'] = titleMap;

                if(dcp[key + '_titleMap'] !== undefined)
                  type = $scope.dataSeries.semantics.metadata.form[j].type;

                dcp = $scope.setHtmlItems(dcp, key, dcp.alias, dcp._id, type);
              }

              $scope.dcpsObject[dcp.alias] = dcp;

              var dcpCopy = Object.assign({}, dcp);
              dcpCopy.removeButton = $scope.getRemoveButton(dcp.alias);

              dcps.push(dcpCopy);

              registersCount++;

              if(registersCount >= 1000) {
                $scope.storageDcps(dcps);
                $scope.addDcpsStorager(dcps);

                registersCount = 0;
                dcps = [];
              }

	            if(inputDataSeries.data_series_semantics.custom_format) {
                $scope.csvFormatData.fields = JSON.parse(inputDataSeries.dataSets[i].format.fields)
                $scope.csvFormatData.header_size = parseInt(inputDataSeries.dataSets[i].format.header_size);
                $scope.csvFormatData.default_type = inputDataSeries.dataSets[i].format.default_type;
                $scope.csvFormatData.convert_all = (inputDataSeries.dataSets[i].format.convert_all == "true");
                $scope.csvFormatData.properties_names_line = parseInt(inputDataSeries.dataSets[i].format.properties_names_line);
              }
            }

            if(registersCount > 0) {
              $scope.storageDcps(dcps);
              $scope.addDcpsStorager(dcps);
            }
          } else {
	          var dataSetFormat = inputDataSeries.dataSets[0].format;
            $scope.model = $scope.prepareFormatToForm(inputDataSeries.dataSets[0].format);
	          if(inputDataSeries.data_series_semantics.custom_format) {
              $scope.csvFormatData.fields = JSON.parse(dataSetFormat.fields)
              $scope.csvFormatData.header_size = parseInt(dataSetFormat.header_size);
              $scope.csvFormatData.default_type = dataSetFormat.default_type;
              $scope.csvFormatData.convert_all = (dataSetFormat.convert_all == "true");
              $scope.csvFormatData.properties_names_line = parseInt(dataSetFormat.properties_names_line);
            }
            $scope.model.temporal = ($scope.model.temporal == 'true' || $scope.model.temporal == true ? true : false);

            if(typeof $scope.model.timezone === "number") {
              $scope.model.timezone = $scope.model.timezone.toString();
            }
          }

          if($scope.hasCollector) {
            $scope.wizard.store.message = i18n.__("Remove store configuration");
            $scope.wizard.store.disabled = false;
            $scope.wizard.store.error = false;
            $scope.advanced.store.disabled = false;
          }
          if(Object.keys($scope.intersection).length > 0) {
            $scope.wizard.intersection.message = i18n.__("Remove intersection configuration");
            $scope.wizard.intersection.disabled = false;
            $scope.advanced.intersection.disabled = false;
          }
        } else {
          $scope.dcpsObject = {};
          $scope.editedDcps = [];
          $scope.removedDcps = [];
          $scope.duplicatedAliasCounter = {};
          $scope.model = {};
          $scope.$broadcast("resetStoragerDataSets");
        }

        var formTranslatorResult = FormTranslator(dataSeriesSemantics.metadata.schema.properties, dataSeriesSemantics.metadata.form, dataSeriesSemantics.metadata.schema.required);

        $scope.schema = {
          type: 'object',
          properties: formTranslatorResult.object,
          required: dataSeriesSemantics.metadata.schema.required
        };

        $scope.form = formTranslatorResult.display;

        $scope.$broadcast('schemaFormRedraw');

        _processParameters();

        $timeout(function() {
          if($scope.tableFields.length > 0)
            $scope.createDataTable();

          $scope.isChecking.value = false;

          if(!$scope.dataSeries.semantics || $scope.dataSeries.semantics.data_format_name != 'POSTGIS') {
            return;
          } else {
            var tableInput = angular.element('#table_name');
            tableInput.attr('list', 'databaseTableList');
          }
        });
      };

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

      /**
       * It defines a TerraMA² MessageBox Service for handling alert box
       *
       * @type {MessageBoxService}
       */
      $scope.MessageBoxService = MessageBoxService;

      /**
       * Helper to reset alert box instance
       */
      $scope.close = function() {
        $scope.MessageBoxService.reset();
      };

      // data series used for intersection
      $scope.dataSeriesList = [];

      // table fields
      $scope.tableFields = [];
      $scope.tableFieldsDataTable = [];

      // injecting state handler in scope
      $scope.stateApp = $state;

      // injecting helper functions in scope
      $scope.capitalizeIt = function(text) {
        return text.charAt(0).toUpperCase() + text.slice(1);
      };
      $scope.isBoolean = function(value) {
        return typeof value === 'boolean';
      };

      //injecting helper functin in store options
      $scope.storeOptions.capitalizeIt = $scope.capitalizeIt;
      $scope.storeOptions.isBoolean = $scope.isBoolean;

      $scope.prepareFormatToForm = function(fmt) {
        var output = {};
        for(var k in fmt) {
          if (fmt.hasOwnProperty(k)) {
            // checking if a number
            if (isNaN(fmt[k]) || fmt[k] == "" || typeof fmt[k] == "boolean") {
              if (k === "active") {
                output[k] = typeof fmt[k] === "string" ? fmt[k] === "true" : fmt[k]; 
              } else {
                output[k] = fmt[k];
              }
            } else {
              output[k] = parseFloat(fmt[k]);
            }
          }
        }
        return output;
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

          if(disabled) {
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

          if (name === "parametersForm" && $scope.countObjectProperties($scope.dcpsObject) > 0) {
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
        {name: i18n.__("Static"), children: []}
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

          if(ds.data_series_semantics.data_series_type_name === globals.enums.DataSeriesType.GRID) {
            ds.isGrid = true;
          } else {
            ds.isGrid = false;
          }
          _helper(0, ds);
        };

        if(ds) {
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

        $scope.dataSeriesGroups[0].children = _helper($scope.dataSeriesGroups[0].children);


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
          var dataProvider = $scope.providersList.filter(function(element) {
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
          url: BASE_URL + "uri/",
          params: params
        });

        httpRequest.then(function(response) {
          $scope.columnsList = response.data.data.map(function(item, index){
            return item.column_name;
          });
          result.resolve(response.data);
        });

        httpRequest.catch(function(response) {
          result.reject(response.data);
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
          return elm.value === attributeValue;
        });

        return found.length === 0;
      };

      $scope.addAttribute = function(form, selected, attributeValue) {
        if (form.$invalid) {
          return;
        }

        if (canAddAttribute(selected, attributeValue, $scope.intersection[selected.id].attributes)) {
          // reset form to the default state
          $scope.intersection[selected.id].attributes.push({value: attributeValue, alias: attributeValue});
          form.$setPristine();
        } else {
          // TODO: throw error message

        }
      };

      $scope.removeAttribute = function(selected, attributeValue) {
        var intersection = $scope.intersection[selected.id];

        intersection.attributes.some(function(attr, index, arr) {
          if (attr.value === attributeValue.value) {
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
          $scope.filter.area = {srid: 4326, showCrop: $scope.filter.area.showCrop};
          delete $scope.filter.data_series_id;
        } 
        else if ($scope.filter.filterArea === $scope.filterTypes.AREA.value){
          delete $scope.filter.data_series_id;
          if ($scope.filter.area){
            $scope.filter.area.srid = 4326;
          } else {
            $scope.filter.area={srid: 4326};
          }
        }
        else {
          $scope.filter.area = {srid: 4326, showCrop: $scope.filter.area.showCrop};
        }
      };

      // storager
      $scope.storeOptions.showStoragerForm = false;
      $scope.storager = {};
      $scope.formStorager = [];
      $scope.modelStorager = {};
      $scope.schemaStorager = {};

      $scope.objectToArray = function(object) {
        return $.map(object, function(value, index) {
          return [value];
        });
      };

      $scope.onStoragerFormatChange = function(viewChange) {
        $scope.storeOptions.showStoragerForm = true;

        $timeout(function() {
          $scope.$broadcast('storagerFormatChange', {
            format: $scope.storager.format,
            viewChange: (viewChange !== undefined ? viewChange : false),
            dcps: (viewChange !== undefined && viewChange ? $scope.objectToArray($scope.dcpsStoragerObject) : $scope.objectToArray($scope.dcpsObject)),
            editedDcps: (viewChange !== undefined && viewChange ? $scope.editedStoragerDcps : []),
            removedDcps: (viewChange !== undefined && viewChange ? $scope.removedStoragerDcps : [])
          });
        }, 1000);
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

      $scope.$watch("dataSeries", function(dSValue) {
        if (dSValue.name && dSValue.semantics && dSValue.data_provider_id){
          $scope.wizard.parameters.disabled = false;
          $scope.wizard.csvFormat.disabled = false;
          if ($scope.dataSeries.semantics.allow_direct_access === false){
            $scope.wizard.store.disabled = false;
            $scope.advanced.store.disabled = false;
            $scope.advanced.store.optional = false;
          }
        }
        else {
          $scope.wizard.parameters.disabled = true;
          $scope.wizard.csvFormat.disabled = true;
          $scope.wizard.store.disabled = true;
          $scope.advanced.store.disabled = true;
          $scope.advanced.store.optional = true;
        }
      }, true);

      // Wizard validations
      $scope.isFirstStepValid = function(obj) {
        isWizardStepValid();
        return $scope.forms.generalDataForm.$valid;
      };

      $scope.isSecondStepValid = function(obj) {
        if($scope.dataSeries.semantics.data_series_type_name === "DCP")
          if($scope.countObjectProperties($scope.dcpsObject) === 0) {
            MessageBoxService.danger(i18n.__("DCP error"), i18n.__("It should have at least one dcp"));
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
        if ($scope.forms.storagerForm && $scope.forms.storagerForm.$valid && $scope.forms.storagerDataForm.$valid && $scope.dataSeries.semantics.data_series_type_name == "GRID"){
          $scope.filter.area.showCrop = true;
        } else {
          $scope.filter.area.showCrop = false;
        }
        return true;
      };

      $scope.goToValidNextStep = function(step) {
        var steps = WizardHandler.wizard().getEnabledSteps();
        var startVerification = false;

        for(var i = 0, stepsLength = steps.length; i < stepsLength; i++) {
          var data = steps[i].wzData || {};
          var name = data.formName || "";

          if(name === step) {
            if(steps[i + 1].canenter !== undefined && !steps[i + 1].canenter) {
              startVerification = true;
              i++;
            } else break;
          } else if(startVerification) {
            if(steps[i].canenter === undefined || steps[i].canenter) {
              WizardHandler.wizard().goTo(i);
              break;
            }
          }
        }
      };

      $scope.goToValidPreviousStep = function(step) {
        var steps = WizardHandler.wizard().getEnabledSteps();
        var startVerification = false;

        for(var i = steps.length - 1; i >= 0; i--) {
          var data = steps[i].wzData || {};
          var name = data.formName || "";

          if(name === step) {
            if(steps[i - 1].canenter !== undefined && !steps[i - 1].canenter) {
              startVerification = true;
              i--;
            } else break;
          } else if(startVerification) {
            if(steps[i].canenter === undefined || steps[i].canenter) {
              WizardHandler.wizard().goTo(i);
              break;
            }
          }
        }
      };

      //. end wizard validations
      $scope.dcpsObject = {};
      $scope.editedDcps = [];
      $scope.removedDcps = [];

      $scope.dcpsStoragerObject = {};
      $scope.editedStoragerDcps = [];
      $scope.removedStoragerDcps = [];

      $scope.duplicatedAliasCounter = {};

      $scope.updatingDcp = false;

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
        data_provider_id: (inputDataSeries.data_provider_id || ""),
        project: configuration.project,
        name: inputName,
        description: inputDataSeries.description,
        access: $scope.hasCollector ? "COLLECT" : "PROCESSING",
        semantics: inputSemantics.code || "",
        active: inputDataSeries.active
      };
      // list data series
      $scope.dataSeriesList = DataSeriesService.list();
      $scope.staticDataSeriesList = [];

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
            $scope.staticDataSeriesList.push(dSeries);
            break;
          default:
            break;
        }
      });

      if ($scope.isUpdating) {
        if (!$scope.hasProjectPermission){
          MessageBoxService.danger(i18n.__("Permission"), i18n.__("You can not edit this data series. He belongs to a protected project!"));
        }
        // setting intersection values
        var collector = configuration.collector || {};
        var intersection = collector.intersection || [];

        if (intersection.length !== 0) {
          var attrs = [];
          intersection.forEach(function(element) {
            attrs.push(element.attribute);
            $scope.dataSeriesList.some(function(ds) {
              if (ds.id === element.dataseries_id) {
                $scope.addDataSeries(ds);

                var target = $scope.intersection[ds.id];
                target.selected = true;

                if (canAddAttribute(target.data_series, element.attribute, target.attributes)) {
                  target.attributes.push({value: element.attribute, alias: element.alias});
                }

                return true;
              }
            });
          }); 
        }
      }

      var dataSeriesSemantics = DataSeriesSemanticsService.list();
      $scope.dataSeriesSemantics = dataSeriesSemantics;

      $scope.dataSeriesSemantics.sort(function(a, b) {
        if(a.name < b.name) return -1;
        if(a.name > b.name) return 1;
        return 0;
      });

      if ($scope.dataSeries.semantics) {
        dataSeriesSemantics.forEach(function(semantics) {
          if (semantics.code === $scope.dataSeries.semantics) {
            $scope.dataSeries.semantics = semantics;
            $scope.onDataSemanticsChange();
          }
        })
      }

      $scope.i18n = i18n;
      $scope.isWizard = $scope.stateApp.current.name === "wizard";
      $scope.projection = "";

      // change form: advanced or wizard
      $scope.onFormView = function() {
        $scope.$broadcast("saveStoragerData");

        $scope.isWizard = !$scope.isWizard;

        // fixing the datatable
        $timeout(function() {
          $scope.createDataTable();
        }, 1000);

        if($scope.isUpdating) {
          // fixing storager loading
          $timeout(function() {
            $scope.onStoragerFormatChange(true);
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
            $scope.wizard.intersection.message = i18n.__("Must have a valid store values to create an intersection");
            $scope.advanced.intersection.message = i18n.__("Must have a valid store values to create an intersection");
          } else {
            $scope.advanced.intersection.message = i18n.__("Add intersection configuration");
            $scope.wizard.intersection.message = i18n.__("Add intersection configuration");
            $scope.dataSeries.access = 'COLLECT';
          }
        }
      });

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
          url: BASE_URL + "uri/",
          params: params
        });

        httpRequest.then(function(response) {
          $scope.tableList = response.data.data.map(function(item, index){
            return item.table_name;
          });
          result.resolve(response.data);
        });

        httpRequest.catch(function(response) {
          result.reject(response.data);
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

        $window.location.href = BASE_URL + "configuration/providers/new?redirectTo=" + url + "&" + $httpParamSerializer(output);
      };

      $scope.removePcd = function(alias) {
        if($scope.dcpsObject[alias] !== undefined) {
          $scope.$broadcast("dcpOperation", {action: "remove", dcp: Object.assign({}, $scope.dcpsObject[alias])});

          $http.post(BASE_URL + "configuration/dynamic/dataseries/removeStoredDcp", {
            key: storedDcpsKey,
            alias: alias
          }).then(function(result) {
            reloadData();
          }, function(error) {
            console.log("Err in removing dcp");
          });

          $scope.removedDcps.push($scope.dcpsObject[alias]._id);
          delete $scope.dcpsObject[alias];
        }
      };

      var isValidParametersForm = function(form) {
        $scope.$broadcast('schemaFormValidate');

        return form.$valid;
      };

      $scope.$on("fieldHasError", function(event, args) {
        $scope.fieldHasError(args.value, args.type, args.pattern, args.titleMap, args.allowEmptyValue);
      });

      $scope.fieldHasError = function(value, type, pattern, titleMap, allowEmptyValue) {
        var error = false;

        switch(type) {
          case "number":
            error = isNaN(value);
            break;
          case "boolean":
            error = (typeof value !== "boolean") && value !== "true" && value !== "false";
            break;
          case "select":
            error = true;

            for(var i = 0, titleMapLength = titleMap.length; i < titleMapLength; i++) {
              if(titleMap[i].value === value) {
                error = false;
                break;
              }
            }

            break;
          default:
            error = false;
        }

        if(!error && pattern !== undefined) {
          var regex = new RegExp(pattern);
          var error = !regex.test(value) ? (allowEmptyValue && value === "" ? false : true) : false;
        }

        return error;
      };

      $scope.isAliasValid = function(value, dcpsObject) {
        for(key in dcpsObject) {
          if(dcpsObject.hasOwnProperty(key)) {
            if(dcpsObject[key].alias == value)
              return false;
          }
        }

        return true;
      };

      $scope.validateFieldEdition = function(value, type, alias, key) {
        if($scope.fieldHasError(value, type, $scope.dcpsObject[alias][key + '_pattern'], $scope.dcpsObject[alias][key + '_titleMap'], $scope.dataSeries.semantics.metadata.schema.properties[key].allowEmptyValue))
          return i18n.__("Invalid value");
        else
          return null;
      };

      $scope.insertEditedDcp = function(id) {
        var insertDcp = true;

        for(var i = 0, editedDcpsLength = $scope.editedDcps.length; i < editedDcpsLength; i++) {
          if($scope.editedDcps[i] == id) {
            insertDcp = false;
            break;
          }
        }

        if(insertDcp) $scope.editedDcps.push(id);
      };

      $scope.addDcp = function() {
        if(isValidParametersForm($scope.forms.parametersForm)) {
          var data = Object.assign({}, $scope.model);
          if($scope.isAliasValid(data.alias, $scope.dcpsObject)) {
            data._id = UniqueNumber();
            var alias = data.alias;

            for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
              var key = $scope.dataSeries.semantics.metadata.form[j].key;
              var type = $scope.dataSeries.semantics.metadata.form[j].schema.type;
              data[key + '_pattern'] = $scope.dataSeries.semantics.metadata.form[j].schema.pattern;
              data[key + '_titleMap'] = $scope.dataSeries.semantics.metadata.form[j].titleMap;

              if(data[key + '_titleMap'] !== undefined)
                type = $scope.dataSeries.semantics.metadata.form[j].type;

              data = $scope.setHtmlItems(data, key, alias, data._id, type);
            }

            $scope.dcpsObject[alias] = Object.assign({}, data);
            $scope.$broadcast("dcpOperation", { action: "add", dcp: data, storageData: true, reloadDataStore: false });
            $scope.model = {active: true};

            var dcpCopy = Object.assign({}, data);
            dcpCopy.removeButton = $scope.getRemoveButton(dcpCopy.alias);

            $scope.storageDcps([dcpCopy]);

            if($scope.isUpdating)
              $scope.insertEditedDcp(dcpCopy._id);

            // reset form to do not display feedback class
            $scope.forms.parametersForm.$setPristine();

            reloadData();
          } else {
            MessageBoxService.danger(i18n.__("Field error"), i18n.__("The Alias has to be unique"));
          }
        }
      };

      Object.equals = function( x, y ) {
        if ( x === y ) return true;
          // if both x and y are null or undefined and exactly the same

        if ( ! ( x instanceof Object ) || ! ( y instanceof Object ) ) return false;
          // if they are not strictly equal, they both need to be Objects

        if ( x.constructor !== y.constructor ) return false;
          // they must have the exact same prototype chain, the closest we can do is
          // test there constructor.

        for ( var p in x ) {
          if ( ! x.hasOwnProperty( p ) ) continue;
            // other properties were tested using x.constructor === y.constructor

          if ( ! y.hasOwnProperty( p ) ) return false;
            // allows to compare x[ p ] and y[ p ] when set to undefined

          if ( x[ p ] === y[ p ] ) continue;
            // if they have the same strict value or identity then they are equal

          if ( typeof( x[ p ] ) !== "object" ) return false;
            // Numbers, Strings, Functions, Booleans must be strictly equal

          if ( ! Object.equals( x[ p ],  y[ p ] ) ) return false;
            // Objects and Arrays must be tested recursively
        }

        for ( p in y ) {
          if ( y.hasOwnProperty( p ) && ! x.hasOwnProperty( p ) ) return false;
            // allows x[ p ] to be set to undefined
        }
        return true;
      }

      $scope.$watch("dcpsObject", function(newVal, oldVal) {
        var newValLength = $scope.countObjectProperties(newVal);
        var oldValLength = $scope.countObjectProperties(oldVal);

        if(newVal && newValLength > 0) {
          //checking if is editing
          if(newValLength === oldValLength) {
            for(var property in newVal) {
              if(newVal.hasOwnProperty(property) && oldVal.hasOwnProperty(property)) {
                if(!Object.equals(JSON.parse(angular.toJson(newVal[property])), JSON.parse(angular.toJson(oldVal[property])))) {
                  if(newVal[property].alias != oldVal[property].alias) {
                    var newAlias = newVal[property].alias;
                    var oldAlias = oldVal[property].alias;

                    $scope.dcpsObject[newAlias] = $scope.dcpsObject[property];
                    delete $scope.dcpsObject[property];

                    for(var j = 0, fieldsLength = $scope.dataSeries.semantics.metadata.form.length; j < fieldsLength; j++) {
                      var key = $scope.dataSeries.semantics.metadata.form[j].key;
                      var type = $scope.dataSeries.semantics.metadata.schema.properties[key].type;

                      $scope.dcpsObject[newAlias] = $scope.setHtmlItems($scope.dcpsObject[newAlias], key, $scope.dcpsObject[newAlias].alias, $scope.dcpsObject[newAlias]._id, type);
                    }

                    $scope.dcpsObject[newAlias].removeButton = $scope.getRemoveButton(newAlias);

                    var dataToSend = Object.assign({}, $scope.dcpsObject[newAlias]);

                    $http.post(BASE_URL + "configuration/dynamic/dataseries/updateDcp", {
                      key: storedDcpsKey,
                      oldAlias: oldAlias,
                      dcp: dataToSend
                    }).then(function(result) {
                      reloadData();
                    }, function(error) {
                      console.log("Err in editing dcp");
                    });

                    newVal[newAlias].oldAlias = oldAlias;
                    newVal[newAlias].newAlias = newAlias;

                    property = newAlias;
                  }

                  $scope.$broadcast("dcpOperation", { action: "edit", dcp: JSON.parse(angular.toJson(newVal[property])) });
                }
              }
            }
          }
        }
      }, true);

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
          request = DataSeriesService.update(configuration.dataSeries.input.id, data);
        } else {
          request = DataSeriesService.create(data);
        }

        request.then(function(data) {
          if($scope.semantics === globals.enums.DataSeriesType.DCP) {
            $scope.$broadcast("deleteDcpsStoreKey");

            $http.post(BASE_URL + "configuration/dynamic/dataseries/deleteDcpsKey", {
              key: storedDcpsKey
            }).then(function(result) {
              $window.location.href = BASE_URL + "configuration/" + configuration.dataSeriesType + "/dataseries?token=" + (data.token || data.data.token);
            }, function(error) {
              console.log("Err in deleting key");
            });
          } else {
            $window.location.href = BASE_URL + "configuration/" + configuration.dataSeriesType + "/dataseries?token=" + (data.token || data.data.token);
          }
        }).catch(function(err) {
          $scope.isChecking.value = false;
          var errMessage = err.message || err.data.message;
          MessageBoxService.danger(i18n.__("Data Registration"), i18n.__(errMessage));
        });
      };

      var confirmNoStorager = function(object) {
        $scope.isChecking.value = true;

        _sendRequest(object);
      };

      $scope.saveStoragerData = function(dcps, editedDcps, removedDcps) {
        $scope.dcpsStoragerObject = dcps;
        $scope.editedStoragerDcps = editedDcps;
        $scope.removedStoragerDcps = removedDcps;
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
            if(dSetObject.hasOwnProperty(key) && key.toLowerCase() !== "id" && key.substr(key.length - 5) != "_html" && key.substr(key.length - 8) != "_pattern" && key.substr(key.length - 9) != "_titleMap" && key != "removeButton" && key != "oldAlias" && key != "newAlias")
              format_[key] = dSetObject[key].toString();
              if(key.startsWith("output_")) {
                format_[key.replace("output_", "")] = dSetObject[key];
              }
          }

          return format_;
        };

        var out;
        if (dSets instanceof Array) {
          // setting to active
          var dSetsLocal = [];
          var tempEditedDcps = [];

          for(var i = 0, dSetsLength = dSets.length; i < dSetsLength; i++) {
            var output_timestamp_property_field = dataObject.dataSeries.dataSets[0].format.output_timestamp_property;
            if (output_timestamp_property_field){
              dSets[i].timestamp_property = output_timestamp_property_field
            }

            var output_geometry_property_field = dataObject.dataSeries.dataSets[0].format.output_geometry_property;
            if (output_geometry_property_field){
              dSets[i].geometry_property = output_geometry_property_field;
            }
            

            var outputDcp = {
              active: dSets[i].active,
              format: _makeFormat(dSets[i])
            };
            delete dSets[i].active;

            if ($scope.dataSeries.semantics.data_format_name !== "POSTGIS") {
              outputDcp.position = GeoLibs.point.build({x: parseFloat(dSets[i].longitude), y: parseFloat(dSets[i].latitude), srid: dSets[i].projection});
            }
            dSetsLocal.push(outputDcp);

            for(var j = 0, editedDcpsLength = values.editedDcps.length; j < editedDcpsLength; j++) {
              if(values.editedDcps[j] == outputDcp.format._id) {
                tempEditedDcps.push(outputDcp);
                break;
              }
            }
          }

          values.editedDcps = tempEditedDcps;

          out = dSetsLocal;
        } else {
          var fmt = angular.merge({}, dSets);
          angular.merge(fmt, dSemantics.metadata.metadata);
          if ($scope.custom_format){
            var output_timestamp_property_field = dataObject.dataSeries.dataSets[0].format.output_timestamp_property;
            if (output_timestamp_property_field){
              fmt.timestamp_property = output_timestamp_property_field
            }

            var output_geometry_property_field = dataObject.dataSeries.dataSets[0].format.output_geometry_property;
            if (output_geometry_property_field){
              fmt.geometry_property = output_geometry_property_field;
            }
          }

          dSets.format = _makeFormat(fmt);
          dSets.active = true,
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
                attribute: attribute.value,
                alias: attribute.alias,
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
          editedDcps: values.editedDcps,
          removedDcps: values.removedDcps,
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
            var tempEditedDcps = [];

            for(var dcpKey in $scope.dcpsObject) {
              var format = {};
              for(var key in $scope.dcpsObject[dcpKey]) {
                if($scope.dcpsObject[dcpKey].hasOwnProperty(key) && key.substr(key.length - 5) != "_html" && key.substr(key.length - 8) != "_pattern" && key.substr(key.length - 9) != "_titleMap" && key != "removeButton" && key != "oldAlias" && key != "newAlias" && key != "latitude" && key != "longitude" && key != "active")
                  format[key] = $scope.dcpsObject[dcpKey][key].toString();
              }
              angular.merge(format, semantics.metadata.metadata);
              if (semantics.custom_format){
                format = Object.assign(format, $scope.csvFormatData);

                var output_timestamp_property_field = getAliasFromCsvFields("DATETIME", $scope.csvFormatData.fields);
                if (output_timestamp_property_field){
                  format.output_timestamp_property = output_timestamp_property_field;
                }

                var output_geometry_property_field = getAliasFromCsvFields("GEOMETRY_POINT", $scope.csvFormatData.fields);
                if (output_geometry_property_field){
                  format.output_geometry_property = output_geometry_property_field;
                }

                var stringFields = angular.toJson($scope.csvFormatData.fields);
                format.fields = stringFields;
              }
              var dataSetStructure = {
                active: $scope.dcpsObject[dcpKey].active,//$scope.dataSeries.active,
                format: format,
                position: GeoLibs.point.build({x: parseFloat($scope.dcpsObject[dcpKey].longitude), y: parseFloat($scope.dcpsObject[dcpKey].latitude), srid: $scope.dcpsObject[dcpKey].projection})
              };

              dataToSend.dataSets.push(dataSetStructure);

              for(var j = 0, editedDcpsLength = $scope.editedDcps.length; j < editedDcpsLength; j++) {
                if($scope.editedDcps[j] == $scope.dcpsObject[dcpKey]._id) {
                  tempEditedDcps.push(dataSetStructure);
                  break;
                }
              }
            }

            dataToSend.editedDcps = tempEditedDcps;
            dataToSend.removedDcps = $scope.removedDcps;

            break;
          case "OCCURRENCE":
          case "GRID":
          case "GEOMETRIC_OBJECT":
            var format = Object.assign({}, $scope.model);
            angular.merge(format, semantics.metadata.metadata);
            if (semantics.custom_format){
              format = Object.assign(format, $scope.csvFormatData);
              format.output_timestamp_property = getAliasFromCsvFields("DATETIME", $scope.csvFormatData.fields);
              format.output_geometry_property = getAliasFromCsvFields("GEOMETRY_POINT", $scope.csvFormatData.fields);
              var stringFields = angular.toJson($scope.csvFormatData.fields);
              format.fields = stringFields;
            }

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
          filterValues.region = GeoLibs.polygon.build($scope.filter.area || {});
        }
        else if ($scope.filter.filterArea === $scope.filterTypes.STATIC_DATA.value){
          filterValues.data_series_id = $scope.filter.data_series_id;
          if ($scope.filter.area.crop_raster){
            filterValues.crop_raster = true;
          }
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

        return {
          dataSeries: dataToSend,
          schedule: scheduleValues,
          filter: filterValues
        };
      };

      var getAliasFromCsvFields = function(fieldType, fields){
        var fieldAlias;
        fields.forEach(function(field){
          if (field.type == fieldType){
            fieldAlias = field.alias;
            return;
          }
        });
        return fieldAlias;
      }

      $scope.save = function(shouldRun) {
        $scope.shouldRun = shouldRun;
        $scope.extraProperties = {};
        $scope.$broadcast('formFieldValidation');

        if($scope.isWizard) {
          isWizardStepValid();
        }

        if (!$scope.hasProjectPermission && $scope.isUpdating){
          MessageBoxService.danger(i18n.__("Permission"), i18n.__("You can not edit this data series. He belongs to a protected project!"));
          return;
        }

        if (!canSave){
          MessageBoxService.danger(i18n.__("Data Registration"), i18n.__(serviceOfflineMessage));
          return;
        }

        if($scope.forms.generalDataForm.$invalid) {
          MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("There are invalid fields on form"));
          return;
        }
        // checking parameters form (semantics) is invalid
        if($scope.countObjectProperties($scope.dcpsObject) === 0 && !isValidParametersForm($scope.forms.parametersForm)) {
          MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("There are invalid fields on form"));
          return;
        }

        if($scope.isDynamic) {
          if(angular.element('form[name="scheduleForm"]').scope()){ 
            var scheduleForm = angular.element('form[name="scheduleForm"]').scope()['scheduleForm'];
            if(scheduleForm.$invalid) {
              MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("There are invalid fields on form"));
              return;
            }
          }
        }

        if ($scope.filter.filterArea == $scope.filterTypes.AREA.value) {
          if (FilterForm.boundedForm.$invalid){
            MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("Invalid filter area"));
            return;
          }
        }

        if ($scope.filter.filterArea == $scope.filterTypes.STATIC_DATA.value) {
          var staticDataSeriesForm;
          if ($scope.isWizard){
            staticDataSeriesForm = angular.element('form[name="staticDataSeriesForm"]').scope()['staticDataSeriesForm'];
          } else {
            staticDataSeriesForm = angular.element('form[name="filterForm"]').scope()['filterForm'];
          }
          if (staticDataSeriesForm.$invalid){
            MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("Invalid filter data series"));
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
                  MessageBoxService.danger(i18n.__("Data Registration"), i18n.__("Invalid intersection. Static data series must have at least a attribute."));
                  return;
                }
              }
            }
          }
        }

        if ($scope.dataSeries.access == 'COLLECT') {
          $scope.isChecking.value = true;

          // getting values from another controller
          $scope.$broadcast("requestStorageValues");
        } else {
          if ($scope.dataSeries.semantics.data_format_name === globals.enums.DataSeriesFormat.GRADS) {
            MessageBoxService.danger(i18n.__("Data Series Registration"), i18n.__("Unconfigured GraDs Data Series storage"));
            return;
          }

          var dataObject = _save();

          if ($scope.isDynamic) {
            //  display alert box
            var extraProperties = {
              object: {
                dataToSend: dataObject.dataSeries,
                scheduleValues: dataObject.schedule,
                filterValues: dataObject.filter
              },
              confirmButtonFn: confirmNoStorager
            };

            MessageBoxService.warning(i18n.__("Data Series"), i18n.__("Note: No storager configuration, this data will be accessed when needed."), extraProperties);
          } else {
            $scope.isChecking.value = true;

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
    })
  }
    RegisterDataSeries.$inject = ["$scope", "$http", "i18n", "$window", "$state", "$httpParamSerializer", "DataSeriesSemanticsService", "DataProviderService", "DataSeriesService", "Service", "$timeout", "WizardHandler", "UniqueNumber", "FilterForm", "MessageBoxService", "$q", "GeoLibs", "$compile", "DateParser", "FormTranslator", "Socket"];

    return { "RegisterDataSeries": RegisterDataSeries};
})
