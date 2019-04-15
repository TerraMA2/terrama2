define([], function () {
  /**
   * Controller of StoragerComponent
   *
   * @class StoragerController
   */
  class StoragerController {
    // Angular Dependency Injector

    /**
     * @param {angular.$scope} $scope Component Scope
     * @param {angular.i18n} i18n Internationalization module
     * @param {DataSeriesService} DataSeries TerraMA2 Data Series DAO
     */
    constructor($scope, i18n, DataSeries, Storage, Service, Provider, $q) {
      this.$scope = $scope;
      $scope.i18n = i18n;
      $scope.css = { boxType: "box-solid" };
      this.DataSeries = DataSeries;
      this.Service = Service;
      this.Storage = Storage;
      this.Provider = Provider;
      this.isLoading = true;
      this.$q = $q;

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

      const wrapExecutionWithAngularPromise = async () => {
        try {
          await DataSeries.init({ schema: 'all' });
          await Service.init();
          await Provider.init();

          this.isUpdate = !!this.storageId;

          if (this.isUpdate) {
            this.storage = await Storage.get(this.storageId);
          }
        } catch (err) {
          return defer.reject(err);
        }

        return defer.resolve();
      };

      wrapExecutionWithAngularPromise();

      return defer.promise;
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
    getStoragerServices() {
      return this.Service.list({ service_type_id: this.Service.types.STORAGE });
    }

    /** Filter dynamic data series in order to retrieve POSTGIS type */
    getPostGISDataSeries() {
      return this.DataSeries.getDynamicDataSeries({ data_series_semantics: { data_format_name: 'POSTGIS' } });
    }

    getDataSeriesIcon(dataSeries) {
      if (!dataSeries)
        return "";

      return this.DataSeries.getIcon(dataSeries);
    }

    changeDataSeries() {
      this.selectedDataSeries = this.getDynamicDataSeries().find(ds => ds.id === this.storage.data_series_id);
    }

    changeDataProvider() {
      const { selectedProvider } = this;

      if (!selectedProvider)
        return;

      this.storage.data_provider_id = selectedProvider.id;
    }

    getDataProviders() {
      const { selectedDataSeries } = this;

      const providerId = selectedDataSeries.data_provider_id;
      const provider = this.Provider.list({ id: providerId })[0];

      return this.Provider.list({ data_provider_type: { id: provider.data_provider_type.id } });
    }

    isDataProviderFile() {
      const { selectedProvider } = this;

      if (!selectedProvider)
        return false;

      const { id } = selectedProvider.data_provider_type;

      return id === 1;
    }

    /**
     * Validate form
     *
     * @returns {boolean}
     */
    validate() {
      return this.storageForm.$valid;
    }

    async save() {
      if (!this.validate()) {
        return;
      }

      this.storage.uri = this.selectedProvider.uri + "/" + this.storage.path;

      const { Storage } = this;
      // console.log(`Form validation: ${this.form.$valid}`, this.storage);

      try {
        await Storage.save(this.storage);
      } catch (err) {
        console.error(err);
      }
    }
  }

  StoragerController.$inject = [
    '$scope',
    'i18n',
    'DataSeriesService',
    'StorageService',
    'Service',
    'DataProviderService',
    '$q'
  ];

  const storageComponent = {
    bindings: {
      storageId: '='
    },
    controller: StoragerController,
    templateUrl: BASE_URL + "dist/templates/storager/templates/storage-component.html"
  };

  return storageComponent;
});