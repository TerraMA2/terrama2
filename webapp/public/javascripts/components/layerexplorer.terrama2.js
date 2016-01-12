/** @class LayerExplorer - Component responsible for presenting an organized list of layers. */
var LayerExplorer = function(terrama2) {

  var selectedLayer = null;
  var parser = new ol.format.WMSCapabilities();
  var capabilities = null;
  var mapDisplay = terrama2.getMapDisplay();
  var map = mapDisplay.getMap();

  var socket = io(terrama2.getTerrama2Url());

  /**
   * Return the selected layer
   * @returns {string} selectedLayer - layer name
   */
  this.getSelectedLayer = function() {
    return selectedLayer;
  }

  /**
   * Process the layers from the capabilities and create the openlayers tiled wms layers
   * @param {json} layers - layers list from the capabilities
   * @returns {array} tilesWMSLayers - openlayers tiled wms layers array
   */
  var processLayers = function(layers) {
    var tilesWMSLayers = [];

    var layersLength = layers.Layer.length;

    for(var i = 0; i < layersLength; i++) {
      if(layers.Layer[i].hasOwnProperty('Layer')) {

        var subLayersLength = layers.Layer[i].Layer.length;
        for(var j = 0; j < subLayersLength; j++) {
          tilesWMSLayers.push(mapDisplay.createTileWMS(terrama2.getConfig().getConfJsonServer().URL, terrama2.getConfig().getConfJsonServer().Type, layers.Layer[i].Layer[j].Name, layers.Layer[i].Layer[j].Title));
        }
      } else {
        tilesWMSLayers.push(mapDisplay.createTileWMS(terrama2.getConfig().getConfJsonServer().URL, terrama2.getConfig().getConfJsonServer().Type, layers.Layer[i].Name, layers.Layer[i].Title));
      }
    }

    return tilesWMSLayers;
  }

  /**
   * Build a layer explorer from the map layers
   * @param {ol.layer} layer - layer or layers group to be used in the layer explorer
   * @param {boolean} firstCall - control flag
   * @returns {string} elem - string containing the HTML code to the layer explorer
   */
  var buildLayerExplorer = function(layer, firstCall) {
    var elem;
    var name = layer.get('name') ? layer.get('name') : "Group";
    var title = layer.get('title') ? layer.get('title') : "Group";

    var div2 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
    "<span class='terrama2-layerexplorer-plus'> " + title + "</span>&nbsp;&nbsp;" +
    terrama2.getConfig().getConfJsonHTML().OpacitySlider;

    /* var div2 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
    "<span class='terrama2-layerexplorer-plus'> " + title + "</span>&nbsp;&nbsp;" +
    terrama2.getConfig().getConfJsonHTML().CheckLayer +
    terrama2.getConfig().getConfJsonHTML().OpacitySlider; */

    if (layer.getLayers) {
      var sublayersElem = '';
      var layers = layer.getLayers().getArray(),
      len = layers.length;
      for (var i = len - 1; i >= 0; i--) {
        sublayersElem += buildLayerExplorer(layers[i]);
      }

      if(firstCall) {
        elem = sublayersElem + "</li>";
      } else {
        elem = div2 + " <ul>" + sublayersElem + "</ul></li>";
      }
    } else {

      var check = layer.get('visible') ? terrama2.getConfig().getConfJsonHTML().CheckLayerChecked : terrama2.getConfig().getConfJsonHTML().CheckLayerUnchecked;

      var div1 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
      check +
      "<span class='terrama2-layerexplorer-checkbox-span'>" + title + "</span>" +
      terrama2.getConfig().getConfJsonHTML().OpacitySlider;

      elem = div1 + " </li>";
    }
    return elem;
  }

  /**
   * Initialize the layer explorer and put it in the page
   * @param {xml} msg - capabilities xml code
   */
  var initializeLayerExplorer = function(msg) {
    capabilities = parser.read(msg);

    var processedLayers = processLayers(capabilities.Capability.Layer);

    map.addLayer(new ol.layer.Group({
      layers: processedLayers,
      name: 'server',
      title: terrama2.getConfig().getConfJsonServer().Name
    }));

    $("#terrama2-leftbar").find("[terrama2-box='terrama2-layerexplorer']").addClass('terrama2-leftbar-button-layers').attr('title', 'Camadas');

    var elem = buildLayerExplorer(map.getLayerGroup(), true);
    $('#terrama2-layerexplorer').append("<div class='terrama2-leftbox-content'><div class='terrama2-leftbox-header'><h2>Camadas</h2></div>" + elem + "</div>");

    $('#terrama2-layerexplorer li:has(ul)').addClass('parent_li');

    // Handle opacity slider control
    $('input.opacity').slider();

    $('.parent_li').find(' > ul > li').hide();
  }

  /**
   * Set the visibility of a given layer or layer group, if it is visible, it will be hidden, otherwise will be shown
   * @param {ol.layer} layer - openlayers layer or layer group
   */
  var setLayerVisibility = function(layer) {
    layer.setVisible(!layer.getVisible());

    if(layer.getLayers) {
      var layers = layer.getLayers().getArray();
      var len = layers.length;
      for (var i = 0; i < len; i++) {
        layers[i].setVisible(layer.getVisible());
      }
    }
  }

  /**
   * Load the LayerExplorer events
   */
  var loadEvents = function() {
    $('#terrama2-layerexplorer li.parent_li > span').on('click', function() {
      var children = $(this).parent('li.parent_li').find(' > ul > li');
      if (children.is(":visible")) {
        children.hide('fast');
        $(this).addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus');
      } else {
        children.show('fast');
        $(this).addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus');
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

      setLayerVisibility(layer);

      var children = _$this.parent('li.parent_li').find(' > ul > li');
      var span = _$this.parent('li.parent_li').find(' > span');
      if (children.is(":visible") || !layer.getVisible()) {
        children.hide('fast');
        span.addClass('terrama2-layerexplorer-plus').removeClass('terrama2-layerexplorer-minus');
      } else {
        children.show('fast');
        span.addClass('terrama2-layerexplorer-minus').removeClass('terrama2-layerexplorer-plus');
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

    $('div.terrama2-leftbox-content').on('click', function(e) {
      var _$this = $(this);

      setTimeout(function() {
        if(_$this.height() > _$this.parent().height()) {
          _$this.parent().find('.ui-resizable-e').css('height', _$this.css('height'));
        } else {
          _$this.parent().find('.ui-resizable-e').css('height', _$this.parent().css('height'));
        }
      }, 200);
    });
  }

  socket.emit(
    'proxyRequest',
    {
      url: terrama2.getConfig().getConfJsonServer().URL + terrama2.getConfig().getConfJsonServer().CapabilitiesParams,
      requestId: 'lala'
    }
  );
  socket.on('proxyResponse', function(msg) {
    initializeLayerExplorer(msg.msg);
    loadEvents();
  });

}
