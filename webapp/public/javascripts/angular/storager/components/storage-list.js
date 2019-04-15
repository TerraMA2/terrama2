define([], () => {
  class StorageListController {
    constructor(i18n, Storage, MessageBox) {
      this.i18n = i18n;
      this.Storage = Storage;
      this.MessageBox = MessageBox;

      this.init();

      this.fields = [{key: 'name', as: i18n.__("Name")}, {key: "description", as: i18n.__("Description")}];
      this.linkToAdd = `${BASE_URL}configuration/storages/new`;
      this.link = (object) => {
        return `${BASE_URL}configuration/storages/${object.id}`;
      };
    }

    async init() {
      await this.Storage.init();

      this.extra = {
        canRemove: this.hasProjectPermission || configuration.hasProjectPermission
      }
    }
  }
  StorageListController.$inject = ['i18n', 'StorageService', 'MessageBoxService'];

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
              <terrama2-alert-box2 handler="$ctrl.MessageBoxService.alertBox" close="$ctrl.close()"></terrama2-alert-box2>
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