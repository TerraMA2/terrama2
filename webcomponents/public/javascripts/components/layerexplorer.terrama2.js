"use strict";

/**
 * Component responsible for presenting an organized list of layers.
 * @module LayerExplorer
 *
 * @property {string} selectedLayer - Selected layer.
 * @property {object} parser - Capabilities parser.
 * @property {json} capabilities - Server capabilities.
 * @property {object} mapDisplay - MapDisplay object.
 * @property {object} map - Map object.
 * @property {object} socket - Socket object.
 */
TerraMA2WebComponents.webcomponents.LayerExplorer = (function() {

  // Selected layer
  var selectedLayer = null;
  // Capabilities parser
  var parser = null;
  // Server capabilities
  var capabilities = null;
  // MapDisplay object
  var mapDisplay = null;
  // Map object
  var map = null;
  // Socket object
  var socket = null;

  /**
   * Returns the selected layer.
   * @returns {string} selectedLayer - Layer name
   *
   * @function getSelectedLayer
   */
  var getSelectedLayer = function() {
    return selectedLayer;
  };

  /**
   * Processes the layers from the capabilities and creates an array of Openlayers tiled WMS layers.
   * @param {json} layers - List of layers from the server capabilities
   * @returns {array} tilesWMSLayers - Array of Openlayers tiled WMS layers
   *
   * @private
   * @function processLayers
   */
  var processLayers = function(layers) {
    var tilesWMSLayers = [];

    var layersLength = layers.Layer.length;

    for(var i = 0; i < layersLength; i++) {
      if(layers.Layer[i].hasOwnProperty('Layer')) {

        var subLayersLength = layers.Layer[i].Layer.length;
        for(var j = 0; j < subLayersLength; j++) {
          tilesWMSLayers.push(mapDisplay.createTileWMS(TerraMA2WebComponents.Config.getConfJsonServer().URL, TerraMA2WebComponents.Config.getConfJsonServer().Type, layers.Layer[i].Layer[j].Name, layers.Layer[i].Layer[j].Title, false, true));
        }
      } else {
        tilesWMSLayers.push(mapDisplay.createTileWMS(TerraMA2WebComponents.Config.getConfJsonServer().URL, TerraMA2WebComponents.Config.getConfJsonServer().Type, layers.Layer[i].Name, layers.Layer[i].Title, false, true));
      }
    }

    return tilesWMSLayers;
  };

  /**
   * Builds a layer explorer from the map layers.
   * @param {ol.layer} layer - Layer or layers group to be used in the layer explorer
   * @param {boolean} firstCall - Control flag that indicates if is the first call, being that this is a recursive function
   * @returns {string} elem - String containing the HTML code to the layer explorer
   *
   * @private
   * @function buildLayerExplorer
   */
  var buildLayerExplorer = function(layer, firstCall) {
    var elem;
    var name = layer.get('name') ? layer.get('name') : "Group";
    var title = layer.get('title') ? layer.get('title') : "Group";

    if(layer.getLayers) {
      var sublayersElem = '';
      var layers = layer.getLayers().getArray(),
      len = layers.length;
      for (var i = len - 1; i >= 0; i--) {
        sublayersElem += buildLayerExplorer(layers[i]);
      }

      if(firstCall) {
        elem = sublayersElem + "</li>";
      } else {
        elem = "<li data-layerid='" + name + "'><span><div class='terrama2-layerexplorer-plus'>+</div>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + title + "</span> <ul>" + sublayersElem + "</ul></li>";
      }
    } else {
      if(layer.get('listOnLayerExplorer')) {
        var check = layer.get('visible') ? "<input type='checkbox' class='terrama2-layerexplorer-checkbox' checked/>" : "<input type='checkbox' class='terrama2-layerexplorer-checkbox'/>";
        elem = "<li data-layerid='" + name + "'>" + check + "<span class='terrama2-layerexplorer-checkbox-span'>" + title + "</span> </li>";
      } else elem = "";
    }
    return elem;
  };

  /**
   * Initializes the layer explorer and puts it in the page.
   * @param {xml} msg - Xml code of the server capabilities
   *
   * @private
   * @function initializeLayerExplorer
   */
  var initializeLayerExplorer = function(msg) {
    capabilities = parser.read(msg);

    var processedLayers = processLayers(capabilities.Capability.Layer);

    map.addLayer(new ol.layer.Group({
      layers: processedLayers,
      name: 'server',
      title: TerraMA2WebComponents.Config.getConfJsonServer().Name
    }));

    resetLayerExplorer(map);
  };

  /**
   * Resets the layer explorer.
   * @param {ol.Map} mapObj - Map object
   *
   * @function resetLayerExplorer
   */
  var resetLayerExplorer = function(mapObj) {
    var elem = buildLayerExplorer(mapObj.getLayerGroup(), true);
    $('#terrama2-layerexplorer').empty().append(elem);

    $('#terrama2-layerexplorer li:has(ul)').addClass('parent_li');

    // Handle opacity slider control
    $('input.opacity').slider();

    $('.parent_li').find(' > ul > li').hide();

    loadEvents();
  };

  /**
   * Loads the DOM events.
   *
   * @private
   * @function loadEvents
   */
  var loadEvents = function() {
    $('#terrama2-layerexplorer li.parent_li > span').on('click', function() {
      var children = $(this).parent('li.parent_li').find(' > ul > li');
      if (children.is(":visible")) {
        children.hide('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus').html('+');
      } else {
        children.show('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus').html('-');
      }
    });

    $('input.opacity').on('slide', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = mapDisplay.findBy(map.getLayerGroup(), 'name', layername);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('.terrama2-layerexplorer-checkbox').on('click', function(e) {
      var _$this = $(this);

      var layername = _$this.closest('li').data('layerid');

      var layer = mapDisplay.findBy(map.getLayerGroup(), 'name', layername);

      mapDisplay.setLayerVisibility(layer);

      var children = _$this.parent('li.parent_li').find(' > ul > li');
      var span = _$this.parent('li.parent_li').find(' > span');
      if (children.is(":visible") || !layer.getVisible()) {
        children.hide('fast');
        span.find('div').addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus').html('+');
      } else {
        children.show('fast');
        span.find('div').addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus').html('-');
      }
      e.stopPropagation();
    });

    $('li.parent_li ul li').on('click', function(e) {
      var _$this = $(this);

      if(_$this.hasClass("selected")) {
        _$this.removeClass("selected");
        selectedLayer = null;
      } else {
        $('li.parent_li ul li').removeClass("selected");
        _$this.addClass("selected");
        selectedLayer = _$this.attr("data-layerid");
      }
    });
  };

  /**
   * Initializes the necessary features.
   *
   * @function init
   */
  var init = function() {
    parser = new ol.format.WMSCapabilities();
    mapDisplay = TerraMA2WebComponents.webcomponents.MapDisplay;
    map = mapDisplay.getMap();
    socket = io(TerraMA2WebComponents.obj.getTerrama2Url());

    socket.emit(
      'proxyRequest',
      {
        url: TerraMA2WebComponents.Config.getConfJsonServer().URL + TerraMA2WebComponents.Config.getConfJsonServer().CapabilitiesParams,
        requestId: 'lala'
      }
    );
    socket.on('proxyResponse', function(msg) {
      initializeLayerExplorer(msg.msg);
    });
  };

  return {
    getSelectedLayer: getSelectedLayer,
    resetLayerExplorer: resetLayerExplorer,
    init: init
  };
})();
