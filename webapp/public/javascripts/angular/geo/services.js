define([], function() {
  'use strict';
  
  var moduleName = "terrama2.components.geo.services";

  angular.module(moduleName, [])
    .service("FilterForm", FilterForm);

  function FilterForm(){

  }

  return moduleName;
});