define([],function(){
  /**
   * It defines a Component structure for handling CSV format form on Dynamic data registration
   * 
   * @property {Object} bindings - Defines component bindings to work
   */

  var terrama2CsvFormatComponent = {
    bindings: {
      csvFormatData: '<',
    },
    templateUrl: '/dist/templates/data-series/templates/csvFormat.html',
    controller: CsvFormatController
  };

  /**
   * It handles component behavior
   * 
   * @param {any} i18n - TerraMA² Internationalization module
   */
  function CsvFormatController($scope, $element, $attrs, i18n) {
    var ctrl = this;
    ctrl.i18n = i18n;

    // Types of fields in csv file
    ctrl.fieldsType = [
      {title: "Point (Geometry)", value: "GEOMETRY_POINT"},
      {title: "Number", value: "FLOAT"},
      {title: "Text", value: "TEXT"},
      {title: "Date", value: "DATETIME"}
    ];

    // Function to add new format in model
    ctrl.addField = function(){
      var newFormat = {type: "GEOMETRY_POINT"};
      ctrl.csvFormatData.fields.push(newFormat);
    }

    // Function to remove 
    ctrl.deleteField = function(format) {
      var idx = ctrl.csvFormatData.fields.indexOf(format);
      if (idx >= 0) {
        ctrl.csvFormatData.fields.splice(idx, 1);
      }
    };
  }

  CsvFormatController.$inject = ['$scope', '$element', '$attrs', 'i18n']; 
  return terrama2CsvFormatComponent;
})