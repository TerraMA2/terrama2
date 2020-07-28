define([
  './../models/mapContainer'
], (MapContainer) => {
  /**
   * Defines OpenLayers map directive for TerraMA2
   *
   * *Requires* @link MapService, OpenLayers
   *
   * @example
   * <terrama2-map id="previewMap" width="400" height="400"></terrama2-map>
   */
  return ['MapService', function(MapService) {
    return {
      restrict: 'EA',
      transclude: true,
      replace: true,
      scope: {

      },
      template: '<div class="internal-terrama2-map" ng-transclude></div>',
      controller: ['$scope', '$q', directiveController],
      link: directiveLink
    }
    /**
     * Bind postLink event to initialize OpenLayers
     *
     * @param {angular.$scope} scope Component scope
     * @param {angular.$element} element Component HTML dom
     * @param {angular.$attrs} attrs Component Attributes
     */
    function directiveLink(scope, element, attrs) {
      // Set width and height if they are defined
      if (attrs.width) {
        if (isNaN(attrs.width)) {
          element.css('width', attrs.width);
        } else {
          element.css('width', attrs.width + 'px');
        }
      }

      const mapId = attrs.id;

      if (!mapId)
        throw new Error(`The map directive requires identifier.`);

      if (attrs.height) {
        if (isNaN(attrs.height)) {
          element.css('height', attrs.height);
        } else {
          element.css('height', attrs.height + 'px');
        }
      }

      const controls = ol.control.defaults();
      const view = new ol.View({
        projection: 'EPSG:4326',
        center: [0, 0],
        zoom: 2
      });

      // Create the Openlayers Map Object with the options
      const olMap = new ol.Map({
        target: element[0],
        controls,
        view,
      });

      const map = new MapContainer(mapId, olMap);

      scope.$on('$destroy', function() {
        olMap.setTarget(null);
        olMap = null;
        MapService.removeMap(mapId);
      });

      // If no layer is defined, set the default tileLayer
      if (!attrs.customLayers) {
        const layer = new ol.layer.Tile({
          source: new ol.source.OSM()
        });

        map.addLayer('OSM', layer);
        olMap.set('default', true);
      }
      // Resolve the map object to the promises
      scope.setMap(map);
      MapService.addMap(mapId, map);
    }

    function directiveController($scope, $q) {
      var _map = $q.defer();
      $scope.getMap = function() {
        return _map.promise;
      };

      $scope.setMap = function(map) {
        _map.resolve(map);
      };

      this.getOpenlayersScope = function() {
        return $scope;
      };
    }
  }]
});