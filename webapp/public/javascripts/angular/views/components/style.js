define([], function () {
  /**
   * It defines a Component structure for handling View Style Legend for Geometric Object and Grid
   * 
   * @property {Object} bindings - Defines component bindings to work
   * @type {angular.IComponent}
   */
  var terrama2StyleComponent = {
    bindings: {
      formCtrl: "<", // controller binding in order to throw up
      type: "=",
      columnsList: "=",
      postgisData: "=",
      model: "=",
      options: "="
    },
    templateUrl: BASE_URL + "dist/templates/views/templates/style.html",
    controller: StyleController
  };

  /**
   * It handles component behavior
   * 
   * @param {ColorFactory} ColorFactory - TerraMA² Color generator
   * @param {any} i18n - TerraMA² Internationalization module
   */
  function StyleController($scope, ColorFactory, i18n, DataSeriesService, StyleType, $http, Utility, DataProviderService, FormTranslator) {
    var self = this;
    // binding component form into parent module in order to expose Form to help during validation
    self.formCtrl = self.form;
    self.DataSeriesType = DataSeriesService.DataSeriesType;
    self.StyleType = StyleType;
    self.i18n = i18n;
    self.styleTypes = [];
    // Function bindings
    self.addColor = addColor;
    self.removeColor = removeColor;
    self.typeFilter = typeFilter;

    // Array with possible values of a column
    self.columnValues = [];

    self.showAutoCreateLegendButton = false;
    self.showGridAutoCreateLegendButton = false;
    self.legendPrecision = 2;
    /**
     * It keeps the rgba color values
     * 
     * @type {object}
     */
    self.rgba = {
      r: null,
      g: null,
      b: null,
      a: 1,
      elm: null
    };

    /**
     * It opens the rgba modal.
     * 
     * @returns {void}
     */
    self.rgbaModal = function(elm) {
      self.rgba.elm = elm;
      var rgbaColor = Utility.hex2rgba(elm.color);
      if (rgbaColor){
        self.rgba.r = rgbaColor.r;
        self.rgba.g = rgbaColor.g;
        self.rgba.b = rgbaColor.b;
        self.rgba.a = rgbaColor.a;
      }
      $("#rgbaModal").modal();
    };

    /**
     * It fillls the hex color field, converting the rgba to hex8.
     * 
     * @returns {void}
     */
    self.rgba2hex = function() {
      self.rgba.elm.color = Utility.rgba2hex(self.rgba.r, self.rgba.g, self.rgba.b, self.rgba.a);

      self.rgba.r = null;
      self.rgba.g = null;
      self.rgba.b = null;
      self.rgba.a = 1;
      self.rgba.elm = null;
    };

    // digesting StyleType enum into array
    for(var k in StyleType) {
      if (StyleType.hasOwnProperty(k)) {
        self.styleTypes.push({name: k, value: StyleType[k]});
      }
    }

    self.predefinedStyles = [];

    var predefinedStylesUrl = BASE_URL + "predefined-styles/";
    $http.get(predefinedStylesUrl).then(function(response){
      if (response.data){
        self.predefinedStyles = response.data.styles;
      }
    });

    /**
     * Function to filter predefined style by data series type
     */
    self.filterPredefinedStyles = function(predefinedStyle){
      if (self.type == null){
        return false;
      }
      var dataSeriesType = self.type;
      if (predefinedStyle.type == dataSeriesType)
        return true;
      return false;
    }

    self.minColorsLength = 1;
    
    var defaultColorOpts = {
      format: "hex8",
      required: true,
      alpha: true
    };
    /**
     * It configures color picker (Angular Color Picker dependency)
     * @type {Object}
     */
    self.colorOptions = self.options && self.options.color ? angular.merge(defaultColorOpts, self.options.color) : defaultColorOpts;
    /**
     * It defines a event listeners for color handling
     */
    self.events = {
      onChange: function (api, color, $event) {
        handleColor();
      }
    };
    /**
     * Listen when change creation type from view register update controller
     */
    $scope.$on('updateCreationType', function(event) {
      self.changeCreationType();
    });
    /**
     * Setting default parameters when change mode to xml file
     */
    self.changeCreationType = function(){
      if (self.model.metadata.creation_type == "editor"){
        delete self.model.metadata.xml_style;
        self.model.colors = [
          {
            color: "#FFFFFFFF",
            isDefault: true,
            title: "Default",
            value: ""
          }
        ];
      } else if (self.model.metadata.creation_type == "xml"){
        self.model.type = 3;
        self.model.colors = [];
        delete self.model.bands;
        delete self.model.beginColor;
        delete self.model.endColor;
      } else {
        self.model.type = 3;
        self.model.colors = [];
        self.setXmlStyleInfo(self.model.metadata.creation_type);
        delete self.model.bands;
        delete self.model.beginColor;
        delete self.model.endColor;
        $scope.$broadcast("schemaFormRedraw");
      }
    }
    // Regex to valide column name of style
    self.regexColumn = "^[a-zA-Z_][a-zA-Z0-9_]*$";

    self.changeColorType = function(){
      if (self.model.type == 1){
        self.minColorsLength = 2;
        self.model.colors = [
          {
            color: "#FFFFFFFF",
            isDefault: false,
            title: "Color 1",
            value: ""
          },
          {
            color: "#FFFFFFFF",
            isDefault: false,
            title: "Color 2",
            value: ""
          }
        ];
      } else {
        self.minColorsLength = 1;
        self.model.colors = [
          {
            color: "#FFFFFFFF",
            isDefault: true,
            title: "Default",
            value: ""
          }
        ];
      }
      if (self.model.type != 3){
        self.showAutoCreateLegendButton = false;
      } else {
        self.getColumnValues();
      }
      if (self.model.type == 2 && self.type == "GRID"){
        self.showGridAutoCreateLegendButton = true;
      } else {
        self.showGridAutoCreateLegendButton = false;
      }
    }

    self.initColorType = function(){
      if (!self.model.type)
        self.model.type = 3;
    }

    /**
     * Setting xml data on model
     */
    self.setXmlStyleInfo = function(styleId){

      var predefinedStyleInfo = self.predefinedStyles.find(function(style){ return style.id == styleId});
      if (predefinedStyleInfo){
        self.model.metadata.xml_style = predefinedStyleInfo.xml;
        self.model.fieldsToReplace = predefinedStyleInfo.fields;
        if (self.model.fieldsToReplace){
          self.model.fieldsToReplace.forEach(function(field){
            if (self.model.metadata[field])
              self.model.metadata[field] = parseInt(self.model.metadata[field])
          });
  
          var formTranslatorResult = FormTranslator(predefinedStyleInfo.gui.schema.properties, predefinedStyleInfo.gui.form, predefinedStyleInfo.gui.schema.required);
          
          self.predefinedStyleSchema = {
            type: 'object',
            properties: formTranslatorResult.object,
            required: predefinedStyleInfo.gui.schema.required
          };
  
          self.predefinedStyleForm = formTranslatorResult.display;
        }
      }
    }
    /**
     * It handles color summarization (begin and end) based in list of colors
     */
    $scope.$on("updateStyleColor", function () {
      if (self.model.metadata.creation_type == "editor"){
        handleColor();
        self.getColumnValues();
      } else if (self.model.metadata.creation_type != undefined){
        self.changeCreationType();
      }
    });

    /**
     * Lists the values of a column from a given table.
     * 
     * @returns {void}
     */
    self.getColumnValues = function(){
      if(self.columnsList.length == 0){
        self.columnValues = [];
        self.showAutoCreateLegendButton = false;
        return;
      } else {
        var hasInList = self.columnsList.some(function(column){
          return column == self.model.metadata.attribute;
        });
        if (!hasInList){
          self.columnValues = [];
          self.showAutoCreateLegendButton = false;
          return;
        }

      }
      if (self.model.type == 3 && self.model.metadata.attribute !== undefined && self.model.metadata.attribute !== ""){
        DataProviderService.listPostgisObjects({providerId: self.postgisData.dataProvider.id, objectToGet: "values", tableName: self.postgisData.tableName, columnName: self.model.metadata.attribute})
          .then(function(response){
            if (response.data.status == 400){
              self.columnValues = [];
              self.showAutoCreateLegendButton = false;
            } else {
              if (response.data.data)
                self.columnValues = response.data.data;
              else
                self.columnValues = [];

              if (self.model.type == 3 && self.columnValues.length > 0){
                self.showAutoCreateLegendButton = true;
              }
              else {
                self.showAutoCreateLegendButton = false;
              }
              
            }
          });
      } else {
        self.columnValues = [];
        self.showAutoCreateLegendButton = false;
        if (self.type == "GRID" && self.model.type == 2){
          self.showGridAutoCreateLegendButton = true;
        }
      }
    };

    /**
     * Auto create legends with possible values of attribute
     * 
     * @returns {void}
     */
    self.autoCreateLegend = function(){
      self.model.colors = [
        {
          color: "#FFFFFFFF",
          isDefault: true,
          title: "Default",
          value: ""
        }
      ];
      var defaultColors = ColorFactory.getDefaultColors();
      for (var i = 0; i < self.columnValues.length; i++){
        var newColor = {
          color: defaultColors[i],
          isDefault: false,
          title: self.columnValues[i],
          value: self.columnValues[i]
        }
        self.model.colors.push(newColor);
      }
    };

    /**
     * Auto create legends from initial and final values
     * 
     * @returns {void}
     */
    self.gridAutoCreateLegend = function(){
      self.model.colors = [
        {
          color: "#FFFFFFFF",
          isDefault: true,
          title: "Default",
          value: ""
        }
      ];
      var portionValue = (self.legendFinalValue - self.legendInitialValue)/self.legendQuantity;
      var defaultColors = ColorFactory.getDefaultColors();
      for (var i = 0; i <= self.legendQuantity; i++){
        var legendValue = Number((self.legendInitialValue + portionValue*i).toFixed(self.legendPrecision));
        var newColor = {
          color: defaultColors[i],
          isDefault: false,
          title: i == 0 ? "< " + legendValue : (Number((legendValue - portionValue).toFixed(self.legendPrecision)) +  " - " + legendValue),
          value: legendValue
        }
        self.model.colors.push(newColor);
      }
    }

    /**
     * It tries to sets begin and end color based in table row selection
     */
    function handleColor() {
      var initial = self.model.colors[0];
      var final = self.model.colors[self.model.colors.length - 1];
      self.model.beginColor = initial.color;
      self.model.endColor = final.color;
    }

    /**
     * It performs a filter iteration on StyleTypes in order to show/hide specific elements with custom restrictions.
     * @returns {any}
     */
    function typeFilter(item) {
      if (!(self.type !== self.DataSeriesType.GRID && item.value === StyleType.GRADIENT)) {
        return item;
      }
    }

    /**
     * It adds color in model array ctrl.model.colors
     */
    function addColor() {  
      var colorsLength = self.model.colors.length;
      var newColor = {
        color: "#FFFFFFFF",
        isDefault: false,
        title: "Color " + colorsLength
      }
      self.model.colors.push(newColor);
    }
    /**
     * It removes color of model array ctrl.model.colors
     */
    function removeColor(index) {  
      self.model.colors.splice(index, 1);
    }
  }

  // Dependencies Injection
  StyleController.$inject = ["$scope", "ColorFactory", "i18n", "DataSeriesService", "StyleType", "$http", "Utility", "DataProviderService", "FormTranslator"];
  return terrama2StyleComponent;
});
