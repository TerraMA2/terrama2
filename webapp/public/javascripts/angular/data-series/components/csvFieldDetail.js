define([],function(){
  /**
   * It defines a Component structure for handling CSV Field form on Dynamic data registration
   * 
   * @property {Object} bindings - Defines component bindings to work
   */

  var terrama2CsvFieldDetailComponent = {
    bindings: {
      field: '<',
      fieldsType: '<',
      onDelete: '&'
    },
    templateUrl: '/dist/templates/data-series/templates/csvFieldDetail.html',
    controller: FieldDetailController
  };

  /**
   * It handles component behavior
   * 
   * @param {any} i18n - TerraMA² Internationalization module
   */
  function FieldDetailController(i18n) {
    var ctrl = this;
    ctrl.i18n = i18n;

    // Call delete function from parent
    ctrl.delete = function() {
      ctrl.onDelete({field: ctrl.field});
    };

    // Reset field model on type change
    ctrl.onFieldTypeChange = function(valueType){
      delete ctrl.field.latitude_property_name;
      delete ctrl.field.longitude_property_name;
      delete ctrl.field.alias;
      delete ctrl.field.property_name;
      delete ctrl.field.format;
    }
  }
  
  FieldDetailController.$inject = ['i18n']; 
  return terrama2CsvFieldDetailComponent;
})