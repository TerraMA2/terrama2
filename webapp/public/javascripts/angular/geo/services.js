define([
  './services/map'
], function(MapService) {
  'use strict';

  var moduleName = "terrama2.components.geo.services";

  angular.module(moduleName, [])
    .service("FilterForm", FilterForm)
    .service('MapService', MapService);

  function FilterForm(){

  }

  return moduleName;
});