define([], () => {
  class StorageListController {
    constructor(i18n, Storage, MessageBox, Socket, Service) {
      this.i18n = i18n;
      this.Storage = Storage;
      this.MessageBox = MessageBox;
      this.Socket = Socket;
      this.Service = Service;

      this.init();

      this.fields = [{key: 'name', as: i18n.__("Name")}, {key: "description", as: i18n.__("Description")}];
      this.linkToAdd = `${BASE_URL}configuration/storages/new`;
      this.link = (object) => {
        return `${BASE_URL}configuration/storages/${object.id}`;
      };

      this.extra = {
        canRemove: this.hasProjectPermission || configuration.hasProjectPermission,
        canRun: this.canRun,
        showRunButton: true,
        run: this.run.bind(this)
      }
    }

    close() {
      this.MessageBox.reset();
    }

    async init() {
      await this.Storage.init();
      await this.Service.init({ type: this.Service.types.STORAGE });

      this.configureSocketListeners();
    }

    configureSocketListeners() {
      const { Socket } = this;

      Socket.on('errorResponse', this.onErrorResponse.bind(this));
      Socket.on('runResponse', () => {
        const message = i18n.__("The process was started successfully");
        this.MessageBox.success(i18n.__(`Storage`), message);
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

    async run(object) {
      const { Service, Socket } = this;
  
      // Make sure service exists on server side before run
      const storageService = await Service.get(object.service_instance_id);
  
      Socket.emit('run', { ids: [ object.id ], service_instance: storageService.id });
    }
  }


  StorageListController.$inject = ['i18n', 'StorageService', 'MessageBoxService', 'Socket', 'Service'];

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