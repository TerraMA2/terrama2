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
        this.model = [];
        
        const dataProviderid = this.dataserie.data_provider_id;
        const dataSerieTableName = this.dataserie.name;
        var attributesJson = [];

        $.ajax({
          url: BASE_URL + 'api/datasetidByDataSerie',
          type: "GET",
          async: false,
          data: {
            dataProviderid: dataProviderid,
            dataSerieTableName: dataSerieTableName
          }
        }).done(function(response){ 

          if(typeof response !== 'undefined' && typeof response[0] !== 'undefined'){
            var dataSetid = response[0].data_set_id;
            
            $.ajax({
              url: BASE_URL + 'api/attributesByDataSerie',
              type: "GET",
              async: false,
              data: {
                dataProviderid: dataProviderid,
                dataSetid: dataSetid
              }
            }).done(function(response){
              if(typeof response !== 'undefined' && response.length > 0 && typeof response[0] !== 'undefined'){
                var attributesResponseStr = response[0].value;
                attributesJson = JSON.parse(attributesResponseStr);
              }
            });
          }
        });

        const {tableName, provider, dataProviderService} = this
        dataProviderService.listPostgisObjects({providerId: provider, objectToGet: "column", tableName})
        .then(response=>{
          if (response.data.status == 400){
            return result.reject(response.data);
          }
          response.data.data.map(element => {
            var visibleOp = false;
            var aliasOp = "";

            attributesJson.forEach(function(jsonElement){
              if(element.column_name == jsonElement.name){
                visibleOp = jsonElement.visible;
                aliasOp = jsonElement.alias;
              }
            });

            this.model.push(
              {              
                name:element.column_name,
                visible: visibleOp,
                alias: aliasOp
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
      dataserie: "<",
      provider: "<",
      tableName: "<"
    },
    controller: AttributesComponent,
    template: `
      <div class="col-md-12">
        <div style="margin-left: 15px;margin-right:15px">
          <div class="table-responsive">
            <table class="table table-hover">
              <thead>
                <tr>
                  <th class="col-md-2">Attribute name</th>
                  <th class="col-md-1">Visible</th>
                  <th class="col-md-9">Alias</th>
                </tr>
              </thead>
              <tbody>
                <tr ng-repeat="cn in $ctrl.model">
                  <td>{{ cn.name }}</td>
                  <td><input type="checkbox" class="custom-control-input" ng-model="cn.visible"></td>
                  <td><input class="form-control" ng-model="cn.alias" type="text"></td>
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