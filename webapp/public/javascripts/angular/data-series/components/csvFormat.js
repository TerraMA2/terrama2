define([],function(){

  var terrama2CsvFormatComponent = {
    bindings: {
      csvFormatData: '<',
    },
    templateUrl: '/dist/templates/data-series/templates/csvFormat.html',
    controller: FormatListController
  };

  function FormatListController($scope, $element, $attrs, i18n) {
    var ctrl = this;
    ctrl.i18n = i18n;

    ctrl.fieldsType = [
      {title: "Point", value: "GEOMETRY_POINT"},
      {title: "Number", value: "FLOAT"},
      {title: "Text", value: "TEXT"},
      {title: "Date", value: "DATETIME"}
    ];

    ctrl.addFormat = function(){
      var newFormat = {type: "GEOMETRY_POINT"};
      ctrl.csvFormatData.fields.push(newFormat);
    }

    ctrl.deleteFormat = function(format) {
      var idx = ctrl.csvFormatData.fields.indexOf(format);
      if (idx >= 0) {
        ctrl.csvFormatData.fields.splice(idx, 1);
      }
    };
  }

  FormatListController.$inject = ['$scope', '$element', '$attrs', 'i18n']; 
  return terrama2CsvFormatComponent;
})