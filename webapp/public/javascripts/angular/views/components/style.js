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
    templateUrl: "/dist/templates/views/templates/style.html",
    controller: StyleController
  };

  /**
   * It handles component behavior
   * 
   * @param {ColorFactory} ColorFactory - TerraMA² Color generator
   * @param {any} i18n - TerraMA² Internationalization module
   */
  function StyleController($scope, ColorFactory, i18n, DataSeriesService, StyleType, StyleOperation) {
    var self = this;
    // binding component form into parent module in order to expose Form to help during validation
    self.formCtrl = self.form;
    self.DataSeriesType = DataSeriesService.DataSeriesType;
    self.StyleOperation = StyleOperation;
    self.StyleType = StyleType;
    self.i18n = i18n;
    self.generate = generateColors;

    var defaultColorOpts = {
      format: "hex",
      required: true
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
     * It handles color summarization (begin and end) based in list of colors
     */
    $scope.$on("updateStyleColor", function () {
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
     * It generate colors arrays and store in ctrl.colors
     */
    function generateColors() {
      if (self.formCtrl.$invalid) {
        return;
      }

      var colorsArr = ColorFactory.generateColor(self.model.beginColor, self.model.endColor, self.model.bands + 1).reverse();
      for (var i = 1; i < colorsArr.length; ++i) {
        colorsArr[i] = { title: i18n.__("Color") + " " + i, color: colorsArr[i], value: i, isDefault: false };
      }
      var firstColor = colorsArr[0];
      colorsArr[0] = {
        title: i18n.__("Default"),
        color: firstColor,
        isDefault: true,
        value: ""
      };

      self.model.colors = colorsArr;
    }
  }

  // Dependencies Injection
  StyleController.$inject = ["$scope", "ColorFactory", "i18n", "DataSeriesService", "StyleType", "StyleOperation"];
  return terrama2StyleComponent;
});