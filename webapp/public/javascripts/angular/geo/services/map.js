define([], () => {
  /**
   * An abstraction of Map Visualizer. Currently handles **OpenLayers**
   *
   * **This component uses single map instance.**
   *
   * @todo Implement map list in order to have multiple map instances
   */
  class MapService {
    /**
     * @param {angular.$http} $http Angular HTTP module
     * @param {angular.$timeout} $timeout Angular Internal Timeout module
     */
    constructor($http, $timeout) {
      this.$http = $http;
      this.$timeout = $timeout;
      /**
       * List of map instances loaded
       * @type {Map<string, MapContainer>}
       */
      this._maps = { };
    }

    /**
     * Add a drawable Layer into Map scope
     *
     * **It wraps MapContainer#addDrawLayer**
     * @param {string} mapId identifier
     */
    addDrawLayer(mapId) {
      const map = this.getMap(mapId);

      map.addDrawLayer();
    }

    /**
     * Add vector layer from WKT object
     *
     * **It wraps MapContainer#addLayerFromWKT**
     * @param {string} mapId identifier
     * @param {string} layerName WKT Layer name
     * @param {any[]} wkts List of WKTS.
     * @param {string} projection Set projection to the layer
     */
    addLayerFromWKT(mapId, layerName, wkts, projection) {
      const map = this.getMap(mapId);

      map.addLayerFromWKT(layerName, wkts, projection);
    }

    /**
     * Add Map instance to the global scope in order to retrieve through data modules
     *
     * @param {string} id identifier
     * @param {MapContainer} map TerraMA² Map Container instance. See @link ../models/mapContainer.js
     */
    addMap(id, map) {
      this._maps[id] = map;
    }

    /**
     * Get layer by name in map instance
     *
     * **It wraps MapContainer#getLayer**
     * @param {string} mapmapId identifier
     * @param {string} layerName WKT Layer name
     */
    getLayer(mapId, layerName) {
      const map = this.getMap(mapId);

      return map.getLayer(layerName);
    }

    /**
     * Retrieves extent of draw layer
     *
     * **It wraps MapContainer#getExtentDrawLayer**
     * @param {string} mapId identifier
     */
    getExtentOfDrawLayer(mapId) {
      const map = this.getMap(mapId);

      return map.getExtentOfDrawLayer();
    }

    /**
     * Retrieves loaded map
     * @param {string} mapId identifier
     */
    getMap(id) {
      const map = this._maps[id];

      if (!map)
        throw new Error('Map not found');

      return map;
    }

    /**
     * Remove layer by name
     *
     * **It wraps MapContainer#removeLayer**
     * @param {string} mapId identifier
     * @param {string} layerName WKT Layer name
     */
    removeLayer(mapId, layerName) {
      const map = this.getMap(mapId);

      map.removeLayer(layerName);
    }

    /**
     * Remove map from global scope
     *
     * @param {string} mapId identifier
     */
    removeMap(id) {
      this._maps[id] = null;
    }

    /**
     * Zoom the current view to the extent provided.
     *
     * **It wraps MapContainer#zoomTo**
     * @param {string} mapId identifier
     * @param {any[]} extent Vectorial Extent
     * @param {boolean?} animation Should animate? default: false
     */
    zoomTo(mapId, extent, animation = false) {
      const map = this.getMap(mapId);

      map.zoomTo(extent, animation);
    }

    /**
     * Zoom the current view to the extent of layer provided
     *
     * **It wraps MapContainer#zoomToLayer**
     * @param {string} mapId identifier
     * @param {string} layerName WKT Layer name
     */
    zoomToLayer(mapId, layerName) {
      /**
       * @type {Map}
       */
      const map = this.getMap(mapId);

      map.zoomToLayer(layerName)
    }
  }

  MapService.$inject = ['$http', '$timeout'];

  return MapService;
});