define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations, DataSeriesService, DataProviderService, $timeout, $scope) {
      this.operationsTitle = i18n.__("Operations");
      this.monitoredDataSeriesTitle = i18n.__("Monitored Data Series");
      this.attributeIdentifierTitle = i18n.__("Attribute Identifier");
      this.dynamicDataSeriesTitle = i18n.__("Dynamic Data Series");

      this.SpatialOperations = SpatialOperations;
      this.DataSeriesService = DataSeriesService;
      this.DataProviderService = DataProviderService;

      this.$timeout = $timeout;

      this.columnsList = [];
      this.inputTableAttributes = {};

      this.staticDataSerieSelected = "";
      this.dynamicDataSerieSelected = "";

      this.listStaticDataSerie = {};
      this.listDynamicDataSerie = {};
      this.tableNameFromClassColumn = {};

      this.listInputLayersSelected = [];

      this.listOutputLayersSelected = [];
      this.outputLayerArray = [];

      this.ouputClassAttributeArray = [];
      this.listClassNameColumnValue = [];
      this.listClassNameColumnValueSelected = [];

      this.$scope = $scope;

      if (!this.model) {
        this.model = { queryBuilder: '' };
      }

      this.verifyData();
    }

    verifyData(){
      this.$timeout(()=>{

        this.onChangeStaticDataSeries();
        this.onChangeDynamicDataSeries();

        const {model} = this

        let outputLayerJson = model.outputlayer;
        let outputClassNameSelected = model.classNameSelected;

        $("#outputlayer").attr('disabled',true);

        if(typeof outputLayerJson !== 'undefined' && outputLayerJson != ""){
          outputLayerJson = outputLayerJson.replace('{','');
          outputLayerJson = outputLayerJson.replace('}','');
          this.outputLayerArray = outputLayerJson.split(",");

          this.outputLayerArray.forEach(layer => {
            layer = layer.replace('"','');
            layer = layer.replace('"','');
            this.listOutputLayersSelected.push(layer);
          });

          $("#selectStaticDataSeries").attr('disabled',true);
          $("#selectDynamicDataSeries").attr('disabled',true);
        }

        if(typeof outputClassNameSelected !== 'undefined' && outputClassNameSelected != ""){
          outputClassNameSelected = outputClassNameSelected.replace('{','');
          outputClassNameSelected = outputClassNameSelected.replace('}','');
          this.ouputClassAttributeArray = outputClassNameSelected.split(",");

          this.ouputClassAttributeArray.forEach(attributeClass => {
            this.listClassNameColumnValue.push(attributeClass);
          });

          $("#classColumnName").attr('disabled',true);
          $("#classNameSelected").attr('disabled',true);
        }

      })
    }

    async onChangeStaticDataSeries() {
      const { model, targetDataSeries } = this;

      if (!targetDataSeries)
        return;

      const id = targetDataSeries.data_provider_id;
      model.data_provider_id = id;

      await this.listAttributes();
    }

    async listAttributes() {
      const { DataProviderService, targetDataSeries, $timeout } = this;

      const options = {
        providerId: targetDataSeries.data_provider_id,
        objectToGet: "column",
        tableName : targetDataSeries.dataSets[0].format.table_name
      }

      const res = await DataProviderService.listPostgisObjects(options);
      this.columnsList = res.data.data.map(item => item.column_name);

      if(this.staticDataSerieSelected !== ""){
        Object.keys(this.listStaticDataSerie).forEach(key => {
          delete this.inputTableAttributes[key];
        });
      }

      this.columnsList.forEach(attribute => {
        let key = options.tableName + "." + attribute + " AS " + options.tableName + "_" + attribute;
        let value = targetDataSeries.name + ":" + attribute;

        this.inputTableAttributes[key] = value;
        this.listStaticDataSerie[key] = value;
      });

      this.staticDataSerieSelected = targetDataSeries.name;

      this.$scope.$digest();
    }

    async onChangeDynamicDataSeries() {

      const { DataProviderService } = this;

      const dynamicDataSeries = this.DataSeriesService.dynamicDataSeries().find(ds => (
        ds.id === parseInt(this.model.dynamicDataSeries)
      ));

      const tableNameFromAnalysisTable = await DataProviderService.listPostgisObjects({providerId: dynamicDataSeries.data_provider_id,
                                                                objectToGet: "values",
                                                                tableName : dynamicDataSeries.dataSets[0].format.table_name,
                                                                columnName: "table_name"})

      let tableNameAttributes = "";

      try {
        tableNameAttributes = tableNameFromAnalysisTable.data.data[0];
      }
      catch(error) {
        console.log(error);
      }

      const options = {
        providerId: dynamicDataSeries.data_provider_id,
        objectToGet: "column",
        tableName : tableNameAttributes === "" ? dynamicDataSeries.dataSets[0].format.table_name : tableNameAttributes
      }

      const res = await DataProviderService.listPostgisObjects(options);
      let dynamicTableAttributes = res.data.data.map(item => item.column_name);

      // Removing attributes from layer selected
      if(this.dynamicDataSerieSelected !== ""){
        Object.keys(this.listDynamicDataSerie).forEach(key => {
          delete this.inputTableAttributes[key];
        });
      }

      dynamicTableAttributes.forEach(attribute => {
        let key = options.tableName + "." + attribute + " AS " + options.tableName + "_" + attribute;
        let value = dynamicDataSeries.name + ":" + attribute;

        this.inputTableAttributes[key] = value;
        this.listDynamicDataSerie[key] = value;
      });

      this.dynamicDataSerieSelected = dynamicDataSeries.name;

      this.tableNameFromClassColumn['table_selected'] = this.dynamicDataSerieSelected;

      this.$scope.$digest();

      if(typeof this.model.classNameSelected == 'undefined'){
        this.selectClassName();
      }

    }

    async onMultInputSelected() {
      const{model} = this;

      this.model.outputlayer = [...this.listInputLayersSelected];

      this.listOutputLayersSelected = [];
      this.listOutputLayersSelected = [...this.listInputLayersSelected];

      if(typeof this.model.outputlayer !== 'undefined' && this.model.outputlayer != ""){
        this.outputLayerArray.forEach(layer => {
          layer = layer.replace('"','');
          layer = layer.replace('"','');
          this.listOutputLayersSelected.push(layer);
        });
      }

      this.model.outputlayer = [ ...new Set(this.listOutputLayersSelected) ];
    }

    async onMultSelectClassName() {
      const{model} = this;
      
      this.model.classNameSelected = [];
      this.model.classNameSelected = [...this.listClassNameColumnValueSelected];

    }

    async selectClassName() {

      const { DataProviderService } = this;

      let columnNameHandler = this.getClassNameFilter(this.dynamicDataSerieSelected);

      const dynamicDataSeries = this.DataSeriesService.dynamicDataSeries().find(ds => (
        ds.id === parseInt(this.model.dynamicDataSeries)
      ));

      const listColumnNameValue = await DataProviderService.listPostgisObjects({providerId: dynamicDataSeries.data_provider_id,
                                                                objectToGet: "values",
                                                                tableName : dynamicDataSeries.dataSets[0].format.table_name,
                                                                columnName: columnNameHandler});                                                        

      this.listClassNameColumnValue = listColumnNameValue.data.data;

      this.$scope.$digest();
    }

     getClassNameFilter(dynamicLayerName) {
      const dynamicLayers = {
        PRODES : "PRODES",
        DETER  : "DETER",
        FOCOS  : "FOCOS INPE",
        AREA_QUEIMADAS : "AREA_QUEIMADAS"
      }

      let classname = undefined;

      switch (dynamicLayerName) {

        case dynamicLayers.PRODES:
          classname = 'mainclass';
          break;

        case dynamicLayers.DETER:
          classname = 'classname';
          break;

        case dynamicLayers.FOCOS:
          classname = 'bioma';
          break;

        case dynamicLayers.AREA_QUEIMADAS:
          classname = undefined;
          break;

        default:
          break;
      }

      return classname;
    }

    getTableName() {
      if (!this.targetDataSeries || angular.equals({}, this.targetDataSeries))
        return "";

      return this.targetDataSeries.dataSets[0].format.table_name;
    }

    onOperatorClicked(item) {
      if (this.model.queryBuilder)
        this.model.queryBuilder += item.code;
      else
        this.model.queryBuilder = item.code;
    }
  }

  VectorProcessingComponent.$inject = ["i18n",
                                       "SpatialOperations",
                                       "DataSeriesService",
                                       "DataProviderService",
                                       '$timeout',
                                       "$scope"];

  const component = {
    bindings : {
      model: "=",
      metadata: '=',
      targetDataSeries: '=',
      identifier: '=',
      css: "<",
    },
    controller: VectorProcessingComponent,
    template: `
              <div class="col-md-12">
                <div class="row">
                  <div class="col-md-6">
                    <div class="col-align-self-start">
                      <div class="form-group has-feedback" terrama2-show-errors>
                        <label>{{$ctrl.operationsTitle}}:</label>
                          <select class="form-control"
                                  name="targetVectorProcessing"
                                  ng-model="$ctrl.model.operationType"
                                  ng-options="v as k for (k,v) in $ctrl.SpatialOperations"
                                  ng-required="true">
                          </select>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
                    ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
                {{ i18n.__('Operation is required') }}
              </span>

              <div class="col-md-6">
                <div class="col-align-self-start">
                  <div class="form-group has-feedback" terrama2-show-errors>
                    <label>Static Data Series:</label>
                    <select id="selectStaticDataSeries"
                            class="form-control"
                            name="targetMonitoredDataSeries"
                            ng-model="$ctrl.targetDataSeries"
                            ng-change="$ctrl.onChangeStaticDataSeries()"
                            ng-options="targetDS as targetDS.name for targetDS in $ctrl.DataSeriesService.staticDataSeries()"
                            ng-required="true">
                    </select>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
              ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
              {{ i18n.__('Monitored Data Series is required') }}
              </span>

              <div class="col-md-12">
                <div class="row">

                  <div class="col-md-6">
                    <div class="col-align-self-start">
                      <div class="form-group has-feedback" terrama2-show-errors>
                        <label>{{$ctrl.dynamicDataSeriesTitle}}:</label>
                          <select id="selectDynamicDataSeries"
                                  class="form-control"
                                  name="targetDynamicDataSeries"
                                  ng-model="$ctrl.model.dynamicDataSeries"
                                  ng-change="$ctrl.onChangeDynamicDataSeries()"
                                  ng-options="targetDS.id.toString() as targetDS.name for targetDS in $ctrl.DataSeriesService.dynamicDataSeries()"
                                  ng-required="true">
                          </select>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <!-- required -->
              <span class="help-block"
                    ng-show="forms.targetDataSeriesForm.targetDataSeries.$dirty && forms.targetDataSeriesForm.targetDataSeries.$error.required">
                {{ i18n.__('Data Series is required') }}
              </span>

              <div class="col-md-6">
                <div class="col-align-self-start">
                  <div class="form-group has-feedback" terrama2-show-errors>
                    <label>Input Attribute Layer:</label>
                    <select class="form-control"
                      name="inputAttributesLayer"
                      id="inputAttributesLayer"
                      ng-model="$ctrl.listInputLayersSelected"
                      ng-options="key as value for (key, value) in $ctrl.inputTableAttributes"
                      ng-change="$ctrl.onMultInputSelected()" multiple>
                    </select><br>
                  </div>
                </div>
              </div>

              <div class="col-md-6">
                <div class="form-group" terrama2-show-errors>
                  <label>Output Attribute Layer:</label>
                  <select class="form-control"
                    name="outputlayer"
                    id="outputlayer"
                    ng-model="$ctrl.model.outputlayer"
                    ng-options="attributes for attributes in $ctrl.listOutputLayersSelected" multiple>
                </select><br>
                </div>
              </div>

              <div class="col-md-12">
                  <div class="checkbox">
                    <label class="form-check-label" for="customClassFilter">
                        <input type="checkbox" ng-model="checked" ng-init="checked=false" id="customClassFilter"> Custom Class Filter
                    </label>
                  </div>
              </div>              

              <div class="col-md-12" ng-hide="!checked">
                <div class="form-group" terrama2-show-errors>
                  <label>Class Name:</label>
                  <select class="form-control"
                          name="classNameSelected"
                          id="classNameSelected"
                          ng-model="$ctrl.listClassNameColumnValueSelected"
                          ng-options="attributes for attributes in $ctrl.listClassNameColumnValue"
                          ng-change="$ctrl.onMultSelectClassName()"
                          multiple>
                  </select><br>
                </div>
              </div>
              `
  };
  return component;
})