"use strict";

/**
 * Class responsible for presenting the map.
 * @module MapDisplay
 *
 * @property {ol.interaction.DragBox} zoomDragBox - DragBox object.
 * @property {array} initialExtent - Initial extent.
 * @property {ol.Map} olMap - Map object.
 */
TerraMA2WebComponents.webcomponents.MapDisplay = (function() {

  // DragBox object
  var zoomDragBox = null;
  // Initial extent
  var initialExtent = null;
  // Map object
  var olMap = new ol.Map({
    renderer: 'canvas',
    layers: [
      new ol.layer.Group({
        layers: [
          new ol.layer.Tile({
            source: new ol.source.OSM(),
            name: 'osm',
            title: 'Open Street Map',
            visible: false,
            listOnLayerExplorer: true
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'osm'}),
            name: 'mapquest_osm',
            title: 'MapQuest OSM',
            visible: false,
            listOnLayerExplorer: true
          }),
          new ol.layer.Tile({
            source: new ol.source.MapQuest({layer: 'sat'}),
            name: 'mapquest_sat',
            title: 'MapQuest Sat&eacute;lite',
            visible: true,
            listOnLayerExplorer: true
          })
        ],
        name: 'bases',
        title: 'Camadas Base',
        listOnLayerExplorer: true
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
   * Returns the map object.
   * @returns {ol.Map} olMap - Map object
   *
   * @function getMap
   */
  var getMap = function() {
    return olMap;
  };

  /**
   * Updates the map size accordingly to its container.
   *
   * @function updateMapSize
   */
  var updateMapSize = function() {
    var interval = window.setInterval(function() { olMap.updateSize(); }, 10);
    window.setTimeout(function() { clearInterval(interval); }, 300);
  };

  /**
   * Creates a new tiled wms layer.
   * @param {string} url - Url to the wms layer
   * @param {string} type - Server type
   * @param {string} layerName - Layer name
   * @param {string} layerTitle - Layer title
   * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
   * @param {boolean} listOnLayerExplorer - Flag that indicates if the layer should be listed on the layer explorer
   * @returns {ol.layer.Tile} new ol.layer.Tile - New tiled wms layer
   *
   * @function createTileWMS
   */
  var createTileWMS = function(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer) {
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
      visible: layerVisible,
      listOnLayerExplorer: listOnLayerExplorer
    });
  };

  /**
   * Adds a new tiled wms layer to the map.
   * @param {string} url - Url to the wms layer
   * @param {string} type - Server type
   * @param {string} layerName - Layer name
   * @param {string} layerTitle - Layer title
   * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
   * @param {boolean} listOnLayerExplorer - Flag that indicates if the layer should be listed on the layer explorer
   *
   * @function addTileWMSLayer
   */
  var addTileWMSLayer = function(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer) {
    olMap.addLayer(
      createTileWMS(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(olMap);
  };

  /**
   * Creates a new GeoJSON vector layer.
   * @param {string} url - Url to the wms layer
   * @param {string} layerName - Layer name
   * @param {string} layerTitle - Layer title
   * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
   * @param {boolean} listOnLayerExplorer - Flag that indicates if the layer should be listed on the layer explorer
   * @param {array} fillColors - Array with the fill colors
   * @param {array} strokeColors - Array with the stroke colors
   * @param {function} styleFunction - Function create
   * @returns {ol.layer.Vector} new ol.layer.Vector - New GeoJSON vector layer
   *
   * @function createGeoJSONVector
   */
  var createGeoJSONVector = function(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction) {
    return new ol.layer.Vector({
      source: new ol.source.Vector({
        url: url,
        format: new ol.format.GeoJSON(),
        strategy: ol.loadingstrategy.bbox
      }),
      style: function(feature) {
        var colors = styleFunction(feature, fillColors, strokeColors);
        return createStyle(colors.fillColor, colors.strokeColor);
      },
      name: layerName,
      title: layerTitle,
      visible: layerVisible,
      listOnLayerExplorer: listOnLayerExplorer
    });
  };

  var addGeoJSONVectorLayer = function(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction) {
    olMap.addLayer(
      createGeoJSONVector(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(olMap);
  };

  var createStyle = function(fill, stroke) {
    return new ol.style.Style({
      fill: new ol.style.Fill({
        color: fill
      }),
      stroke: new ol.style.Stroke({
        color: stroke,
        width: 2
      })
    });
  };

  var addZoomDragBox = function() {
    olMap.addInteraction(zoomDragBox);
  };

  var removeZoomDragBox = function() {
    olMap.removeInteraction(zoomDragBox);
  };

  var getZoomDragBoxExtent = function() {
    return zoomDragBox.getGeometry().getExtent();
  };

  var setZoomDragBoxStart = function(eventFunction) {
    zoomDragBox.on('boxstart', eventFunction);
  };

  var setZoomDragBoxEnd = function(eventFunction) {
    zoomDragBox.on('boxend', eventFunction);
  };

  var getCurrentExtension = function() {
    return olMap.getView().calculateExtent(olMap.getSize());
  };

  var zoomToInitialExtent = function() {
    olMap.getView().fit(initialExtent, olMap.getSize());
  };

  var zoomToExtent = function(extent) {
    olMap.getView().fit(extent, olMap.getSize(), { constrainResolution: false });
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
   * Apply a given CQL filter to a given layer
   * @param {string} cql - CQL filter to be applied
   * @param {string} layerName - layer name to be filtered
   */
  var applyCQLFilter = function(cql, layerName) {
    findBy(olMap.getLayerGroup(), 'name', layerName).getSource().updateParams({ "CQL_FILTER": cql });
  };

  var init = function() {
    olMap.getLayerGroup().set('name', 'root');
    olMap.getLayerGroup().set('title', 'Geoserver Local');
    var zoomslider = new ol.control.ZoomSlider();
    olMap.addControl(zoomslider);

    zoomDragBox = new ol.interaction.DragBox({
      condition: ol.events.condition.always
    });

    initialExtent = olMap.getView().calculateExtent(olMap.getSize());

    $(document).ready(function() {
      updateMapSize();
    });
  };

  return {
    getMap: getMap,
  	updateMapSize: updateMapSize,
  	createTileWMS: createTileWMS,
    addTileWMSLayer: addTileWMSLayer,
    addGeoJSONVectorLayer: addGeoJSONVectorLayer,
    addZoomDragBox: addZoomDragBox,
    removeZoomDragBox: removeZoomDragBox,
    getZoomDragBoxExtent: getZoomDragBoxExtent,
    setZoomDragBoxStart: setZoomDragBoxStart,
    setZoomDragBoxEnd: setZoomDragBoxEnd,
    getCurrentExtension: getCurrentExtension,
    zoomToInitialExtent: zoomToInitialExtent,
    zoomToExtent: zoomToExtent,
  	findBy: findBy,
    applyCQLFilter: applyCQLFilter,
  	init: init
  };
})();
