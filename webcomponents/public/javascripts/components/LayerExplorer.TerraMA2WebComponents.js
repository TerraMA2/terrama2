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
   * Creates a layer group.
   * @param {string} name - Layer group name
   * @param {string} title - Layer group title
   * @param {string} layers - HTML code of the layers that will be inside of the layer group
   * @returns {string} html - HTML code of the layer group
   *
   * @private
   * @function createLayerGroup
   */
  var createLayerGroup = function(name, title, layers) {
    return "<li data-layerid='" + name + "' id='" + name.replace(':', '') + "' class='parent_li'><span class='group-title'><div class='terrama2-layerexplorer-plus'>+</div>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + title + "</span><ul class='children'>" + layers + "</ul></li>";
  };

  /**
   * Creates a layer.
   * @param {string} name - Layer name
   * @param {string} title - Layer title
   * @param {boolean} visible - Flag that indicates if the layer should be visible when created
   * @returns {string} html - HTML code of the layer
   *
   * @private
   * @function createLayer
   */
  var createLayer = function(name, title, visible) {
    var check = visible ? "<input type='checkbox' class='terrama2-layerexplorer-checkbox' checked/>" : "<input type='checkbox' class='terrama2-layerexplorer-checkbox'/>";

    return "<li data-layerid='" + name + "' id='" + name.replace(':', '') + "' class='layer'>" + check + "<span class='terrama2-layerexplorer-checkbox-span'>" + title + "</span></li>";
  };

  /**
   * Adds a layer group to the layer explorer.
   * @param {string} name - Layer group name
   * @param {string} title - Layer group title
   * @param {string} layers - HTML code of the layers that will be inside of the layer group
   * @param {string} parent - Parent element id
   *
   * @function addLayerGroup
   */
  var addLayerGroup = function(name, title, layers, parent) {
    if($('#' + parent.replace(':', '')).hasClass('parent_li')) {
      $('#' + parent.replace(':', '') + ' > ul.children').append(createLayerGroup(name, title, layers));

      if(!$('#' + parent.replace(':', '')).hasClass('open'))
        $('#' + parent.replace(':', '')).find(' > ul > li').hide();
    } else {
      $('#' + parent.replace(':', '')).append(createLayerGroup(name, title, layers));
    }
  };

  /**
   * Adds a layer to the layer explorer.
   * @param {string} name - Layer name
   * @param {string} title - Layer title
   * @param {boolean} visible - Flag that indicates if the layer should be visible when created
   * @param {string} parent - Parent element id
   *
   * @function addLayer
   */
  var addLayer = function(name, title, visible, parent) {
    if($('#' + parent.replace(':', '')).hasClass('parent_li')) {
      $('#' + parent.replace(':', '') + ' > ul.children').append(createLayer(name, title, visible));

      if(!$('#' + parent.replace(':', '')).hasClass('open'))
        $('#' + name.replace(':', '')).hide();
    } else {
      $('#' + parent.replace(':', '')).append(createLayer(name, title, visible));
    }
  };

  /**
   * Adds the layers of a given capabilities to the layer explorer and the map.
   * @param {string} capabilitiesUrl - Capabilities URL
   * @param {string} serverUrl - Server URL
   * @param {string} serverType - Server type
   * @param {string} serverName - Server name
   *
   * @function addCapabilitiesLayers
   */
  var addCapabilitiesLayers = function(capabilitiesUrl, serverUrl, serverType, serverName) {
    memberSocket.emit('proxyRequest', { url: capabilitiesUrl, additionalParameters: { serverUrl: serverUrl, serverType: serverType, serverName: serverName } });
  };

  /**
   * Creates the capabilities layers in the layer explorer and in the map.
   * @param {xml} xml - Xml code of the server capabilities
   * @param {string} serverUrl - Server URL
   * @param {string} serverType - Server type
   * @param {string} serverName - Server name
   *
   * @private
   * @function createCapabilitiesLayers
   */
  var createCapabilitiesLayers = function(xml, serverUrl, serverType, serverName) {
    memberCapabilities = memberParser.read(xml);

    var processedLayers = processCapabilitiesLayers(memberCapabilities.Capability.Layer, serverUrl, serverType);

    memberMap.addLayer(new ol.layer.Group({
      layers: processedLayers,
      name: 'server',
      title: serverName
    }));

    var elem = buildCapabilitiesLayers(memberMap.getLayerGroup(), true);
    $('#terrama2-layerexplorer').empty().append(elem);

    // Handle opacity slider control
    $('input.opacity').slider();

    $('.parent_li').find(' > ul > li').hide();
  };

  /**
   * Processes the layers from the capabilities and creates an array of Openlayers tiled WMS layers.
   * @param {json} layers - List of layers from the server capabilities
   * @param {string} serverUrl - Server URL
   * @param {string} serverType - Server type
   * @returns {array} tilesWMSLayers - Array of Openlayers tiled WMS layers
   *
   * @private
   * @function processCapabilitiesLayers
   */
  var processCapabilitiesLayers = function(layers, serverUrl, serverType) {
    var tilesWMSLayers = [];

    var layersLength = layers.Layer.length;
    for(var i = 0; i < layersLength; i++) {
      if(layers.Layer[i].hasOwnProperty('Layer')) {

        var subLayersLength = layers.Layer[i].Layer.length;
        for(var j = 0; j < subLayersLength; j++) {
          tilesWMSLayers.push(memberMapDisplay.createTileWMS(serverUrl, serverType, layers.Layer[i].Layer[j].Name, layers.Layer[i].Layer[j].Title, false));
        }
      } else {
        tilesWMSLayers.push(memberMapDisplay.createTileWMS(serverUrl, serverType, layers.Layer[i].Name, layers.Layer[i].Title, false));
      }
    }

    return tilesWMSLayers;
  };

  /**
   * Builds the capabilities layers.
   * @param {ol.layer} layer - Layer or layers group to be used in the layer explorer
   * @param {boolean} firstCall - Control flag that indicates if is the first call, being that this is a recursive function
   * @returns {string} elem - String containing the HTML code to the layers
   *
   * @private
   * @function buildCapabilitiesLayers
   */
  var buildCapabilitiesLayers = function(layer, firstCall) {
    var name = layer.get('name') ? layer.get('name') : "Group";
    var title = layer.get('title') ? layer.get('title') : "Group";

    if(layer.getLayers) {
      var sublayersElem = '',
          layers = layer.getLayers().getArray(),
          len = layers.length;

      for(var i = len - 1; i >= 0; i--)
        sublayersElem += buildCapabilitiesLayers(layers[i]);

      if(firstCall)
        var elem = sublayersElem;
      else
        var elem = createLayerGroup(name, title, sublayersElem);
    } else {
      var elem = createLayer(name, title, layer.get('visible'));
    }
    return elem;
  };

  /**
   * Loads the DOM events.
   *
   * @private
   * @function loadEvents
   */
  var loadEvents = function() {
    $('#terrama2-layerexplorer').on('click', 'span.group-title', function(ev) {
      var children = $(this).parent('li.parent_li').find(' > ul > li');
      if(children.is(":visible")) {
        children.hide('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus').html('+');
        $(this).parent('li.parent_li').removeClass('open');
      } else {
        children.show('fast');
        $(this).find('div').addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus').html('-');
        $(this).parent('li.parent_li').addClass('open');
      }
    });

    $('input.opacity').on('slide', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'name', layername);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('#terrama2-layerexplorer').on('click', 'input.terrama2-layerexplorer-checkbox', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'name', layername);

      if(layer !== null) {
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
      }

      ev.stopPropagation();
    });

    $('#terrama2-layerexplorer').on('click', 'li.layer', function() {
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
   * Loads the sockets listeners.
   *
   * @private
   * @function loadSocketsListeners
   */
  var loadSocketsListeners = function() {
    memberSocket.on('proxyResponse', function(response) {
      createCapabilitiesLayers(response.msg, response.additionalParameters.serverUrl, response.additionalParameters.serverType, response.additionalParameters.serverName);
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

    loadEvents();
    loadSocketsListeners();
  };

  return {
    getSelectedLayer: getSelectedLayer,
    addLayerGroup: addLayerGroup,
    addLayer: addLayer,
    addCapabilitiesLayers: addCapabilitiesLayers,
    init: init
  };
})();
