"use strict";

/** @class MapDisplay - Class responsible for presenting the map. */
TerraMA2WebComponents.webcomponents.MapDisplay = (function() {

  var dragBox = null;
  var initialExtent = null;

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

  var addTileWMSLayer = function(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer) {
    olMap.addLayer(
      createTileWMS(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(olMap);
  };

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

  var addGeoJSONVectorLayer = function(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction) {
    olMap.addLayer(
      createGeoJSONVector(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(olMap);
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

  /**
   * Update the map size according to its container
   */
  var updateMapSize = function() {
    var interval = window.setInterval(function() { olMap.updateSize(); }, 10);
    window.setTimeout(function() { clearInterval(interval); }, 300);
  };

  var addDragBox = function() {
    olMap.addInteraction(dragBox);
  };

  var removeDragBox = function() {
    olMap.removeInteraction(dragBox);
  };

  var getDragBoxExtent = function() {
    return dragBox.getGeometry().getExtent();
  };

  var setDragBoxStart = function(eventFunction) {
    dragBox.on('boxstart', eventFunction);
  };

  var setDragBoxEnd = function(eventFunction) {
    dragBox.on('boxend', eventFunction);
  };

  var zoomToExtent = function(extent) {
    olMap.getView().fit(extent, olMap.getSize(), { constrainResolution: false });
  };

  var zoomToInitialExtent = function() {
    olMap.getView().fit(initialExtent, olMap.getSize());
  };

  var getCurrentExtension = function() {
    return olMap.getView().calculateExtent(olMap.getSize());
  };

  var init = function() {
    olMap.getLayerGroup().set('name', 'root');
    olMap.getLayerGroup().set('title', 'Geoserver Local');

    $("#terrama2-map").find('div.ol-zoom').removeClass('ol-zoom').addClass('terrama2-map-simple-zoom');
    $("#terrama2-map").find('div.ol-attribution').addClass('terrama2-map-attribution');

    dragBox = new ol.interaction.DragBox({
      condition: ol.events.condition.always
    });

    initialExtent = olMap.getView().calculateExtent(olMap.getSize());

    $(document).ready(function() {
      updateMapSize();
    });
  };

  return {
    getMap: getMap,
  	createTileWMS: createTileWMS,
  	findBy: findBy,
  	updateMapSize: updateMapSize,
    addGeoJSONVectorLayer: addGeoJSONVectorLayer,
    applyCQLFilter: applyCQLFilter,
    addDragBox: addDragBox,
    removeDragBox: removeDragBox,
    zoomToExtent: zoomToExtent,
    setDragBoxStart: setDragBoxStart,
    setDragBoxEnd: setDragBoxEnd,
    getDragBoxExtent: getDragBoxExtent,
    zoomToInitialExtent: zoomToInitialExtent,
    getCurrentExtension: getCurrentExtension,
  	init: init
  };
})();
