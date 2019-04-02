define([],()=> {
  class AttributesComponent {
    constructor(i18n, dataProviderService, $timeout) {
      this.saveBtnTitle=i18n.__("Save")
      this.dataProviderService=dataProviderService
      this.$timeout=$timeout
      this.onListColumns()
    }

    onListColumns(){
      this.$timeout(()=>{

        if(this.model){
          this.model = JSON.parse(this.model);
          return;
        }

        this.model = [];
        const {tableName, provider, dataProviderService} = this
        dataProviderService.listPostgisObjects({providerId: provider, objectToGet: "column", tableName})
        .then(response=>{
          if (response.data.status == 400){
            return result.reject(response.data);
          }
          response.data.data.map(element => {
            this.model.push(
              {
                name:element.column_name,
                alias:""
              }
            );
          });
        });
      })
    }
  }

  AttributesComponent.$inject = ["i18n", "DataProviderService", "$timeout"];

  const component = {
    bindings : {
      model: "=",
      metadata: '=',
      provider: "<",
      tableName: "<",
      form: "="
    },
    controller: AttributesComponent,
    template: `
      <div class="col-md-12">
        <div style="margin-left: 15px;margin-right:15px">
          <div class="table-responsive">
            <table class="table table-hover">
              <thead>
                <tr>
                  <th>Attribute name</th>
                  <th>Alias</th>
                </tr>
              </thead>
              <tbody>
                <tr ng-repeat="cn in $ctrl.model">
                  <td>{{ cn.name }}</td>
                  <td><input class="form-control" name="form_attributes_{{cn.name}}" ng-model="cn.alias" type="text" required></td>
                </tr>
              </tbody>
            </table>
          </div>
          <hr />
        </div>
      </div>
      `
  };
  return component;
})