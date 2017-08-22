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
  function StyleController($scope, ColorFactory, i18n, DataSeriesService, StyleType, $http) {
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
     * Setting default parameters when change mode to xml file
     */
    self.changeCreationType = function(){
      if (self.model.metadata.creation_type == "0"){
        delete self.model.metadata.xml_style;
        self.model.colors = [
          {
            color: "#FFFFFFFF",
            isDefault: true,
            title: "Default",
            value: ""
          }
        ];
      } else if (self.model.metadata.creation_type == "1"){
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
        self.model.fieldsToReplace.forEach(function(field){
          if (self.model.metadata[field])
            self.model.metadata[field] = parseInt(self.model.metadata[field])
        })
        self.predefinedStyleSchema = predefinedStyleInfo.gui.schema;
        self.predefinedStyleForm = predefinedStyleInfo.gui.form;
      }
    }
    /**
     * It handles color summarization (begin and end) based in list of colors
     */
    $scope.$on("updateStyleColor", function () {
      if (self.model.metadata.creation_type == "0"){
        handleColor();
      } else if (self.model.metadata.creation_type != undefined){
        self.changeCreationType();
      }
    });

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
  StyleController.$inject = ["$scope", "ColorFactory", "i18n", "DataSeriesService", "StyleType", "$http"];
  return terrama2StyleComponent;
});
