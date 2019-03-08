define([
  'TerraMA2WebApp/geo/components/map',
  './../services'
], (map, serviceApp) => {
// import serviceApp from './../services';
// import map from './../components/map'


  const geomodule = 'terrama2.geo.components';

  angular.module(geomodule, [serviceApp])
    .component('terrama2Map', map);

  return geomodule;
  // export default geomodule;
})