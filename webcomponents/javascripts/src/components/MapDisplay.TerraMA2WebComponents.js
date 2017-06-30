"use strict";

/**
 * Class responsible for presenting the map.
 * @class MapDisplay
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {ol.interaction.DragBox} memberZoomDragBox - DragBox object.
 * @property {array} memberInitialExtent - Initial extent.
 * @property {object} memberParser - Capabilities parser.
 * @property {ol.Map} memberOlMap - Map object.
 * @property {int} memberResolutionChangeEventKey - Resolution change event key.
 * @property {int} memberDoubleClickEventKey - Double click event key.
 * @property {int} memberSingleClickEventKey - Single click event key.
 * @property {object} memberSocket - Socket object.
 * @property {function} memberLayersStartLoadingFunction - Function triggered when a layer starts to load
 * @property {function} memberLayersEndLoadingFunction - Function triggered when a layer is loaded
 * @property {function} memberLoading - Loading layers counters
 * @property {function} memberLoaded - Loaded layers counters
 */
define(
  function() {

    // DragBox object
    var memberZoomDragBox = null;
    // Initial extent
    var memberInitialExtent = null;
    // Capabilities parser
    var memberParser = null;
    // Map object
    var memberOlMap = new ol.Map({
      renderer: ["canvas", "dom"],
      target: 'terrama2-map',
      view: new ol.View({ center: [-55, -15], zoom: 3, maxResolution: 0.5625, extent: [-180, -90, 180, 90], projection: ol.proj.get("EPSG:4326") })
    });
    // Resolution change event key
    var memberResolutionChangeEventKey = null;
    // Double click event key
    var memberDoubleClickEventKey = null;
    // Single click event key
    var memberSingleClickEventKey = null;
    // Socket object
    var memberSocket = null;
    // Function triggered when a layer starts to load
    var memberLayersStartLoadingFunction = null;
    // Function triggered when a layer is loaded
    var memberLayersEndLoadingFunction = null;
    // Loading layers counters
    var memberLoading = {};
    // Loaded layers counters
    var memberLoaded = {};

    /**
     * Returns the map object.
     * @returns {ol.Map} memberOlMap - Map object
     *
     * @function getMap
     * @memberof MapDisplay
     * @inner
     */
    var getMap = function() {
      return memberOlMap;
    };

    /**
     * Updates the map size accordingly to its container.
     *
     * @function updateMapSize
     * @memberof MapDisplay
     * @inner
     */
    var updateMapSize = function() {
      var interval = window.setInterval(function() { memberOlMap.updateSize(); }, 100);
      window.setTimeout(function() { clearInterval(interval); }, 2000);
    };

    /**
     * Updates the parameters of the source of a given layer.
     * @param {string} layerId - Layer id
     * @param {object} params - New layer source parameters
     * @param {boolean} refresh - Flag that indicates if the source should be refreshed
     *
     * @function updateLayerSourceParams
     * @memberof MapDisplay
     * @inner
     */
    var updateLayerSourceParams = function(layerId, params, refresh) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      layer.getSource().updateParams(params);
      if(refresh) layer.getSource().refresh();
    };

    /**
     * Updates the time of a given layer.
     * @param {string} layerId - Layer id
     * @param {string} newTime - Layer new time
     * @param {object} options - Necessary options when the layer has a WMTS source, in case the the parameter is not provided it will be considered that the layer does not has a WMTS source, when provided should be an object as follows: { url: <URL>, format: <FORMAT>, matrixSet: <MATRIX_SET>, tileGrid: <TILE_GRID> }
     *
     * @function updateLayerTime
     * @memberof MapDisplay
     * @inner
     */
    var updateLayerTime = function(layerId, newTime, options) {
      if(options !== undefined && options !== null) {
        var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
        layer.setSource(createWMTSSource(options.url, layerId, newTime, options.format, options.matrixSet, options.tileGrid));
      } else {
        updateLayerSourceParams(layerId, { TIME: newTime }, true);
      }
    };

    /**
     * Updates the opacity of the layer, allowed values range from 0 to 1.
     * @param {string} layerId - Layer id
     * @param {number} opacityValue = Opacity value to set in layer
     *
     * @function updateLayerOpacity
     * @memberof MapDisplay
     * @inner
     */
    var updateLayerOpacity = function(layerId, opacityValue){
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      layer.setOpacity(opacityValue);
    }

    /**
     * Get the opacity of the layer.
     * @param {string} layerId - Layer id
     *
     * @function getLayerOpacity
     * @memberof MapDisplay
     * @inner
     */
    var getLayerOpacity = function(layerId){
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      return layer.getOpacity();
    }

    /**
     * Updates a given attribute of a given layer with a given value.
     * @param {string} layerId - Layer id
     * @param {string} key - Attribute name
     * @param {string} value - Attribute new value
     *
     * @function updateLayerAttribute
     * @memberof MapDisplay
     * @inner
     */
    var updateLayerAttribute = function(layerId, key, value) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      layer.set(key, value, false);
    };

    /**
     * Corrects the longitude of the map, if it's wrong. That's necessary because Openlayers 3 (in the current version) has a bug, when the map is moved to the right or to the left the X coordinate keeps growing.
     * @param {float} longitude - Original longitude
     * @returns {float} correctedLongitude - Corrected longitude
     *
     * @private
     * @function correctLongitude
     * @memberof MapDisplay
     * @inner
     */
    var correctLongitude = function(longitude) {
      // Variable that will keep the corrected longitude
      var correctedLongitude = parseFloat(longitude);
      // Variable that will keep the original longitude
      var originalLongitude = parseFloat(longitude);

      // The correction is executed only if the longitude is incorrect
      if(originalLongitude > 180 || originalLongitude <= -180) {

        // If the longitude is negative, it's converted to a positive float, otherwise just to a float
        longitude = originalLongitude < 0 ? longitude * -1 : parseFloat(longitude);

        // Division of the longitude by 180:
        //   If the result is an even negative integer, nothing is added, subtracted or rounded
        //   If the result is an odd negative integer, is added 1 to the result
        //   If the result is a positive integer, is subtracted 1 from the result
        //   If isn't integer but its integer part is even, it's rounded down
        //   Otherwise, it's rounded up
        var divisionResult = 0;
        if((originalLongitude / 180) % 2 === -0)
          divisionResult = longitude / 180;
        else if((originalLongitude / 180) % 2 === -1)
          divisionResult = (longitude / 180) + 1;
        else if((longitude / 180) % 1 === 0)
          divisionResult = (longitude / 180) - 1;
        else if(parseInt(longitude / 180) % 2 === 0)
          divisionResult = parseInt(longitude / 180);
        else
          divisionResult = Math.ceil(longitude / 180);

        // If the division result is greater than zero, the correct longitude is calculated:
        //   If the original longitude is negative, the division result multiplied by 180 is added to it
        //   Otherwise, the division result multiplied by 180 is subtracted from it
        if(divisionResult > 0)
          correctedLongitude = (originalLongitude < 0) ? originalLongitude + (divisionResult * 180) : originalLongitude - (divisionResult * 180);
      }

      return correctedLongitude;
    };

    /**
     * Converts a coordinate from decimal format to DMS.
     * @param {string} coordinate - Coordinate to be converted
     * @param {string} type - Coordinate type
     * @returns {string} dmsCoordinate - Converted coordinate
     *
     * @private
     * @function convertCoordinateToDMS
     * @memberof MapDisplay
     * @inner
     */
    var convertCoordinateToDMS = function(coordinate, type) {
      var absCoordinate = Math.abs(coordinate);
      var coordinateDegrees = Math.floor(absCoordinate);

      var coordinateMinutes = (absCoordinate - coordinateDegrees) / (1 / 60);
      var tempCoordinateMinutes = coordinateMinutes;
      coordinateMinutes = Math.floor(coordinateMinutes);
      var coordinateSeconds = (tempCoordinateMinutes - coordinateMinutes) / (1 / 60);
      coordinateSeconds =  Math.round(coordinateSeconds * 10);
      coordinateSeconds /= 10;

      if(coordinateDegrees < 10)
        coordinateDegrees = "0" + coordinateDegrees;

      if(coordinateMinutes < 10)
        coordinateMinutes = "0" + coordinateMinutes;

      if(coordinateSeconds < 10)
        coordinateSeconds = "0" + coordinateSeconds;

      var dmsCoordinate = coordinateDegrees + " " + coordinateMinutes + " " + coordinateSeconds + " " + getCoordinateHemisphereAbbreviation(coordinate, type);

      return dmsCoordinate;
    };

    /**
     * Returns the hemisphere abbreviation for the given coordinate.
     * @param {string} coordinate - Coordinate
     * @param {string} type - Coordinate type
     * @returns {string} coordinateHemisphereAbbreviation - Hemisphere abbreviation
     *
     * @private
     * @function getCoordinateHemisphereAbbreviation
     * @memberof MapDisplay
     * @inner
     */
    var getCoordinateHemisphereAbbreviation = function(coordinate, type) {
      var coordinateHemisphereAbbreviation = "";

      if(type == 'LAT')
        coordinateHemisphereAbbreviation = coordinate >= 0 ? "N" : "S";
      else if(type == 'LON')
        coordinateHemisphereAbbreviation = coordinate >= 0 ? "E" : "W";

      return coordinateHemisphereAbbreviation;
    };

    /**
     * Adds a mouse position display in the map.
     *
     * @function addMousePosition
     * @memberof MapDisplay
     * @inner
     */
    var addMousePosition = function() {
      var controlAlreadyExists = false;

      memberOlMap.getControls().forEach(function(control, i) {
        if(control instanceof ol.control.MousePosition) {
          controlAlreadyExists = true;
          return;
        }
      });

      if(!controlAlreadyExists) {
        $("#terrama2-map").append('<div id="terrama2-map-info"></div>');

        var mousePositionControl = new ol.control.MousePosition({
          coordinateFormat: (function(precision) {
            return (function(coordinates) {
              return ol.coordinate.toStringXY([correctLongitude(coordinates[0]), coordinates[1]], precision) + "<br/>" + convertCoordinateToDMS(correctLongitude(coordinates[0]), 'LON') + ", " + convertCoordinateToDMS(coordinates[1], 'LAT');
            });
          })(6),
          projection: 'EPSG:4326',
          className: 'terrama2-mouse-position',
          target: document.getElementById('terrama2-map-info')
        });

        memberOlMap.addControl(mousePositionControl);
      }
    };

    /**
     * Removes the mouse position display from the map.
     *
     * @function removeMousePosition
     * @memberof MapDisplay
     * @inner
     */
    var removeMousePosition = function() {
      $("#terrama2-map-info").remove();

      memberOlMap.getControls().forEach(function(control, i) {
        if(control instanceof ol.control.MousePosition) {
          memberOlMap.removeControl(control);
          return;
        }
      });
    };

    /**
     * Adds a scale display in the map.
     *
     * @function addScale
     * @memberof MapDisplay
     * @inner
     */
    var addScale = function() {
      var controlAlreadyExists = false;

      memberOlMap.getControls().forEach(function(control, i) {
        if(control instanceof ol.control.ScaleLine) {
          controlAlreadyExists = true;
          return;
        }
      });

      if(!controlAlreadyExists)
        memberOlMap.addControl(new ol.control.ScaleLine());
    };

    /**
     * Removes the scale display from the map.
     *
     * @function removeScale
     * @memberof MapDisplay
     * @inner
     */
    var removeScale = function() {
      memberOlMap.getControls().forEach(function(control, i) {
        if(control instanceof ol.control.ScaleLine) {
          memberOlMap.removeControl(control);
          return;
        }
      });
    };

    /**
     * Enables the double click zoom.
     *
     * @function enableDoubleClickZoom
     * @memberof MapDisplay
     * @inner
     */
    var enableDoubleClickZoom = function() {
      var interactionAlreadyExists = false;

      memberOlMap.getInteractions().forEach(function(interaction, i) {
        if(interaction instanceof ol.interaction.DoubleClickZoom) {
          interactionAlreadyExists = true;
          return;
        }
      });

      if(!interactionAlreadyExists)
        memberOlMap.addInteraction(new ol.interaction.DoubleClickZoom());
    };

    /**
     * Disables the double click zoom.
     *
     * @function disableDoubleClickZoom
     * @memberof MapDisplay
     * @inner
     */
    var disableDoubleClickZoom = function() {
      memberOlMap.getInteractions().forEach(function(interaction, i) {
        if(interaction instanceof ol.interaction.DoubleClickZoom) {
          memberOlMap.removeInteraction(interaction);
          return;
        }
      });
    };

    /**
     * Creates a new layer group.
     * @param {string} id - Layer group id
     * @param {string} name - Layer group name
     * @returns {ol.layer.Group} layerGroup - New layer group
     *
     * @private
     * @function createLayerGroup
     * @memberof MapDisplay
     * @inner
     */
    var createLayerGroup = function(id, name) {
      var layerGroup = new ol.layer.Group({
        id: id,
        name: name
      });

      return layerGroup;
    };

    /**
     * Adds a new layer group to the map.
     * @param {string} id - Layer group id
     * @param {string} name - Layer group name
     * @param {string} parentGroup - Parent group id
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addLayerGroup
     * @memberof MapDisplay
     * @inner
     */
    var addLayerGroup = function(id, name, parentGroup) {
      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();

        layers.push(createLayerGroup(id, name));
        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Creates a new tiled wms layer.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {string} url - Url to the wms layer
     * @param {string} type - Server type
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution,
     *    {string} time - Time parameter for temporal layers,
     *    {integer} buffer - Buffer of additional border pixels that are used in the GetMap and GetFeatureInfo operations,
     *    {string} version - WMS version,
     *    {string} format - Layer format,
     *    {object} tileGrid - Grid pattern for accessing the tiles
     *    {object} sourceParams - Layer source parameters
     * @returns {ol.layer.Tile} tile - New tiled wms layer
     *
     * @private
     * @function createTileWMSLayer
     * @memberof MapDisplay
     * @inner
     */
    var createTileWMSLayer = function(layerId, layerName, layerTitle, url, type, layerVisible, disabled, params) {
      params = params !== undefined && params !== null ? params : {};

      var sourceParams = params.sourceParams !== undefined && params.sourceParams !== null ? params.sourceParams : {};

      sourceParams['LAYERS'] = layerId;
      sourceParams['TILED'] = true;

      if(params.time !== undefined && params.time !== null && params.time !== '')
        sourceParams['TIME'] = params.time;

      if(params.buffer !== undefined && params.buffer !== null && params.buffer !== '')
        sourceParams['BUFFER'] = params.buffer;

      if(params.version !== undefined && params.version !== null && params.version !== '')
        sourceParams['VERSION'] = params.version;

      if(params.format !== undefined && params.format !== null && params.format !== '')
        sourceParams['FORMAT'] = params.format;

      var layerSourceOptions = {
        url: url,
        serverType: type,
        params: sourceParams
      };

      if(params.tileGrid !== undefined && params.tileGrid !== null && params.tileGrid !== '')
        layerSourceOptions['tileGrid'] = new ol.tilegrid.TileGrid(params.tileGrid);

      var layerSource = new ol.source.TileWMS(layerSourceOptions);

      if(memberLayersStartLoadingFunction !== null && memberLayersEndLoadingFunction !== null) {
        layerSource.on('tileloadstart', function() { increaseLoading(layerId); });
        layerSource.on('tileloadend', function() { increaseLoaded(layerId); });
        layerSource.on('tileloaderror', function() { increaseLoaded(layerId); });
      }

      var tile = new ol.layer.Tile({
        source: layerSource,
        id: layerId,
        name: layerName,
        title: layerTitle,
        visible: layerVisible,
        preload: Infinity,
        disabled: disabled
      });

      if(params.minResolution !== undefined && params.minResolution !== null)
        tile.setMinResolution(params.minResolution);

      if(params.maxResolution !== undefined && params.maxResolution !== null)
        tile.setMaxResolution(params.maxResolution);

      return tile;
    }; 

    /**
     * Creates a new image wms layer.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {string} url - Url to the wms layer
     * @param {string} type - Server type
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution,
     *    {string} time - Time parameter for temporal layers,
     *    {integer} buffer - Buffer of additional border pixels that are used in the GetMap and GetFeatureInfo operations,
     *    {string} version - WMS version,
     *    {string} format - Layer format,
     *    {object} tileGrid - Grid pattern for accessing the tiles
     *    {object} sourceParams - Layer source parameters
     * @returns {ol.layer.Image} tile - New image wms layer
     *
     * @private
     * @function createImageWMSLayer
     * @memberof MapDisplay
     * @inner
     */

    var createImageWMSLayer = function(layerId, layerName, layerTitle, url, type, layerVisible, disabled, params) {
      params = params !== undefined && params !== null ? params : {};

      var sourceParams = params.sourceParams !== undefined && params.sourceParams !== null ? params.sourceParams : {};

      sourceParams['LAYERS'] = layerId;

      if(params.time !== undefined && params.time !== null && params.time !== '')
        sourceParams['TIME'] = params.time;

      if(params.buffer !== undefined && params.buffer !== null && params.buffer !== '')
        sourceParams['BUFFER'] = params.buffer;

      if(params.version !== undefined && params.version !== null && params.version !== '')
        sourceParams['VERSION'] = params.version;

      if(params.format !== undefined && params.format !== null && params.format !== '')
        sourceParams['FORMAT'] = params.format;

      var layerSourceOptions = {
        url: url,
        serverType: type,
        params: sourceParams
      };

      var layerSource = new ol.source.ImageWMS(layerSourceOptions);

      if(memberLayersStartLoadingFunction !== null && memberLayersEndLoadingFunction !== null) {
        layerSource.on('imageloadstart', function() { increaseLoading(layerId); });
        layerSource.on('imageloadend', function() { increaseLoaded(layerId); });
        layerSource.on('imageloaderror', function() { increaseLoaded(layerId); });
      }

      var image = new ol.layer.Image({
        source: layerSource,
        id: layerId,
        name: layerName,
        title: layerTitle,
        visible: layerVisible,
        preload: Infinity,
        disabled: disabled
      });

      if(params.minResolution !== undefined && params.minResolution !== null)
        tile.setMinResolution(params.minResolution);

      if(params.maxResolution !== undefined && params.maxResolution !== null)
        tile.setMaxResolution(params.maxResolution);

      return image;
    };

    /**
     * Creates a new WMTS source.
     * @param {string} url - Url to the wms layer
     * @param {string} layerId - Layer id
     * @param {string} time - Time parameter for temporal layers
     * @param {string} format - Layer format
     * @param {string} matrixSet - Matrix set
     * @param {object} tileGrid - Grid pattern for accessing the tiles
     * @returns {ol.source.WMTS} layerSource - New WMTS source
     *
     * @private
     * @function createWMTSSource
     * @memberof MapDisplay
     * @inner
     */
    var createWMTSSource = function(url, layerId, time, format, matrixSet, tileGrid) {
      url = (time !== undefined && time !== null && time !== '') ? url + "?TIME=" + time : url;

      var layerSourceOptions = {
        url: url,
        layer: layerId,
        format: format,
        matrixSet: matrixSet,
        tileGrid: new ol.tilegrid.WMTS(tileGrid)
      };

      var layerSource = new ol.source.WMTS(layerSourceOptions);

      if(memberLayersStartLoadingFunction !== null && memberLayersEndLoadingFunction !== null) {
        layerSource.on('tileloadstart', function() { increaseLoading(layerId); });
        layerSource.on('tileloadend', function() { increaseLoaded(layerId); });
        layerSource.on('tileloaderror', function() { increaseLoaded(layerId); });
      }

      return layerSource;
    };

    /**
     * Creates a new tiled wms layer.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {string} url - Url to the wms layer
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {string} time - Time parameter for temporal layers
     * @param {string} format - Layer format
     * @param {string} matrixSet - Matrix set
     * @param {object} tileGrid - Grid pattern for accessing the tiles
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution
     * @returns {ol.layer.Tile} tile - New WMTS layer
     *
     * @private
     * @function createWMTSLayer
     * @memberof MapDisplay
     * @inner
     */
    var createWMTSLayer = function(layerId, layerName, layerTitle, url, layerVisible, disabled, time, format, matrixSet, tileGrid, params) {
      var tile = new ol.layer.Tile({
        source: createWMTSSource(url, layerId, time, format, matrixSet, tileGrid),
        id: layerId,
        name: layerName,
        title: layerTitle,
        visible: layerVisible,
        preload: Infinity,
        disabled: disabled
      });

      if(params.minResolution !== undefined && params.minResolution !== null)
        tile.setMinResolution(params.minResolution);

      if(params.maxResolution !== undefined && params.maxResolution !== null)
        tile.setMaxResolution(params.maxResolution);

      return tile;
    };

    /**
     * Creates a new tiled wms layer with a Bing Maps source.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {string} imagerySet - Type of imagery, there are the following options: 'Road', 'Aerial', 'AerialWithLabels', 'collinsBart', 'ordnanceSurvey'
     * @param {string} bingMapsKey - Bing Maps require an api key to work, this api key can be generated at Bing Maps portal: http://www.bingmapsportal.com/
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution
     * @returns {ol.layer.Tile} tile - New WMS layer
     *
     * @private
     * @function createBingMapsLayer
     * @memberof MapDisplay
     * @inner
     */
    var createBingMapsLayer = function(layerId, layerName, layerTitle, layerVisible, disabled, imagerySet, bingMapsKey, params) {
      var layerSourceOptions = {
        key: bingMapsKey,
        imagerySet: imagerySet,
        maxZoom: 19,
        culture: 'pt-br'
      };

      var layerSource = new ol.source.BingMaps(layerSourceOptions);

      if(memberLayersStartLoadingFunction !== null && memberLayersEndLoadingFunction !== null) {
        layerSource.on('tileloadstart', function() { increaseLoading(layerId); });
        layerSource.on('tileloadend', function() { increaseLoaded(layerId); });
        layerSource.on('tileloaderror', function() { increaseLoaded(layerId); });
      }

      var tile = new ol.layer.Tile({
        source: layerSource,
        id: layerId,
        name: layerName,
        title: layerTitle,
        visible: layerVisible,
        disabled: disabled
      });

      if(params.minResolution !== undefined && params.minResolution !== null)
        tile.setMinResolution(params.minResolution);

      if(params.maxResolution !== undefined && params.maxResolution !== null)
        tile.setMaxResolution(params.maxResolution);

      return tile;
    };

    /**
     * Adds a new tiled wms layer to the map.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {string} url - Url to the wms layer
     * @param {string} type - Server type
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {string} parentGroup - Parent group id
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution,
     *    {string} time - Time parameter for temporal layers,
     *    {integer} buffer - Buffer of additional border pixels that are used in the GetMap and GetFeatureInfo operations,
     *    {string} version - WMS version,
     *    {string} format - Layer format,
     *    {object} tileGrid - Grid pattern for accessing the tiles
     *    {object} sourceParams - Layer source parameters
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addTileWMSLayer
     * @memberof MapDisplay
     * @inner
     */
    var addTileWMSLayer = function(layerId, layerName, layerTitle, url, type, layerVisible, disabled, parentGroup, params) {
      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();

        layers.push(
          createTileWMSLayer(layerId, layerName, layerTitle, url, type, layerVisible, disabled, params)
        );

        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Adds a new WMTS layer to the map.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {string} url - Url to the wms layer
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {string} time - Time parameter for temporal layers
     * @param {string} format - Layer format
     * @param {string} matrixSet - Matrix set
     * @param {object} tileGrid - Grid pattern for accessing the tiles
     * @param {string} parentGroup - Parent group id
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addWMTSLayer
     * @memberof MapDisplay
     * @inner
     */
    var addWMTSLayer = function(layerId, layerName, layerTitle, url, layerVisible, disabled, time, format, matrixSet, tileGrid, parentGroup, params) {
      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();

        layers.push(
          createWMTSLayer(layerId, layerName, layerTitle, url, layerVisible, disabled, time, format, matrixSet, tileGrid, params)
        );

        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Adds a new tiled wms layer with a Bing Maps source.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled in the layer explorer when created
     * @param {string} imagerySet - Type of imagery, there are the following options: 'Road', 'Aerial', 'AerialWithLabels', 'collinsBart', 'ordnanceSurvey'
     * @param {string} bingMapsKey - Bing Maps require an api key to work, this api key can be generated at Bing Maps portal: http://www.bingmapsportal.com/
     * @param {string} parentGroup - Parent group id
     * @param {boolean} appendAtTheEnd - Flag that indicates if the layer should be inserted as last in the layers order, if the parameter isn't provided, it's set to false
     * @param {object} params - Optional parameters, there are the following items:
     *    {float} minResolution - Layer minimum resolution,
     *    {float} maxResolution - Layer maximum resolution
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addBingMapsLayer
     * @memberof MapDisplay
     * @inner
     */
    var addBingMapsLayer = function(layerId, layerName, layerTitle, layerVisible, disabled, imagerySet, bingMapsKey, parentGroup, appendAtTheEnd, params) {
      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();

        var newLayer = createBingMapsLayer(layerId, layerName, layerTitle, layerVisible, disabled, imagerySet, bingMapsKey, params);

        if(appendAtTheEnd) layers.insertAt(0, newLayer);
        else layers.push(newLayer);

        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Creates a new GeoJSON vector layer.
     * @param {string} url - Url to the wms layer
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {float} minResolution - Layer minimum resolution
     * @param {float} maxResolution - Layer maximum resolution
     * @param {array} fillColors - Array with the fill colors
     * @param {array} strokeColors - Array with the stroke colors
     * @param {function} styleFunction - Function responsible for attributing the colors to the layer features
     * @returns {ol.layer.Vector} vector - New GeoJSON vector layer
     *
     * @private
     * @function createGeoJSONVectorLayer
     * @memberof MapDisplay
     * @inner
     */
    var createGeoJSONVectorLayer = function(url, layerId, layerName, layerTitle, layerVisible, minResolution, maxResolution, fillColors, strokeColors, styleFunction) {
      var vector = new ol.layer.Vector({
        source: new ol.source.Vector({
          url: url,
          format: new ol.format.GeoJSON(),
          strategy: ol.loadingstrategy.bbox
        }),
        style: function(feature) {
          var colors = styleFunction(feature, fillColors, strokeColors);
          return createStyle(colors.fillColor, colors.strokeColor);
        },
        id: layerId,
        name: layerName,
        title: layerTitle,
        visible: layerVisible
      });

      if(minResolution !== undefined && minResolution !== null)
        vector.setMinResolution(minResolution);

      if(maxResolution !== undefined && maxResolution !== null)
        vector.setMaxResolution(maxResolution);

      return vector;
    };

    /**
     * Adds a new GeoJSON vector layer to the map.
     * @param {string} url - Url to the wms layer
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {boolean} layerVisible - Flag that indicates if the layer should be visible on the map when created
     * @param {float} minResolution - Layer minimum resolution
     * @param {float} maxResolution - Layer maximum resolution
     * @param {array} fillColors - Array with the fill colors
     * @param {array} strokeColors - Array with the stroke colors
     * @param {function} styleFunction - Function responsible for attributing the colors to the layer features
     * @param {string} parentGroup - Parent group id
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addGeoJSONVectorLayer
     * @memberof MapDisplay
     * @inner
     */
    var addGeoJSONVectorLayer = function(url, layerId, layerName, layerTitle, layerVisible, minResolution, maxResolution, fillColors, strokeColors, styleFunction, parentGroup) {
      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();

        layers.push(
          createGeoJSONVectorLayer(url, layerId, layerName, layerTitle, layerVisible, minResolution, maxResolution, fillColors, strokeColors, styleFunction)
        );

        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Removes a layer with a given id from the Map.
     * @param {string} layerId - Layer id
     * @param {string|undefined} parentGroupId - Parent group id
     *
     * @function removeLayer
     * @memberof MapDisplay
     * @inner
     */
    var removeLayer = function(layerId, parentGroupId) {
      if(parentGroupId !== "undefined" && parentGroupId !== 'undefined' && parentGroupId !== undefined && parentGroupId !== null) {
        var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroupId);
        var layers = layerGroup.getLayers();

        layers.forEach(function(layer, i, array) {
          if(layerId === layer.get('id')) {
            layers.remove(layer);
            return false;
          }
        });

        layerGroup.setLayers(layers);
      } else {
        var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
        memberOlMap.removeLayer(layer);
      }
    };

    /**
     * Increases the loading tiles counter of a given layer. If the counter doesn't exist, it's created and the layer loading start function is executed.
     * @param {string} layerId - Layer id
     *
     * @private
     * @function increaseLoading
     * @memberof MapDisplay
     * @inner
     */
    var increaseLoading = function(layerId) {
      if(memberLoading[layerId] === undefined) memberLoading[layerId] = 0;

      if(memberLoading[layerId] === 0) {
        memberLayersStartLoadingFunction();
      }

      ++memberLoading[layerId];
    };

    /**
     * Increases the loaded tiles counter of a given layer. If the loaded or the loading counter doesn't exist, it's created, and if the counters are equal after the increase, the layer loading end function is executed.
     * @param {string} layerId - Layer id
     *
     * @private
     * @function increaseLoaded
     * @memberof MapDisplay
     * @inner
     */
    var increaseLoaded = function(layerId) {
      if(memberLoading[layerId] === undefined) memberLoading[layerId] = 0;
      if(memberLoaded[layerId] === undefined) memberLoaded[layerId] = 0;

      setTimeout(function() {
        ++memberLoaded[layerId];

        if(memberLoading[layerId] === memberLoaded[layerId]) {
          memberLoading[layerId] = 0;
          memberLoaded[layerId] = 0;

          memberLayersEndLoadingFunction();
        }
      }, 500);
    };

    /**
     * Adds an OSM layer to a given layer group.
     * @param {string} layerId - Layer id
     * @param {string} layerName - Layer name
     * @param {string} layerTitle - Layer title
     * @param {boolean} layerVisible - Layer visibility
     * @param {string} parentGroup - Parent layer group id
     * @param {boolean} appendAtTheEnd - Flag that indicates if the layer should be inserted as last in the layers order, if the parameter isn't provided, it's set to false
     * @returns {boolean} layerGroupExists - Indicates if the layer group exists
     *
     * @function addOSMLayer
     * @memberof MapDisplay
     * @inner
     */
    var addOSMLayer = function(layerId, layerName, layerTitle, layerVisible, parentGroup, appendAtTheEnd) {
      appendAtTheEnd = (appendAtTheEnd !== undefined && appendAtTheEnd !== null) ? appendAtTheEnd : false;

      var layerGroup = findBy(memberOlMap.getLayerGroup(), 'id', parentGroup);
      var layerGroupExists = layerGroup !== null;

      if(layerGroupExists) {
        var layers = layerGroup.getLayers();
        var source = new ol.source.OSM();

        if(memberLayersStartLoadingFunction !== null && memberLayersEndLoadingFunction !== null) {
          source.on('tileloadstart', function() { increaseLoading(layerId); });
          source.on('tileloadend', function() { increaseLoaded(layerId); });
          source.on('tileloaderror', function() { increaseLoaded(layerId); });
        }

        var newTile = new ol.layer.Tile({
          source: source,
          id: layerId,
          name: layerName,
          title: layerTitle,
          preload: Infinity,
          visible: layerVisible
        });

        if(appendAtTheEnd) layers.insertAt(0, newTile);
        else layers.push(newTile);

        layerGroup.setLayers(layers);
      }

      return layerGroupExists;
    };

    /**
     * Sets the layers start loading function.
     * @param {function} startLoadingFunction - Layers start loading function
     *
     * @function setLayersStartLoadingFunction
     * @memberof MapDisplay
     * @inner
     */
    var setLayersStartLoadingFunction = function(startLoadingFunction) {
      memberLayersStartLoadingFunction = startLoadingFunction;
    };

    /**
     * Sets the layers end loading function.
     * @param {function} endLoadingFunction - Layers end loading function
     *
     * @function setLayersEndLoadingFunction
     * @memberof MapDisplay
     * @inner
     */
    var setLayersEndLoadingFunction = function(endLoadingFunction) {
      memberLayersEndLoadingFunction = endLoadingFunction;
    };

    /**
     * Creates the capabilities layers in the map.
     * @param {xml} xml - Xml code of the server capabilities
     * @param {string} serverUrl - Server URL
     * @param {string} serverType - Server type
     * @param {string} serverId - Server id
     * @param {string} serverName - Server name
     *
     * @private
     * @function createCapabilitiesLayers
     * @memberof MapDisplay
     * @inner
     */
    var createCapabilitiesLayers = function(xml, serverUrl, serverType, serverId, serverName) {
      var capabilities = memberParser.read(xml);
      var layers = capabilities.Capability.Layer;

      var tilesWMSLayers = [];

      var layersLength = layers.Layer.length;
      for(var i = 0; i < layersLength; i++) {
        if(layers.Layer[i].hasOwnProperty('Layer')) {

          var subLayersLength = layers.Layer[i].Layer.length;
          for(var j = 0; j < subLayersLength; j++) {
            tilesWMSLayers.push(createTileWMSLayer(layers.Layer[i].Layer[j].Name, layers.Layer[i].Layer[j].Title, layers.Layer[i].Layer[j].Title, serverUrl, serverType, false, false));
          }
        } else {
          tilesWMSLayers.push(createTileWMSLayer(layers.Layer[i].Name, layers.Layer[i].Title, layers.Layer[i].Title, serverUrl, serverType, false, false));
        }
      }

      var layerGroup = new ol.layer.Group({
        layers: tilesWMSLayers,
        id: serverId,
        name: serverName
      });

      var parentLayerGroup = findBy(memberOlMap.getLayerGroup(), 'id', 'terrama2-layerexplorer');

      if(parentLayerGroup !== null) {
        var parentSubLayers = parentLayerGroup.getLayers();

        parentSubLayers.push(layerGroup);

        parentLayerGroup.setLayers(parentSubLayers);
      }
    };

    /**
     * Creates a new Openlayers Style object.
     * @param {string} fill - Layer fill color
     * @param {string} stroke - Layer stroke color
     * @returns {ol.style.Style} new ol.style.Style - New Openlayers Style object
     *
     * @private
     * @function createStyle
     * @memberof MapDisplay
     * @inner
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
     * @memberof MapDisplay
     * @inner
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

      $(document).trigger("layerVisibilityChange", [layer.get('id')]);
    };

    /**
     * Sets the visibility of a given layer or layer group by its id.
     * @param {string} layerId - Layer id
     * @param {boolean} visibilityFlag - Visibility flag, true to show and false to hide
     *
     * @function setLayerVisibilityById
     * @memberof MapDisplay
     * @inner
     */
    var setLayerVisibilityById = function(layerId, visibilityFlag) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      layer.setVisible(visibilityFlag);

      if(layer.getLayers) {
        var layers = layer.getLayers().getArray();
        var len = layers.length;
        for(var i = 0; i < len; i++) {
          layers[i].setVisible(visibilityFlag);
        }
      }

      $(document).trigger("layerVisibilityChange", [layerId]);
    };

    /**
     * Returns the flag that indicates if the given layer is visible.
     * @param {string} layerId - Layer id
     *
     * @function isLayerVisible
     * @memberof MapDisplay
     * @inner
     */
    var isLayerVisible = function(layerId) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      return layer.get('visible');
    };

    /**
     * Sets the layer visibility change event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setLayerVisibilityChangeEvent
     * @memberof MapDisplay
     * @inner
     */
    var setLayerVisibilityChangeEvent = function(eventFunction) {
      $(document).unbind("layerVisibilityChange");
      $(document).on("layerVisibilityChange", function(e, layerId) {
        eventFunction(layerId);
      });
    };

    /**
     * Adds the Zoom DragBox to the map.
     *
     * @function addZoomDragBox
     * @memberof MapDisplay
     * @inner
     */
    var addZoomDragBox = function() {
      memberOlMap.addInteraction(memberZoomDragBox);
    };

    /**
     * Removes the Zoom DragBox from the map.
     *
     * @function removeZoomDragBox
     * @memberof MapDisplay
     * @inner
     */
    var removeZoomDragBox = function() {
      memberOlMap.removeInteraction(memberZoomDragBox);
    };

    /**
     * Returns the current Zoom DragBox extent.
     * @returns {array} extent - Zoom DragBox extent
     *
     * @function getZoomDragBoxExtent
     * @memberof MapDisplay
     * @inner
     */
    var getZoomDragBoxExtent = function() {
      var extent = memberZoomDragBox.getGeometry().getExtent();
      return extent;
    };

    /**
     * Sets the Zoom DragBox start event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setZoomDragBoxStartEvent
     * @memberof MapDisplay
     * @inner
     */
    var setZoomDragBoxStartEvent = function(eventFunction) {
      memberZoomDragBox.on('boxstart', function(e) {
        eventFunction();
      });
    };

    /**
     * Sets the Zoom DragBox end event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setZoomDragBoxEndEvent
     * @memberof MapDisplay
     * @inner
     */
    var setZoomDragBoxEndEvent = function(eventFunction) {
      memberZoomDragBox.on('boxend', function(e) {
        eventFunction();
      });
    };

    /**
     * Returns the current map extent.
     * @returns {array} extent - Map extent
     *
     * @function getCurrentExtent
     * @memberof MapDisplay
     * @inner
     */
    var getCurrentExtent = function() {
      var extent = memberOlMap.getView().calculateExtent(memberOlMap.getSize());
      return extent;
    };

    /**
     * Zooms to the initial map extent.
     *
     * @function zoomToInitialExtent
     * @memberof MapDisplay
     * @inner
     */
    var zoomToInitialExtent = function() {
      memberOlMap.getView().fit(memberInitialExtent, memberOlMap.getSize(), { constrainResolution: false });
    };

    /**
     * Zooms to the received extent.
     * @param {array} extent - Extent
     *
     * @function zoomToExtent
     * @memberof MapDisplay
     * @inner
     */
    var zoomToExtent = function(extent) {
      memberOlMap.getView().fit(extent, memberOlMap.getSize(), { constrainResolution: false });
    };

    /**
     * Returns the current map resolution.
     * @returns {float} resolution - Map resolution
     *
     * @function getCurrentResolution
     * @memberof MapDisplay
     * @inner
     */
    var getCurrentResolution = function() {
      return memberOlMap.getView().getResolution();
    };

    /**
     * Verifies if the current resolution is valid for a given layer.
     * @param {string} layerId - Layer id
     * @returns {boolean} flag - Flag that indicates if the current resolution is valid for the layer
     *
     * @function isCurrentResolutionValidForLayer
     * @memberof MapDisplay
     * @inner
     */
    var isCurrentResolutionValidForLayer = function(layerId) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);
      var currentResolution = getCurrentResolution();

      return layer!== null && (layer.getMaxResolution() >= currentResolution && layer.getMinResolution() <= currentResolution);
    };

    /**
     * Sets the Map resolution change event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setMapResolutionChangeEvent
     * @memberof MapDisplay
     * @inner
     */
    var setMapResolutionChangeEvent = function(eventFunction) {
      if(memberResolutionChangeEventKey !== null) memberOlMap.getView().unByKey(memberResolutionChangeEventKey);
      memberResolutionChangeEventKey = memberOlMap.getView().on('propertychange', function(e) {
        switch(e.key) {
          case 'resolution':
            eventFunction();
            break;
        }
      });
    };

    /**
     * Sets the Map double click event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setMapDoubleClickEvent
     * @memberof MapDisplay
     * @inner
     */
    var setMapDoubleClickEvent = function(eventFunction) {
      if(memberDoubleClickEventKey !== null) memberOlMap.getView().unByKey(memberDoubleClickEventKey);
      memberDoubleClickEventKey = memberOlMap.on('dblclick', function(e) {
        eventFunction(correctLongitude(e.coordinate[0]), e.coordinate[1]);
      });
    };

    /**
     * Sets the Map single click event.
     * @param {function} eventFunction - Function to be executed when the event is triggered
     *
     * @function setMapSingleClickEvent
     * @memberof MapDisplay
     * @inner
     */
    var setMapSingleClickEvent = function(eventFunction) {
      if(memberSingleClickEventKey !== null) memberOlMap.getView().unByKey(memberSingleClickEventKey);
      memberSingleClickEventKey = memberOlMap.on('click', function(e) {
        eventFunction(correctLongitude(e.coordinate[0]), e.coordinate[1]);
      });
    };

    /**
     * Unsets the Map single click event.
     *
     * @function unsetMapSingleClickEvent
     * @memberof MapDisplay
     * @inner
     */
    var unsetMapSingleClickEvent = function() {
      if(memberSingleClickEventKey !== null) memberOlMap.getView().unByKey(memberSingleClickEventKey);
    };

    /**
     * Sets the Map single click event to call the GetFeatureInfoUrl function.
     * @param {string} layerId - Layer to be used in the GetFeatureInfoUrl function
     * @param {function} callback - Callback function
     *
     * @function setGetFeatureInfoUrlOnClick
     * @memberof MapDisplay
     * @inner
     */
    var setGetFeatureInfoUrlOnClick = function(layerId, callback) {
      unsetMapSingleClickEvent();
      setMapSingleClickEvent(function(longitude, latitude) {
        var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);

        if(layer !== null) {
          var source = layer.getSource();
          var coordinate = [longitude, latitude];
          var resolution = memberOlMap.getView().getResolution();
          var projection = 'EPSG:4326';
          var params = { 'INFO_FORMAT': 'application/json' };

          try {
            var url = source.getGetFeatureInfoUrl(coordinate, resolution, projection, params);
          } catch(e) {
            var url = null;
          }

          if(url) callback(url);
          else callback(null);
        } else callback(null);
      });
    };

    /**
     * Calls the function responsible for unset the Map single click event.
     *
     * @function unsetGetFeatureInfoUrlOnClick
     * @memberof MapDisplay
     * @inner
     */
    var unsetGetFeatureInfoUrlOnClick = function() {
      unsetMapSingleClickEvent();
    };

    /**
     * Sets a property of a given layer, with a given property key and value.
     * @param {string} layerId - Layer id
     * @param {string} property - Property key
     * @param {string} value - Property value
     *
     * @function setLayerProperty
     * @memberof MapDisplay
     * @inner
     */
    var setLayerProperty = function(layerId, property, value) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);

      try {
        layer.set(property, value);
      } catch(e) {
        console.error("TerraMA2WebComponents: Layer '" + layerId + "' not found!");
      }
    };

    /**
     * Returns the property of a given layer and property key.
     * @param {string} layerId - Layer id
     * @param {string} property - Property key
     * @returns {string} value - Property value
     *
     * @function getLayerProperty
     * @memberof MapDisplay
     * @inner
     */
    var getLayerProperty = function(layerId, property) {
      var layer = findBy(memberOlMap.getLayerGroup(), 'id', layerId);

      try {
        var value = layer.get(property);
      } catch(e) {
        var value = null;
      }

      return value;
    };

    /**
     * Finds a layer by a given key.
     * @param {ol.layer.Group} layer - The layer group where the method will run the search
     * @param {string} key - Layer attribute to be used in the search
     * @param {string} value - Value to be used in the search
     * @returns {ol.layer} layer - Layer found
     *
     * @function findBy
     * @memberof MapDisplay
     * @inner
     */
    var findBy = function(layer, key, value) {
      if(layer.get(key) === value)
        return layer;

      if(layer.getLayers) {
        var layers = layer.getLayers().getArray(),
        len = layers.length, result;
        for(var i = 0; i < len; i++) {
          result = findBy(layers[i], key, value);
          if(result)
            return result;
        }
      }

      return null;
    };

    /**
     * Applies a given CQL filter to a given layer.
     * @param {string} cql - CQL filter to be applied
     * @param {string} layerId - Layer id to be filtered
     *
     * @function applyCQLFilter
     * @memberof MapDisplay
     * @inner
     */
    var applyCQLFilter = function(cql, layerId) {
      findBy(memberOlMap.getLayerGroup(), 'id', layerId).getSource().updateParams({ "CQL_FILTER": (cql == "" ? null : cql) });
    };

    /**
     * Alters the index of a layer.
     * @param {string} parent - Parent id
     * @param {int} indexFrom - Current index of the layer
     * @param {int} indexTo - New index
     *
     * @function alterLayerIndex
     * @memberof MapDisplay
     * @inner
     */
    var alterLayerIndex = function(parent, indexFrom, indexTo) {
      var layers = findBy(memberOlMap.getLayerGroup(), 'id', parent).getLayers();
      var layer = layers.removeAt(indexFrom);
      layers.insertAt(indexTo, layer);
    };

    /**
     * Initializes the necessary features.
     *
     * @function init
     * @memberof MapDisplay
     * @inner
     */
    var init = function() {
      memberParser = new ol.format.WMSCapabilities();

      memberOlMap.getLayerGroup().set('id', 'terrama2-layerexplorer');
      memberOlMap.getLayerGroup().set('name', 'terrama2-layerexplorer');

      memberZoomDragBox = new ol.interaction.DragBox({
        condition: ol.events.condition.always
      });

      memberInitialExtent = memberOlMap.getView().calculateExtent(memberOlMap.getSize());

      $('.ol-attribution > button > span').html('©');

      $(document).ready(function() {
        updateMapSize();
      });
    };

    return {
      getMap: getMap,
      updateMapSize: updateMapSize,
      updateLayerSourceParams: updateLayerSourceParams,
      updateLayerAttribute: updateLayerAttribute,
      addMousePosition: addMousePosition,
      removeMousePosition: removeMousePosition,
      addScale: addScale,
      removeScale: removeScale,
      enableDoubleClickZoom: enableDoubleClickZoom,
      disableDoubleClickZoom: disableDoubleClickZoom,
      addLayerGroup: addLayerGroup,
      updateLayerTime: updateLayerTime,
      addImageWMSLayer: addImageWMSLayer,
      addWMTSLayer: addWMTSLayer,
      addBingMapsLayer: addBingMapsLayer,
      addGeoJSONVectorLayer: addGeoJSONVectorLayer,
      removeLayer: removeLayer,
      addOSMLayer: addOSMLayer,
      setLayersStartLoadingFunction: setLayersStartLoadingFunction,
      setLayersEndLoadingFunction: setLayersEndLoadingFunction,
      setLayerVisibility: setLayerVisibility,
      setLayerVisibilityById: setLayerVisibilityById,
      isLayerVisible: isLayerVisible,
      setLayerVisibilityChangeEvent: setLayerVisibilityChangeEvent,
      addZoomDragBox: addZoomDragBox,
      removeZoomDragBox: removeZoomDragBox,
      getZoomDragBoxExtent: getZoomDragBoxExtent,
      setZoomDragBoxStartEvent: setZoomDragBoxStartEvent,
      setZoomDragBoxEndEvent: setZoomDragBoxEndEvent,
      getCurrentExtent: getCurrentExtent,
      zoomToInitialExtent: zoomToInitialExtent,
      zoomToExtent: zoomToExtent,
      getCurrentResolution: getCurrentResolution,
      isCurrentResolutionValidForLayer: isCurrentResolutionValidForLayer,
      setMapResolutionChangeEvent: setMapResolutionChangeEvent,
      setMapDoubleClickEvent: setMapDoubleClickEvent,
      setMapSingleClickEvent: setMapSingleClickEvent,
      unsetMapSingleClickEvent: unsetMapSingleClickEvent,
      setGetFeatureInfoUrlOnClick: setGetFeatureInfoUrlOnClick,
      unsetGetFeatureInfoUrlOnClick: unsetGetFeatureInfoUrlOnClick,
      setLayerProperty: setLayerProperty,
      getLayerProperty: getLayerProperty,
      findBy: findBy,
      applyCQLFilter: applyCQLFilter,
      alterLayerIndex: alterLayerIndex,
      init: init,
      updateLayerOpacity: updateLayerOpacity,
      getLayerOpacity: getLayerOpacity
    };
  }
);
