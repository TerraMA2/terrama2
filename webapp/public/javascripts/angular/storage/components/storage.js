define([], function () {
  /**
   * Controller of StorageComponent
   *
   * @class StorageController
   */
  class StorageController {
    // Angular Dependency Injector

    /**
     * @param {angular.$scope} $scope Component Scope
     * @param {angular.i18n} i18n Internationalization module
     * @param {DataSeriesService} DataSeries TerraMA2 Data Series DAO
     */
    constructor($scope, i18n, DataSeries, Storage, Service, Provider, $q, $window, MessageBox) {
      this.$scope = $scope;
      this.i18n = i18n;
      $scope.i18n = i18n;
      $scope.css = { boxType: "box-solid" };
      this.DataSeries = DataSeries;
      this.Service = Service;
      this.Storage = Storage;
      this.Provider = Provider;
      this.isLoading = true;
      this.$q = $q;
      this.$window = $window;
      this.MessageBox = MessageBox;

      this.scheduleOptions = {
        showHistoricalOption: false,
        showAutomaticOption: false
      };

      // Initialize component
      this.init()
        .then(() => console.log('done'))
        .finally(() => {
          this.isLoading = false;
        });
    }

    /** Initialize Component services */
    init() {
      const { DataSeries, Provider, Service, Storage } = this;

      const defer = this.$q.defer();

      /**
       * Wrap of initializer module
       *
       * It uses Angular promise to fix context execution
       */
      const wrapExecutionWithAngularPromise = async () => {
        try {
          await DataSeries.init({ schema: 'all' });
          await Service.init();
          await Provider.init();

          this.isUpdate = !!this.storageId;

          if (this.isUpdate) {
            this.storage = await Storage.get(this.storageId);

            if (this.storage.schedule) {
              this.$scope.$broadcast("updateSchedule", this.storage.schedule);
            } else {
              // default is manual
              this.storage.schedule = { scheduleType: "3" }
            }

            // Trigger change
            this.changeDataSeries();
            this.changeDataProvider();

            if (this.storage.uriObject) {
              if (this.storage.uriObject.protocol === "FILE") {
                // Parse URI
                const providerURI = this.selectedProvider.uri.replace(/([^:]\/\/)\/+/g, "$1");
                const storageURI = this.storage.uri.replace(/([^:]\/\/)\/+/g, "$1");

                let parsedPath = storageURI.replace(providerURI, '');

                if (parsedPath[0] === '/')
                  parsedPath = parsedPath.substr(1);

                this.storage.path = parsedPath;
              } else {
                const path = this.storage.uriObject.pathname;
                this.storage.path = path.slice(path.lastIndexOf("/")+1, path.length);
              }
            }
          }
        } catch (err) {
          return defer.reject(err);
        }

        return defer.resolve();
      };

      wrapExecutionWithAngularPromise();

      return defer.promise;
    }

    closeBox() {
      this.MessageBox.reset();
    }

    /**
     * Retrieves all dynamic data series
     *
     * @returns {any[]}
     */
    getDynamicDataSeries() {
      return this.DataSeries.getDynamicDataSeries({});
    }

    /** Retrieves all Storage Services */
    getStorageServices() {
      return this.Service.list({ service_type_id: this.Service.types.STORAGE });
    }

    /** Filter dynamic data series in order to retrieve POSTGIS type */
    getPostGISDataSeries() {
      return this.DataSeries.getDynamicDataSeries({ data_series_semantics: { data_format_name: 'POSTGIS' } });
    }

    /**
     * Get the respective data series icon in order to fill combobox
     *
     * @param {any} dataSeries Data series scope
     */
    getDataSeriesIcon(dataSeries) {
      if (!dataSeries)
        return "";

      return this.DataSeries.getIcon(dataSeries);
    }

    /** Triggered when data series combobox changes. Selects the current data series into scope */
    changeDataSeries() {
      this.selectedDataSeries = this.getDynamicDataSeries().find(ds => ds.id === this.storage.data_series_id);
    }

    /** Triggered when data provider combobox changes. Selects the current provider into scope */
    changeDataProvider() {
      this.selectedProvider = this.getDataProviders().find(provider => provider.id === this.storage.data_provider_id);
    }


    getDataProviders() {
      const { selectedDataSeries } = this;

      if (!selectedDataSeries)
        return;

      const providerId = selectedDataSeries.data_provider_id;
      const provider = this.Provider.list({ id: providerId })[0];

      return this.Provider.list({ data_provider_type: { id: provider.data_provider_type.id } });
    }

    /** Check if data provider is FILE */
    isDataProviderFile() {
      const { selectedProvider } = this;

      if (!selectedProvider)
        return false;

      const { id } = selectedProvider.data_provider_type;

      return id === 1;
    }

    /**
     * Validate form and format model concept
     *
     * @returns {boolean}
     */
    validate() {
      return this.storageForm.$valid && (this.parametersForm ? this.parametersForm.$valid : true);
    }

    /** Trigger when cancel button clicked. It redirects to the previous page */
    onCancelClicked() {
      this.$window.history.back();
    }

    async save() {
      if (!this.validate()) {
        this.MessageBox.danger(this.i18n.__('Error'), this.i18n.__('There are invalid fields on form'))
        return;
      }

      if (this.storage.backup)
        this.storage.uri = this.selectedProvider.uri + "/" + this.storage.path;

      const { Storage } = this;

      if (this.storage.schedule) {
        this.storage.schedule_type = this.storage.schedule.scheduleType;
        switch(this.storage.schedule.scheduleHandler) {
          case "seconds":
          case "minutes":
          case "hours":
            this.storage.schedule.frequency_unit = this.storage.schedule.scheduleHandler;
            this.storage.schedule.frequency_start_time = this.storage.schedule.frequency_start_time ? moment(this.storage.schedule.frequency_start_time).format("HH:mm:ssZ") : "";
            break;
          case "weeks":
          case "monthly":
          case "yearly":
            // todo: verify
            const dt = this.storage.schedule.schedule_time;
            this.storage.schedule.schedule_unit = this.storage.schedule.scheduleHandler;
            this.storage.schedule.schedule_time = moment(dt).format("HH:mm:ss");
            break;

          default:
            if (this.storage.schedule.scheduleType == "4"){
              this.storage.schedule.data_ids = [self.view.data_series_id];
            }
            break;
        }
      }

      try {
        if (this.isUpdate) {
          this.storage.schedule_type = this.storage.scheduleType;
          await Storage.update(this.storageId, this.storage);
        } else {
          await Storage.save(this.storage);
        }

        this.$window.location = `${BASE_URL}administration/storages`;
      } catch (err) {
        console.error(err);

        this.MessageBox.danger(this.i18n.__('Error'), err.message);
      }
    }
  }
  // Angular Inject dependencies
  StorageController.$inject = [
    '$scope',
    'i18n',
    'DataSeriesService',
    'StorageService',
    'Service',
    'DataProviderService',
    '$q',
    '$window',
    'MessageBoxService'
  ];

  const storageComponent = {
    bindings: {
      storageId: '='
    },
    controller: StorageController,
    templateUrl: BASE_URL + "dist/templates/storage/templates/storage-component.html"
  };

  return storageComponent;
});
