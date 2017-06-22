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
        self.model.metadata = {
          creation_type: "1"
        };
        delete self.model.bands;
        delete self.model.beginColor;
        delete self.model.endColor;
      } else if (self.model.metadata.creation_type == "2"){
        self.model.type = 3;
        self.model.colors = [];
        self.model.metadata = {
          creation_type: "2"
        };
        self.setXmlInfo("wind_style.json");
        delete self.model.bands;
        delete self.model.beginColor;
        delete self.model.endColor;
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
            title: "Color 1",
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
     * Get xml file
     */
    self.onStyleChange = function(){
      switch (self.styleId){
        case "1":
          self.setXmlInfo("wind_style.json");
          break;
        case "0":
        default:
          self.model.metadata.xml_style = "";
          break;
      }
    }
    /**
     * Setting xml data on model
     */
    self.setXmlInfo = function(styleFile){
      var xmlUrl = BASE_URL + "json_styles/" + styleFile;
      $http.get(xmlUrl).then(function(response){
        self.model.metadata.xml_style = response.data.xml;
      });
    }
    /**
     * It handles color summarization (begin and end) based in list of colors
     */
    $scope.$on("updateStyleColor", function () {
      if (self.model.metadata.creation_type == "0")
        handleColor();
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
        title: "Color Title"
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
