define([], () => {
  class QueryBuilderController {
    constructor($scope, DataSeriesService, MapService, DataProviderService, $q, i18n) {
      this.$scope = $scope;
      this.DataSeriesService = DataSeriesService;
      this.MapService = MapService;
      this.DataProviderService = DataProviderService;
      this.$q = $q;
      this.inputTableAttributes = {};
      this.listInputLayersSelected = [];
      this.listOutputLayersSelected = [];
      this.listMoveToLeft = [];
      this.outputLayer = [];
      this.table_name = "";
      this.isDisable = false;

      this.viewNameLabel = i18n.__('Name of the view to be created');
      this.inputAttributeLabel = i18n.__('Input Attribute Layer');
      this.outputAttributeLabel = i18n.__('Output Attribute Layer');
      this.attributeLabel = i18n.__('Attribute');
      this.operatorLabel = i18n.__('Operator');
      this.valueLabel = i18n.__('Value');
      this.loadAttributesLabel = i18n.__("Load attributes");
      this.cancelButton = i18n.__("Cancel");
      this.updateButton = i18n.__("Yes, update");
      this.attention = i18n.__("Attention");
      this.messageBody = i18n.__("This view already exists!\n Do you want to update it?");
      this.moveRightLabel = i18n.__('Move selected attributes to the right');
      this.moveAllRightLabel = i18n.__('Move all selected attributes to the right');
      this.moveLeftLabel = i18n.__('Move selected attributes to the left');
      this.moveAllLeftLabel = i18n.__('Move all selected attributes to the left');

      this.dataProvider = {};
      this.listTableAttributes = [];
      this.listToUpdate = [];
      this.listValuesOfColumn = [];
      this.layerSelected = "";
      this.loadAttr = true;
      this.attributeFilter = "---";
      this.attributeValue = "";
      this.operatorValue = "=";
      this.clauseValue = "";
      this.blockField = false;
      this.geometryColumn = 0;
    }

    $onInit() {
      if (this.getTableName() && this.getProvider()) {
        this.onChange();
      }
    }

    showOptions(){
      return;
    }

    getTableName() {
      const res = this.$scope.evalExpr('model.show_view');
      
      if(this.$scope.model.listOutputLayersSelected !== undefined && this.loadAttr == true){
        const {listOutputLayersSelected, table_name } = this.$scope.model;
        var _this = this;
        this.blockField = true;
        this.layerSelected = table_name;
        this.table_name = table_name;
        this.listToUpdate = listOutputLayersSelected.replace("[","").replace("]","").replace(/"/g,"").split(",");
        this.listToUpdate = this.listToUpdate.map(e => { return e.trim() });

      }
      if(this.loadAttr && $("#table_name").val()){
        this.loadAttributes();
        this.loadAttr = false;
      }
      if(this.table_name !== $("#table_name").val()){
        this.isDisable = false;
        this.inputTableAttributes = {};
        this.listOutputLayersSelected = [];
      }

      if(res){
        $("#query-builder").css("resize","vertical");
      }

      return res;
    }

    // Query builder
    changeAttribute(){
      this.attributeValue = "";
    }

    setOperatorValue(){
      const{ operatorValue } = this;
      this.operatorValue = operatorValue;
    }

    fillTextbox(string){
      this.attributeValue = string;
      this.listAttributeValues = null;

    }

    complete(evt){

      var str = evt;
      var output=[];
      this.msgValueError = "";
			this.listValuesOfColumn.forEach(function(value){
				if(value.toLowerCase().indexOf(str.toLowerCase())>=0){
					output.push(value);
				}
			});
      this.listAttributeValues = output;

    }

    async getValuesByColumn(){
      const{ DataProviderService } = this;

      const options = {
        providerId: this.dataProvider[0].id,
        objectToGet: "allvalues",
        columnName: this.attributeFilter,
        tableName : $("#table_name").val()
      }

      const res = await DataProviderService.listPostgisObjects(options);
      if (res.data.status == 200){
        this.listValuesOfColumn = res.data.data;
        this.complete("");
        this.$scope.$apply();

      }else{
        throw new Error(res.data.message);
      }

    }

    insertAtCaret(value) {
      var fieldText = document.getElementById('query-builder');
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
            fieldText.value = fieldText.value.substring(0, startPos)+ ` ${value}`
               + fieldText.value.substring(endPos, fieldText.value.length);
          }else if((lastCharFromCursor == "\n") || ( lastCharFromCursor == ") " )){
            fieldText.value = fieldText.value.substring(0, startPos).replace(/\n$/, "") + ` ${value}` + "\n"
               + fieldText.value.substring(endPos, fieldText.value.length)
          }else{
            return;
          }
        }else{
          fieldText.value = fieldText.value.substring(0, startPos)+ value
                 + fieldText.value.substring(endPos, fieldText.value.length);
        }

        var pos = startPos + value.length;
        fieldText.focus();
        fieldText.setSelectionRange(pos, pos);
      }
      else if (fieldText.selectionStart || fieldText.selectionStart == '0') {
          var startPos = fieldText.selectionStart;
          var endPos = fieldText.selectionEnd;
          if (startPos == 0){
            startPos = fieldText.value.length;
            endPos = fieldText.value.length;
          }
          if (clauses.includes(value)){
            var lastCharFromCursor = fieldText.value.substring(startPos - 1, endPos);
            var lastClause = fieldText.value.substring(startPos - 5, endPos).replace(/\n$/, "").trim();

            if((lastCharFromCursor == ")") || (lastCharFromCursor == " ") || (lastCharFromCursor == "") || (lastCharFromCursor == "'")){
              fieldText.value = fieldText.value.substring(0, startPos).replace(/\n$/, "") + ` ${value}` + "\n"
                + fieldText.value.substring(endPos, fieldText.value.length)
            }else{
              return;
            }
          }else{
            fieldText.value = fieldText.value.substring(0, startPos)+ ` ${value}`
                  + fieldText.value.substring(endPos, fieldText.value.length);
          }

      } else {
          fieldText.value += value;
          fieldText.value = fieldText.value;
      }
    }

    getSelectedText () {
      var fieldText = document.getElementById('query-builder');

      if(window.navigator.userAgent.indexOf("Edge") > -1) {
        var startPos = fieldText.selectionStart;
        var endPos = fieldText.selectionEnd;

        fieldText.value = "*" + fieldText.value.substring(0, startPos);

        var pos = startPos + 1;
        fieldText.focus();
        fieldText.setSelectionRange(pos, pos);
      }
      else if (fieldText.selectionStart || fieldText.selectionStart == '0') {
          var startPos = fieldText.selectionStart;
          var endPos = fieldText.selectionEnd;
          fieldText.value =  fieldText.value.substring(0, startPos)
                                + "("
                                + fieldText.value.substring(startPos, endPos)
                                + ")"
                                + fieldText.value.substring(endPos, fieldText.value.length);
      }
    }

    setClauseValue(value){

      this.clauseValue = value;
      this.insertAtCaret(this.clauseValue);

    }

    makeQuery(){
      var table = this.table_name;
      var attribute = `${this.attributeFilter}`;
      var operator = `${this.operatorValue}`;
      var value = `${this.attributeValue}`;

      if(operator == ""){
        this.msgValueErrorOperator = "Select one operator!"
        return;
      }else{
        this.msgValueErrorOperator = "";
      }

      if (!this.listValuesOfColumn.includes(value)){
        this.msgValueError = `${ this.valueLabel } ${ value } ${ this.notFoundText }`;
        this.listAttributeValues = null;
      }else{
        this.msgValueError = "";
        var query = `(${table}.${attribute} ${operator} '${value}')`;
        this.insertAtCaret(query);
        $("#query-builder").trigger("change")
      }


    }
    //End query builder


    // Wicket
    
    getBackAttrGeom(){
      var index = this.inputTableAttributes.findIndex(x => x.type.startsWith("geom"));
      var attr = this.inputTableAttributes[index];
      this.listOutputLayersSelected.push(attr);
      this.inputTableAttributes.splice(index, 1);
    }

    getAttrGeom(){
      var index = this.listOutputLayersSelected.findIndex(x => x.type.startsWith("geom"));
      var attr = this.listOutputLayersSelected[index];
      this.inputTableAttributes.push(attr);
      this.listOutputLayersSelected.splice(index, 1);
    }

    updateInputAttributes(){
      let i = this.listInputLayersSelected.length;
      var j = 0;
      for(j=0; j<i; j++){
        var currentValue = JSON.parse(this.listInputLayersSelected[j]);
        var index = this.inputTableAttributes.findIndex(x => x.column_name === currentValue.column_name);
        this.inputTableAttributes.splice(index, 1);
      }
    }

    updateOutputAttributes(){
      let i = this.listMoveToLeft.length;
      var j = 0;
      var notInclude = ['pk', 'geom'];

      for(j=0; j<i; j++){
        var currentValue = JSON.parse(this.listMoveToLeft[j]);
        var index = this.listOutputLayersSelected.findIndex(x => x.column_name === currentValue.column_name);
        
        if(currentValue.type !== 'pk' && this.geometryColumn > 1){
          this.listOutputLayersSelected.splice(index, 1);
        }
        if(!notInclude.includes(currentValue.type) && this.geometryColumn < 2){
          this.listOutputLayersSelected.splice(index, 1);
        }
      }
    }

    getPkGeom(){
      var _this = this;
      this.inputTableAttributes.map(e => {
        if(e.type == 'pk' || e.type == 'geom'){
          // if(this.listToUpdate.length > 0){
          //   e.type = "pk";
          // }
          this.listInputLayersSelected.push(JSON.stringify(e));
          this.listOutputLayersSelected.push(e);
        }
      });
      if(this.listToUpdate.length > 0){
        _this.inputTableAttributes.map(e => {
          if(_this.listToUpdate.includes(e.column_name) && e.type != "geom" && e.type != "pk"){
            var element = JSON.stringify(e);
            _this.listInputLayersSelected.push(element);
            _this.listOutputLayersSelected.push(e);
          }
        });
      }
      
      this.updateInputAttributes();
    }

    moveToRight(){

      this.listInputLayersSelected.map(e => {
        var value = JSON.parse(e);
        if(this.geometryColumn > 1 && value.type.startsWith("geom")){
          this.getAttrGeom();
        }
        this.listOutputLayersSelected.push(value);
      });
      this.updateInputAttributes();
    }

    moveAllToRight(){
      var listAux = [];
      var isUpdate = false;
      this.inputTableAttributes.map(e =>{
        if(this.geometryColumn > 1 && e.type.startsWith("geom") && !isUpdate){
          var index = this.listOutputLayersSelected.findIndex(x => x.type.startsWith("geom"));
          var attr = this.listOutputLayersSelected[index];
          listAux.push(attr);
          this.listOutputLayersSelected.splice(index, 1);
          isUpdate = true;
        }
        this.listOutputLayersSelected.push(e);
      });
      this.inputTableAttributes = listAux;
      this.listInputLayersSelected = [];
    }

    moveToLeft(){
      this.listMoveToLeft.map(e => {
        var element = JSON.parse(e);
        var notInclude = ['pk', 'geom'];

        if(this.geometryColumn > 1 && element.type.startsWith("geom")){
          this.getBackAttrGeom();
        }
        if(element.type !== 'pk' && this.geometryColumn > 1){
          this.inputTableAttributes.push(element);
        }
        if(!notInclude.includes(element.type) && this.geometryColumn < 2){
          this.inputTableAttributes.push(element);
        }

      });
      this.updateOutputAttributes();
    }

    moveAllToLeft(){
      var listAux = [];
      var notInclude = ['geom', 'pk', 'geom_1'];

      this.listOutputLayersSelected.map(e => {
        if(!notInclude.includes(e.type)){
          this.inputTableAttributes.push(e);
        }else{
          listAux.push(e);
        }
      });
      this.listOutputLayersSelected = listAux;
      this.listMoveToLeft = [];
    }

    async onMultOutputSelected(e){
      this.listMoveToLeft = e;
    }

    async onMultInputSelected(e) {
      this.listInputLayersSelected = e;
    }

    // End wicket

    loadAttributes(){
      this.table_name = $("#table_name").val();
      this.layerSelected = this.table_name;
      var data_provider = $( "div[name='data_provider_id']" ).text().trim();
      var providersList = this.DataProviderService.list();
      var listIds = [];

      this.dataProvider = providersList.filter(function(element) {
        return element.name == data_provider;
      });

      this.DataProviderService.listPostgisObjects({providerId: this.dataProvider[0].id, objectToGet: "idcolumn", tableName: this.table_name})
        .then(function(response){
          if (response.data.status == 400){
            return result.reject(response.data);
          }
          listIds = response.data.data.map((e) => {
            return e.column_name;
          });
        });

        var result = this.$q.defer();
        var _this = this;
        this.DataProviderService.listPostgisObjects({providerId: this.dataProvider[0].id, objectToGet: "gettypes", tableName: this.table_name})
            .then(function(response){
              if (response.data.status == 400){
                return result.reject(response.data);
              }
  
              var attributes = response.data.data.map((e) =>{
                if(e.udt_name == 'geom' || e.udt_name == "geometry" || e.udt_name == "string:geom"){
                  _this.geometryColumn++;
                  if(_this.geometryColumn > 1){
                    return {'column_name': e.column_name, 'icon':'fa fa-cubes', 'type':'geom_1'}
                  }
                  return {'column_name': e.column_name, 'icon':'fa fa-cubes', 'type':'geom'}
                }else if(!listIds.includes(e.column_name)){
                  return {'column_name': e.column_name, 'icon':'glyphicon glyphicon-text-background','type':'abc123'}
                }else{
                  return {'column_name': e.column_name, 'icon':'fa fa-key','type':'pk'}
                }
              });
              
              _this.listTableAttributes = attributes.slice();
              _this.inputTableAttributes = attributes;
              _this.isDisable = true;

              _this.getPkGeom();
            });

    }

    /**
     * Retrieves dataprovider from parent instance
     * @return {number}
     */
    getProvider() {
      if (!this.$scope.form.provider)
        return;

      const expression = this.$scope.form.provider;

      const res = this.$scope.evalExpr(expression);

      if (!res)
        return 0;

      return res;
    }

    /**
     * Detects user change iteractions on QueryBuilder component
     *
     * **Its already debounced function**
     */
    async onChange() {

      const { mapId } = this.$scope.form;
      const { MapService, DataSeriesService } = this;

      if (!mapId) {
        console.warn(`The Query Builder component is associated with mapId ${mapId} but it does not exist. The component may not work properly to visualize on Map`)
        return;
      }

      /**
       * Retrieves Map instance from MapService. It is injected by default when use <terrama2-map> directive
       * @type {MapContainer}
       */
      const map = MapService.getMap(mapId);

      const { query_builder, table_name, view_name } = this.$scope.model;

      // Get data provider from instance in order to detect which database should list
      const provider = this.getProvider();

      // Retrieve list of WKT objects with associated parameters
      const wkts = await DataSeriesService.getWKT(table_name, provider, query_builder);

      // If there is already a layer, just remove
      if (map.getLayer(view_name))
        map.removeLayer(view_name);

      // Add geometry polygon into map instance
      map.addLayerFromWKT(view_name, wkts, 'EPSG:4326');
    }
  }

  QueryBuilderController.$inject = [ '$scope', 'DataSeriesService', 'MapService', 'DataProviderService','$q', 'i18n' ];

  return QueryBuilderController;
});