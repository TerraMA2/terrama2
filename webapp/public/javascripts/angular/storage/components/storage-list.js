define([], () => {
  class StorageListController {
    constructor(i18n, Storage, MessageBox, Socket, Service, DataSeries) {
      this.i18n = i18n;
      this.Storage = Storage;
      this.MessageBox = MessageBox;
      this.Socket = Socket;
      this.Service = Service;
      this.DataSeries = DataSeries;

      this.init();

      this.fields = [{key: 'name', as: i18n.__("Name")}, {key: "description", as: i18n.__("Description")}];
      this.linkToAdd = `${BASE_URL}administration/storages/new`;
      this.link = (object) => {
        return `${BASE_URL}administration/storages/${object.id}`;
      };

      this.extra = {
        canRemove: this.hasProjectPermission || configuration.hasProjectPermission,
        canRun: this.canRun,
        showRunButton: true,
        run: this.run.bind(this)
      }
    }

    /** Close alert box handler */
    close() {
      this.MessageBox.reset();
    }

    /** Initializes Storage List component, loading dependencies services in memory */
    async init() {
      await Promise.all([
        this.Storage.init(),
        this.Service.init({ type: this.Service.types.STORAGE }),
        this.DataSeries.init(),
      ])

      this.configureSocketListeners();
    }

    configureSocketListeners() {
      const { Socket } = this;

      Socket.on('errorResponse', this.onErrorResponse.bind(this));
      Socket.on('runResponse', () => {
        const message = this.i18n.__("The process was started successfully");
        this.MessageBox.success(this.i18n.__(`Storage`), message);
      });
    }

    onStatusResponse() {
      console.log(arguments);
    }

    async onErrorResponse(error) {
      const { message } = error;

      this.MessageBox.danger(this.i18n.__('Process'), message);
    }

    canRun() {
      return true;
    }

    remove(object) {
      return `${BASE_URL}api/storages/${object.id}`;
    }

    /**
     * Tries to dispatch the execution of storage
     *
     * @param {any} object Storage to run
     */
    async run(object) {
      const { DataSeries, Service, Socket } = this;

      // Make sure service exists on server side before run
      const storageService = await Service.get(object.service_instance_id);

      // Make sure Data Series is inactive to run Storager
      //const storageDataSeries = DataSeries.list({ id: object.data_series_id })[0];

      //if (!storageDataSeries.active) {
        return Socket.emit('run', { ids: [ object.id ], service_instance: storageService.id });
      //}

      //this.MessageBox.danger(this.i18n.__('Process'), this.i18n.__(`The storage "${object.name}" can't be executed due data series "${storageDataSeries.name}" is active. Make sure the data series is disabled in order to execute Storager.`))
    }
  }


  StorageListController.$inject = ['i18n', 'StorageService', 'MessageBoxService', 'Socket', 'Service', 'DataSeriesService'];

  const storageListComponent = {
    bindings: {
      hasProjectPermission: '<'
    },
    controller: StorageListController,
    template: `
      <terrama2-box title="$ctrl.i18n.__('Storage')" helper="'${BASE_URL}helper/storages.html'">
        <div class="row">
          <div class="col-md-12">
            <div class="col-md-12">
              <terrama2-alert-box2 handler="$ctrl.MessageBox.alertBox" close="$ctrl.close()"></terrama2-alert-box2>
            </div>

            <terrama2-table fields="$ctrl.fields" model="$ctrl.Storage.list()" icon="$ctrl.iconFn" link="$ctrl.link" status-change-link="$ctrl.statusChangeLink"
                          icon-properties="$ctrl.iconProperties" link-to-add="$ctrl.linkToAdd"
                          remove="$ctrl.remove(object)" extra="$ctrl.extra" order-by="$ctrl.orderBydata">
            </terrama2-table>
          </div>
        </div>
      </terrama2-box>
    `
  }

  return storageListComponent;
});
