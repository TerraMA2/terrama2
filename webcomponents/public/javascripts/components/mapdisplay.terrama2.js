"use strict";

/** @class MapDisplay - Class responsible for presenting the map. */
TMA2WebComponents.components.MapDisplay = (function() {

  var olMap = new ol.Map({
    renderer: 'canvas',
    layers: [
      new ol.layer.Group({
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM(),
            name: 'osm',
            title: 'Open Street Map',
            visible: false
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'osm'}),
            name: 'mapquest_osm',
            title: 'MapQuest OSM',
            visible: false
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'sat'}),
            name: 'mapquest_sat',
            title: 'MapQuest Sat&eacute;lite',
            visible: true
          })
        ],
        name: 'bases',
        title: 'Camadas Base'
      })
    ],
    target: 'terrama2-map',
    view: new ol.View({
      projection: 'EPSG:4326',
      center: [-55, -15],
      zoom: 4
    })
  });

  /**
   * Return the map object
   * @returns {ol.Map} olMap - map object
   */
  var getMap = function() {
    return olMap;
  };

  /**
   * Create a new tiled wms layer
   * @param {string} url - url to the wms layer
   * @param {string} type - server type
   * @param {string} layerName - layer name
   * @param {string} layerTitle - layer title
   * @returns {ol.layer.Tile} olMap - new tiled wms layer
   */
  var createTileWMS = function(url, type, layerName, layerTitle) {
    return new ol.layer.Tile({
      source: new ol.source.TileWMS({
        preload: Infinity,
        url: url,
        serverType: type,
        params: {
          'LAYERS': layerName, 'TILED': true
        }
      }),
      name: layerName,
      title: layerTitle,
      visible: false
    });
  };

  /**
   * Find a layer by a given key
   * @param {ol.layer.Group} layer - the layer group where the method will run the search
   * @param {string} key - layer attribute to be used in the search
   * @param {string} value - value to be used in the search
   * @returns {ol.layer} found layer
   */
  var findBy = function(layer, key, value) {
    if(layer.get(key) === value) {
      return layer;
    }

    if(layer.getLayers) {
      var layers = layer.getLayers().getArray(),
      len = layers.length, result;
      for (var i = 0; i < len; i++) {
        result = findBy(layers[i], key, value);
        if (result) {
          return result;
        }
      }
    }

    return null;
  };

  /**
   * Update the map size according to its container
   */
  var updateMapSize = function() {
    var interval = window.setInterval(function() { olMap.updateSize(); }, 10);
    window.setTimeout(function() { clearInterval(interval); }, 300);
  };

  var init = function() {
    olMap.getLayerGroup().set('name', 'root');
    olMap.getLayerGroup().set('title', 'Geoserver Local');

    $("#terrama2-map").find('div.ol-zoom').removeClass('ol-zoom').addClass('terrama2-map-simple-zoom');
    $("#terrama2-map").find('div.ol-attribution').addClass('terrama2-map-attribution');

    $(document).ready(function() {
      updateMapSize();
    });
  };

  return {
    getMap: getMap,
  	createTileWMS: createTileWMS,
  	findBy: findBy,
  	updateMapSize: updateMapSize,
  	init: init
  };
})();
