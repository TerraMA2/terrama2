define([],()=> {
  class VectorProcessingComponent {
    constructor(i18n, SpatialOperations, DataSeriesService, DataProviderService, $timeout, $scope, $sce) {
      this.operationsTitle = i18n.__("Operations");
      this.monitoredDataSeriesTitle = i18n.__("Monitored Data Series");
      this.attributeIdentifierTitle = i18n.__("Attribute Identifier");
      this.dynamicDataSeriesTitle = i18n.__("Dynamic Data Series");
      this.tableLabel = i18n.__("Table");
      this.attributeLabel = i18n.__("Attribute");
      this.valueLabel = i18n.__("Value");
      this.operatorLabel = i18n.__("Operator");
      this.sqlResultLabel = i18n.__("SQL Result");
      this.clauseLabel = i18n.__("Clause");
      this.classFilterLabel = i18n.__("Custom Class Filter");
      this.selectLabel = i18n.__("Select");
      this.notFoundText = i18n.__("not found.")
      this.staticDataSerie = i18n.__('Static Data Series');
      this.inputAttributeLabel = i18n.__('Input Attribute Layer');
      this.outputAttributeLabel = i18n.__('Output Attribute Layer');
      
      this.SpatialOperations = SpatialOperations;
      this.DataSeriesService = DataSeriesService;
      this.DataProviderService = DataProviderService;

      this.$timeout = $timeout;

      this.columnsList = [];
      this.inputTableAttributes = {};
      this.listTableAttributes = [];
      this.listAttributeValues = [];
      this.listValuesOfColumn = [];
      this.clauseCount = 56;
      this.showQueryResult = false;
      this.checked = false
      this.hasChecked = false;

      this.staticDataSerieSelected = "";
      this.dynamicDataSerieSelected = "";
      this.attributeValue = "";
      this.operatorValue = "";
      this.clauseValue = "";
      this.filterString = "";
      this.msgValueError = "";
      this.msgValueErrorOperator = "";
      this.tableNameSelected = {};

      this.dynamicDataSerie = {};

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
      this.$sce = $sce;

      if (!this.model) {
        this.model = { queryBuilder: '' };
      }

      this.verifyData();
    }


    
    checkFilter(e){
      if(this.hasChecked){
        this.showQueryResult = false;
        this.hasChecked = false;
      }else{
        this.showQueryResult = true;
        this.hasChecked = true;
      }
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
          
          this.checked = true;
          this.hasChecked = true;
          this.showQueryResult = true;
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
      this.dynamicDataSerie = dynamicDataSeries;

      this.tableNameFromClassColumn['table_selected'] = this.dynamicDataSerieSelected;

      this.$scope.$digest();

      if(typeof this.model.classNameSelected == 'undefined'){
        this.selectClassName();
      }

    }

    getSelectedText () {
      var fieldText = document.getElementById('sql-result');

      if(window.navigator.userAgent.indexOf("Edge") > -1) {
        var startPos = fieldText.selectionStart; 
        var endPos = fieldText.selectionEnd; 
            
        this.filterString = "*" + fieldText.value.substring(0, startPos);
        
        var pos = startPos + 1;
        fieldText.focus();
        fieldText.setSelectionRange(pos, pos);
      }
      else if (fieldText.selectionStart || fieldText.selectionStart == '0') {
          var startPos = fieldText.selectionStart;
          var endPos = fieldText.selectionEnd;
          this.filterString =  fieldText.value.substring(0, startPos) 
                                + "("
                                + fieldText.value.substring(startPos, endPos)
                                + ")"
                                + fieldText.value.substring(endPos, fieldText.value.length);
      }
    }
    
    insertAtCaret(value) {
      var fieldText = document.getElementById('sql-result');
      var clauses = ['and', 'or', 'AND', 'OR']
      if (document.selection) {
        fieldText.focus();
        sel = document.selection.createRange();
        sel.text = value;

      }else if(window.navigator.userAgent.indexOf("Edge") > -1) {
        var startPos = fieldText.selectionStart; 
        var endPos = fieldText.selectionEnd;

        if (clauses.includes(value)){
          var lastCharFromCursor = fieldText.value.substring(startPos - 1, endPos).trim();
          if (lastCharFromCursor == ")"){
            this.filterString = fieldText.value.substring(0, startPos)+ ` ${value}` 
               + fieldText.value.substring(endPos, fieldText.value.length);
          }else if((lastCharFromCursor == "\n") || ( lastCharFromCursor == ") " )){
            this.filterString = fieldText.value.substring(0, startPos).replace(/\n$/, "") + ` ${value}` + "\n"
               + fieldText.value.substring(endPos, fieldText.value.length)
          }else{
            return;
          }
        }else{
          this.filterString = fieldText.value.substring(0, startPos)+ value 
                 + fieldText.value.substring(endPos, fieldText.value.length);
        }

        var pos = startPos + value.length;
        fieldText.focus();
        fieldText.setSelectionRange(pos, pos);
      }
      else if (fieldText.selectionStart || fieldText.selectionStart == '0') {
          var startPos = fieldText.selectionStart;
          var endPos = fieldText.selectionEnd;
          console.log(fieldText.value.length);
          if (startPos == 0){
            startPos = fieldText.value.length;
            endPos = fieldText.value.length;
          }
          if (clauses.includes(value)){
            var lastCharFromCursor = fieldText.value.substring(startPos - 1, endPos);
            var lastClause = fieldText.value.substring(startPos - 5, endPos).replace(/\n$/, "").trim();

            if((lastCharFromCursor == ")") || (lastCharFromCursor == " ") || (lastCharFromCursor == "")){
              this.filterString = fieldText.value.substring(0, startPos).replace(/\n$/, "") + ` ${value}` + "\n"
                + fieldText.value.substring(endPos, fieldText.value.length)
            }else{
              return;
            }
          }else{
            this.filterString = fieldText.value.substring(0, startPos)+ ` ${value}` 
                  + fieldText.value.substring(endPos, fieldText.value.length);
          }

      } else {
          fieldText.value += value;
          this.filterString = fieldText.value;
      }
    }

    makeQuery(){
      var table = `${this.tableNameSelected.dataSets[0].format.table_name}`;
      var attribute = `${this.attributeFilter}`;
      var operator = `${this.operatorValue}`;
      var value = `${this.attributeValue}`;
      var clause = `${this.clauseValue}`;

      if(operator == ""){
        this.msgValueErrorOperator = "Select one operator!"
        return;
      }else{
        this.msgValueErrorOperator = "";
      }

      this.msgValueError = "";
      var query = `(${table}.${attribute} ${operator} '${value}')`;
      this.insertAtCaret(query);


    }

    setOperatorValue(){
      const{ operatorValue } = this;

      this.operatorValue = operatorValue;
    }

    setClauseValue(value){

      this.clauseValue = value;
      this.insertAtCaret(this.clauseValue);

    }

    complete(evt){
      
      var str = evt;
      var output=[];
      this.msgValueError = "";
      angular.forEach(this.listValuesOfColumn,function(value){
        if(value.toString().toLowerCase().indexOf(str.toLowerCase())>=0){
          output.push(value);
        }
      });
      this.listAttributeValues = output;
      
    }

    fillTextbox(string){
      this.attributeValue = string;
      this.listAttributeValues = null;
      
    }
    
    changeAttribute(){
      this.operatorValue = "";
      this.attributeValue = "";
      this.$scope.$apply();
    }
    async getValuesByColumn(){
      const{ DataProviderService, attributeFilter } = this;

      const options = {
        providerId: this.tableNameSelected.data_provider_id,
        objectToGet: "allvalues",
        columnName: attributeFilter,
        tableName : this.tableNameSelected.dataSets[0].format.table_name
      }

      const res = await DataProviderService.listPostgisObjects(options);
      console.log(res);
      if (res.data.status == 200){
        this.listValuesOfColumn = res.data.data;
        this.complete("");
        this.$scope.$apply();

      }else{
        throw new Error(res.data.message);
      }

    }

    async getAttributesOfDataSeries(evt) {
      const{ DataProviderService,  layerSelected } = this;
      var listColumnGeometry = ['geom', 'the_geom', 'geometry']
      var attributeList = [];
      
      if(layerSelected !== ""){
        
        let layer = JSON.parse(layerSelected);
        this.tableNameSelected = layer;

        let layerType = layer.data_series_semantics;

        if(layerType.temporality == "DYNAMIC"){

          const options = {
            providerId: layer.data_provider_id,
            objectToGet: "column",
            tableName : layer.dataSets[0].format.table_name
          }

          const res = await DataProviderService.listPostgisObjects(options);
          
          if(res.data.status == 200){

            res.data.data.forEach((item) =>{
              if (!listColumnGeometry.includes(item.column_name)){
                attributeList.push(item.column_name);
              }
            });

          }else{
            throw new Error(res.data.message);
          }

        }else{

          const options = {
            providerId: layer.data_provider.id,
            objectToGet: "column",
            tableName : layer.dataSets[0].format.table_name
          }

          const res = await DataProviderService.listPostgisObjects(options);

          if (res.data.status == 200){
            var attributeList = [];

            res.data.data.forEach((item) =>{
              if (!listColumnGeometry.includes(item.column_name)){
                attributeList.push(item.column_name);
              }
            });

          }else{
            throw new Error(res.data.message);
          }

        }
        this.listTableAttributes = attributeList;
        this.attributeValue = "";
        this.operatorValue = "";
        this.$scope.$apply();
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
              <style type="text/css">
                .list-group-item:hover{
                  color: #337ab7;
                  text-shadow:  0 0 1em #337ab7;
                  cursor: pointer;
                }
                .clause-value{
                  color:red
                }
                .filter-value{
                  color:purple
                }
                .attribute-value{
                  color:blue
                }
                .table-value{
                  color:green
                }
                .operator-value{
                  color:orange
                }
              </style>
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
                    <label>{{ $ctrl.staticDataSerie }}:</label>
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
                    <label>{{ $ctrl.inputAttributeLabel }}:</label>
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
                  <label>{{ $ctrl.outputAttributeLabel }}:</label>
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
                        <input type="checkbox" ng-change="$ctrl.checkFilter(this)" ng-model="$ctrl.checked" id="customClassFilter">{{ $ctrl.classFilterLabel }}
                    </label>
                  </div>
              </div>
              
              
              <div class="col-md-12" ng-hide="!$ctrl.showQueryResult">
                <div class="form-group col-md-12">

                  <div class="col-md-2">
                    <label>{{ $ctrl.tableLabel }}:</label>
                    <select class="form-control" ng-click="$ctrl.getAttributesOfDataSeries(this)" id="layerSelectedId" ng-model="$ctrl.layerSelected">
                      <option value="" selected>--- {{ $ctrl.selectLabel }} ---</option>
                      <option value="{{ $ctrl.targetDataSeries }}">{{ $ctrl.targetDataSeries.name }}</option>
                      <option value="{{ $ctrl.dynamicDataSerie }}">{{ $ctrl.dynamicDataSerieSelected }}</option>
                    </select>
                  </div>

                  <div class="col-md-2">
                    <label>{{ $ctrl.attributeLabel }}</label>
                    <select 
                      class="form-control"
                      id="listAttributeId"
                      ng-model="$ctrl.attributeFilter"
                      ng-options="item for item in $ctrl.listTableAttributes"
                      ng-change="$ctrl.changeAttribute()">
                      <option value="" selected>--- {{ $ctrl.selectLabel }} ---</option>
                    </select>
                  </div>

                  <div class="col-md-1">
                    <label>{{ $ctrl.operatorLabel }}:</label>
                    <select class="form-control" id="operatorId" ng-click="$ctrl.setOperatorValue(this)" ng-model="$ctrl.operatorValue" required>
                      <option value="=" style="font-size:16px">=</option>
                      <option value=">" style="font-size:16px">></option>
                      <option value="<" style="font-size:16px"><</option>
                      <option value=">=" style="font-size:16px">>=</option>
                      <option value="<=" style="font-size:16px"><=</option>
                      <option value="<>" style="font-size:16px"><></option>
                      <option value="LIKE" style="font-size:16px">LIKE</option>
                    </select>
                    <span style="color:red;font-size:12px">{{ $ctrl.msgValueErrorOperator }}</span>
                  </div>

                  <div class="col-md-1">
                    <label></label>
                    <button class="btn btn-primary" ng-click="$ctrl.getValuesByColumn()" style="margin-top:25px">
                      <i class="fa fa-search fa-1x"></i>
                    </button>
                  </div>

                  <div class="col-md-2">
                    <label>{{ $ctrl.valueLabel }}:</label>
                    <input type="text" name="attributeValue" id="attributeValue" ng-model="$ctrl.attributeValue" ng-keypress="$ctrl.complete($ctrl.attributeValue)" class="form-control" />
                    <span style="color:red;font-size:12px">{{ $ctrl.msgValueError }}</span>
                    <ul class="col-md-10" style="max-height:100px;overflow:auto;overflow-x:hidden;padding:0px;position:absolute">
                      <li class="list-group-item" ng-repeat="attributeValue in $ctrl.listAttributeValues" ng-click="$ctrl.fillTextbox(attributeValue)">{{attributeValue}}</li>
                    </ul>
                  </div>

                  <div class="col-md-1">
                    <button class="btn btn-primary" style="margin-top:25px" ng-click="$ctrl.makeQuery()">
                      <i class="fa fa-plus fa-1x"></i>
                    </button>
                  </div>

                  <div class="col-md-2">
                    <div class="row">
                      <button class="btn btn-primary" ng-click="$ctrl.setClauseValue('and')" value="and" ng-model="$ctrl.clauseValue" style="margin-top:25px">
                      AND
                      </button>
                      <button class="btn btn-primary" ng-click="$ctrl.setClauseValue('or')" value="and" ng-model="$ctrl.clauseValue" style="margin-top:25px">
                      OR
                      </button>
                      <button class="btn btn-primary" ng-click="$ctrl.getSelectedText()" style="margin-top:25px">
                      ( )
                      </button>
                    </div>
                  </div>

                </div>

                <div class="form-group" name="script">
                  <label>{{ $ctrl.sqlResultLabel }}:</label>
                  <textarea ng-model="$ctrl.filterString" style="height:250px;overflow:auto;min-height:150px;width:100%" id="sql-result"></textarea>
                </div>
              </div>
              `
  };
  return component;
})