define([],function(){

  var terrama2CsvFormatDetailComponent = {
    bindings: {
      format: '<',
      fieldsType: '<',
      onDelete: '&',
      index: '<'
    },
    templateUrl: '/dist/templates/data-series/templates/csvFormatDetail.html',
    controller: FormatDetailController
  };

  function FormatDetailController(i18n) {
    var ctrl = this;
    ctrl.i18n = i18n;

    ctrl.delete = function() {
      ctrl.onDelete({format: ctrl.format});
    };

    ctrl.onFieldTypeChange = function(valueType){
      delete ctrl.format.latitude_property_name;
      delete ctrl.format.longitude_property_name;
      delete ctrl.format.alias;
      delete ctrl.format.property_name;
      delete ctrl.format.format;
    }
  }
  
  FormatDetailController.$inject = ['i18n']; 
  return terrama2CsvFormatDetailComponent;
})