"use strict";

/**
 * Component responsible for presenting an organized list of layers.
 * @module LayerExplorer
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {string} memberSelectedLayer - Selected layer.
 * @property {object} memberParser - Capabilities parser.
 * @property {json} memberCapabilities - Server capabilities.
 * @property {object} memberMapDisplay - MapDisplay object.
 * @property {object} memberMap - Map object.
 * @property {object} memberSocket - Socket object.
 */
TerraMA2WebComponents.webcomponents.LayerExplorer = (function() {

  // Selected layer
  var memberSelectedLayer = null;
  // Capabilities parser
  var memberParser = null;
  // Server capabilities
  var memberCapabilities = null;
  // MapDisplay object
  var memberMapDisplay = null;
  // Map object
  var memberMap = null;
  // Socket object
  var memberSocket = null;

  /**
   * Returns the selected layer.
   * @returns {string} memberSelectedLayer - Layer name
   *
   * @function getSelectedLayer
   */
  var getSelectedLayer = function() {
    return memberSelectedLayer;
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
          tilesWMSLayers.push(memberMapDisplay.createTileWMS(TerraMA2WebComponents.Config.getConfJsonServer().URL, TerraMA2WebComponents.Config.getConfJsonServer().Type, layers.Layer[i].Layer[j].Name, layers.Layer[i].Layer[j].Title, false, true));
        }
      } else {
        tilesWMSLayers.push(memberMapDisplay.createTileWMS(TerraMA2WebComponents.Config.getConfJsonServer().URL, TerraMA2WebComponents.Config.getConfJsonServer().Type, layers.Layer[i].Name, layers.Layer[i].Title, false, true));
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
      for(var i = len - 1; i >= 0; i--) {
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
    memberCapabilities = memberParser.read(msg);

    var processedLayers = processLayers(memberCapabilities.Capability.Layer);

    memberMap.addLayer(new ol.layer.Group({
      layers: processedLayers,
      name: 'server',
      title: TerraMA2WebComponents.Config.getConfJsonServer().Name
    }));

    resetLayerExplorer(memberMap);
  };

  /**
   * Resets the layer explorer.
   * @param {ol.Map} map - Map object
   *
   * @function resetLayerExplorer
   */
  var resetLayerExplorer = function(map) {
    var elem = buildLayerExplorer(map.getLayerGroup(), true);
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
      if(children.is(":visible")) {
        children.hide('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus').html('+');
      } else {
        children.show('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus').html('-');
      }
    });

    $('input.opacity').on('slide', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'name', layername);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('.terrama2-layerexplorer-checkbox').on('click', function(e) {
      var layername = $(this).closest('li').data('layerid');

      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'name', layername);

      memberMapDisplay.setLayerVisibility(layer);

      var children = $(this).parent('li.parent_li').find(' > ul > li');
      var span = $(this).parent('li.parent_li').find(' > span');
      if(children.is(":visible") || !layer.getVisible()) {
        children.hide('fast');
        span.find('div').addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus').html('+');
      } else {
        children.show('fast');
        span.find('div').addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus').html('-');
      }
      e.stopPropagation();
    });

    $('li.parent_li ul li').on('click', function(e) {
      if($(this).hasClass("selected")) {
        $(this).removeClass("selected");
        memberSelectedLayer = null;
      } else {
        $('li.parent_li ul li').removeClass("selected");
        $(this).addClass("selected");
        memberSelectedLayer = $(this).attr("data-layerid");
      }
    });
  };

  /**
   * Initializes the necessary features.
   *
   * @function init
   */
  var init = function() {
    memberParser = new ol.format.WMSCapabilities();
    memberMapDisplay = TerraMA2WebComponents.webcomponents.MapDisplay;
    memberMap = memberMapDisplay.getMap();
    memberSocket = io(TerraMA2WebComponents.obj.getTerrama2Url());

    memberSocket.emit(
      'proxyRequest',
      {
        url: TerraMA2WebComponents.Config.getConfJsonServer().URL + TerraMA2WebComponents.Config.getConfJsonServer().CapabilitiesParams
      }
    );
    memberSocket.on('proxyResponse', function(msg) {
      initializeLayerExplorer(msg.msg);
    });
  };

  return {
    getSelectedLayer: getSelectedLayer,
    resetLayerExplorer: resetLayerExplorer,
    init: init
  };
})();
