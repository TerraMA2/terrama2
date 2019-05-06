define([
  './mapDirective',
  './../services'
], (map, serviceApp) => {
  const geomodule = 'terrama2.geo.directives';

  angular.module(geomodule, [serviceApp])
    .directive('terrama2Map', map);

  return geomodule;
})