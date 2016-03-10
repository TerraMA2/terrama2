"use strict";

/**
 * Class responsible for presenting the map.
 * @module MapDisplay
 *
 * @property {ol.interaction.DragBox} memberZoomDragBox - DragBox object.
 * @property {array} memberInitialExtent - Initial extent.
 * @property {ol.Map} memberOlMap - Map object.
 * @property {int} memberResolutionChangeEventKey - Resolution change event key.
 */
TerraMA2WebComponents.webcomponents.MapDisplay = (function() {

  // DragBox object
  var memberZoomDragBox = null;
  // Initial extent
  var memberInitialExtent = null;
  // Map object
  var memberOlMap = new ol.Map({
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
  // Resolution change event key
  var memberResolutionChangeEventKey = null;

  /**
   * Returns the map object.
   * @returns {ol.Map} memberOlMap - Map object
   *
   * @function getMap
   */
  var getMap = function() {
    return memberOlMap;
  };

  /**
   * Updates the map size accordingly to its container.
   *
   * @function updateMapSize
   */
  var updateMapSize = function() {
    var interval = window.setInterval(function() { memberOlMap.updateSize(); }, 10);
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
    memberOlMap.addLayer(
      createTileWMS(url, type, layerName, layerTitle, layerVisible, listOnLayerExplorer)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(memberOlMap);
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
   * @param {function} styleFunction - Function responsible for attributing the colors to the layer features
   * @returns {ol.layer.Vector} new ol.layer.Vector - New GeoJSON vector layer
   *
   * @private
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

  /**
   * Adds a new GeoJSON vector layer to the map.
   * @param {string} url - Url to the wms layer
   * @param {string} layerName - Layer name
   * @param {string} layerTitle - Layer title
   * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
   * @param {boolean} listOnLayerExplorer - Flag that indicates if the layer should be listed on the layer explorer
   * @param {array} fillColors - Array with the fill colors
   * @param {array} strokeColors - Array with the stroke colors
   * @param {function} styleFunction - Function responsible for attributing the colors to the layer features
   *
   * @function addGeoJSONVectorLayer
   */
  var addGeoJSONVectorLayer = function(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction) {
    memberOlMap.addLayer(
      createGeoJSONVector(url, layerName, layerTitle, layerVisible, listOnLayerExplorer, fillColors, strokeColors, styleFunction)
    );

    TerraMA2WebComponents.webcomponents.LayerExplorer.resetLayerExplorer(memberOlMap);
  };

  /**
   * Creates a new Openlayers Style object.
   * @param {string} fill - Layer fill color
   * @param {string} stroke - Layer stroke color
   * @returns {ol.style.Style} new ol.style.Style - New Openlayers Style object
   *
   * @private
   * @function createStyle
   */
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

  /**
   * Sets the visibility of a given layer or layer group, if it is visible, it will be hidden, otherwise will be shown.
   * @param {ol.layer} layer - Layer or layer group
   *
   * @function setLayerVisibility
   */
  var setLayerVisibility = function(layer) {
    layer.setVisible(!layer.getVisible());

    if(layer.getLayers) {
      var layers = layer.getLayers().getArray();
      var len = layers.length;
      for(var i = 0; i < len; i++) {
        layers[i].setVisible(layer.getVisible());
      }
    }
  };

  /**
   * Sets the visibility of a given layer or layer group by its name.
   * @param {string} layerName - Layer name
   * @param {boolean} visibilityFlag - Visibility flag, true to show and false to hide
   *
   * @function setLayerVisibilityByName
   */
  var setLayerVisibilityByName = function(layerName, visibilityFlag) {
    var layer = findBy(memberOlMap.getLayerGroup(), 'name', layerName);
    layer.setVisible(visibilityFlag);

    if(layer.getLayers) {
      var layers = layer.getLayers().getArray();
      var len = layers.length;
      for(var i = 0; i < len; i++) {
        layers[i].setVisible(visibilityFlag);
      }
    }
  };

  /**
   * Adds the Zoom DragBox to the map.
   *
   * @function addZoomDragBox
   */
  var addZoomDragBox = function() {
    memberOlMap.addInteraction(memberZoomDragBox);
  };

  /**
   * Removes the Zoom DragBox from the map.
   *
   * @function removeZoomDragBox
   */
  var removeZoomDragBox = function() {
    memberOlMap.removeInteraction(memberZoomDragBox);
  };

  /**
   * Returns the current Zoom DragBox extent.
   * @returns {array} extent - Zoom DragBox extent
   *
   * @function getZoomDragBoxExtent
   */
  var getZoomDragBoxExtent = function() {
    return memberZoomDragBox.getGeometry().getExtent();
  };

  /**
   * Sets the Zoom DragBox start event.
   * @param {function} eventFunction - Function to be executed when the event is triggered
   *
   * @function setZoomDragBoxStart
   */
  var setZoomDragBoxStart = function(eventFunction) {
    memberZoomDragBox.on('boxstart', eventFunction);
  };

  /**
   * Sets the Zoom DragBox end event.
   * @param {function} eventFunction - Function to be executed when the event is triggered
   *
   * @function setZoomDragBoxEnd
   */
  var setZoomDragBoxEnd = function(eventFunction) {
    memberZoomDragBox.on('boxend', eventFunction);
  };

  /**
   * Returns the current map extent.
   * @returns {array} extent - Map extent
   *
   * @function getCurrentExtent
   */
  var getCurrentExtent = function() {
    return memberOlMap.getView().calculateExtent(memberOlMap.getSize());
  };

  /**
   * Zooms to the initial map extent.
   *
   * @function zoomToInitialExtent
   */
  var zoomToInitialExtent = function() {
    memberOlMap.getView().fit(memberInitialExtent, memberOlMap.getSize());
  };

  /**
   * Zooms to the received extent.
   * @param {array} extent - Extent
   *
   * @function zoomToExtent
   */
  var zoomToExtent = function(extent) {
    memberOlMap.getView().fit(extent, memberOlMap.getSize(), { constrainResolution: false });
  };

  /**
   * Finds a layer by a given key.
   * @param {ol.layer.Group} layer - The layer group where the method will run the search
   * @param {string} key - Layer attribute to be used in the search
   * @param {string} value - Value to be used in the search
   * @returns {ol.layer} layer - Layer found
   *
   * @function findBy
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
   * Applies a given CQL filter to a given layer.
   * @param {string} cql - CQL filter to be applied
   * @param {string} layerName - Layer name to be filtered
   *
   * @function applyCQLFilter
   */
  var applyCQLFilter = function(cql, layerName) {
    findBy(memberOlMap.getLayerGroup(), 'name', layerName).getSource().updateParams({ "CQL_FILTER": cql });
  };

  /**
   * Sets the Map resolution change event.
   * @param {function} eventFunction - Function to be executed when the event is triggered
   *
   * @function setMapResolutionChange
   */
  var setMapResolutionChange = function(eventFunction) {
    if(memberResolutionChangeEventKey !== null) memberOlMap.getView().unByKey(memberResolutionChangeEventKey);
    memberResolutionChangeEventKey = memberOlMap.getView().on('propertychange', function(e) {
      switch(e.key) {
        case 'resolution':
          eventFunction(e);
          break;
      }
    });
  };

  /**
   * Returns the current map resolution.
   * @returns {float} resolution - Map resolution
   *
   * @function getCurrentResolution
   */
  var getCurrentResolution = function() {
    return memberOlMap.getView().getResolution();
  };

  /**
   * Initializes the necessary features.
   *
   * @function init
   */
  var init = function() {
    memberOlMap.getLayerGroup().set('name', 'root');
    memberOlMap.getLayerGroup().set('title', 'Geoserver Local');
    var zoomslider = new ol.control.ZoomSlider();
    memberOlMap.addControl(zoomslider);

    memberZoomDragBox = new ol.interaction.DragBox({
      condition: ol.events.condition.always
    });

    memberInitialExtent = memberOlMap.getView().calculateExtent(memberOlMap.getSize());

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
    setLayerVisibility: setLayerVisibility,
    setLayerVisibilityByName: setLayerVisibilityByName,
    addZoomDragBox: addZoomDragBox,
    removeZoomDragBox: removeZoomDragBox,
    getZoomDragBoxExtent: getZoomDragBoxExtent,
    setZoomDragBoxStart: setZoomDragBoxStart,
    setZoomDragBoxEnd: setZoomDragBoxEnd,
    getCurrentExtent: getCurrentExtent,
    zoomToInitialExtent: zoomToInitialExtent,
    zoomToExtent: zoomToExtent,
  	findBy: findBy,
    applyCQLFilter: applyCQLFilter,
    setMapResolutionChange: setMapResolutionChange,
    getCurrentResolution: getCurrentResolution,
  	init: init
  };
})();
