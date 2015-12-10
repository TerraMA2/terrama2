var LayerExplorer = function(terrama2) {

  var selectedLayer = null;

  var processLayers = function(arrLayers) {
    var tilesWMSLayers = [];

    var arrLayersLength = arrLayers.Layer.length;

    for(var i = 0; i < arrLayersLength; i++) {
      if(arrLayers.Layer[i].hasOwnProperty('Layer')) {

        var subLayersLength = arrLayers.Layer[i].Layer.length;
        for(var j = 0; j < subLayersLength; j++) {
          tilesWMSLayers.push(mapDisplay.createTileWMS(terrama2.getConfig().getConfJsonServer().URL, terrama2.getConfig().getConfJsonServer().Type, arrLayers.Layer[i].Layer[j].Name, arrLayers.Layer[i].Layer[j].Title));
        }
      } else {
        tilesWMSLayers.push(mapDisplay.createTileWMS(terrama2.getConfig().getConfJsonServer().URL, terrama2.getConfig().getConfJsonServer().Type, arrLayers.Layer[i].Name, arrLayers.Layer[i].Title));
      }
    }

    return tilesWMSLayers;
  }

  /**
  * Build a tree layer from the map layers with visible and opacity
  * options.
  *
  * @param {type} layer
  * @returns {String}
  */
  var buildLayerTree = function(layer, firstCall) {
    var elem;
    var name = layer.get('name') ? layer.get('name') : "Group";
    var title = layer.get('title') ? layer.get('title') : "Group";

    var div2 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
    "<span><i class='glyphicon glyphicon-plus'></i> " + title + "</span>&nbsp;&nbsp;" +
    terrama2.getConfig().getConfJsonHTML().OpacitySlider;

    /* var div2 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
    "<span><i class='glyphicon glyphicon-plus'></i> " + title + "</span>&nbsp;&nbsp;" +
    terrama2.getConfig().getConfJsonHTML().CheckLayer +
    terrama2.getConfig().getConfJsonHTML().OpacitySlider; */

    if (layer.getLayers) {
      var sublayersElem = '';
      var layers = layer.getLayers().getArray(),
      len = layers.length;
      for (var i = len - 1; i >= 0; i--) {
        sublayersElem += buildLayerTree(layers[i]);
      }

      if(firstCall) {
        elem = sublayersElem + "</li>";
      } else {
        elem = div2 + " <ul>" + sublayersElem + "</ul></li>";
      }
    } else {

      var check = layer.get('visible') ? terrama2.getConfig().getConfJsonHTML().CheckLayerChecked : terrama2.getConfig().getConfJsonHTML().CheckLayerUnchecked;

      var div1 = terrama2.getConfig().getConfJsonHTML().LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().LiLayer2 +
      "<span>" + title + "</span>" +
      check +
      terrama2.getConfig().getConfJsonHTML().OpacitySlider;

      elem = div1 + " </li>";
    }
    return elem;
  }

  /**
  * Initialize the tree from the map layers
  * @returns {undefined}
  */
  var initializeTree = function() {

    var elem = buildLayerTree(map.getLayerGroup(), true);
    $('#terrama2-layertree .terrama2-leftbox-content').empty().append("<div class='terrama2-leftbox-header'><h2>Camadas</h2><hr/></div>" + elem);

    $('#terrama2-layertree li:has(ul)').addClass('parent_li').find(' > span').attr('title', 'Collapse this branch');
    $('#terrama2-layertree li.parent_li > span').on('click', function(e) {
      var children = $(this).parent('li.parent_li').find(' > ul > li');
      if (children.is(":visible")) {
        children.hide('fast');
        $(this).attr('title', 'Expand this branch').find(' > i').addClass('glyphicon-plus').removeClass('glyphicon-minus');
      } else {
        children.show('fast');
        $(this).attr('title', 'Collapse this branch').find(' > i').addClass('glyphicon-minus').removeClass('glyphicon-plus');
      }
      //e.stopPropagation();
    });
  }

  /**
  * Set the visibility of a layer or a group of layers.
  * @param {ol.layer} layer
  * @returns {undefined}
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

  var getSelectedLayer = function() {
    return selectedLayer;
  }

  this.getSelectedLayer = getSelectedLayer;

  var parser = new ol.format.WMSCapabilities();
  var capabilities = null;
  var mapDisplay = terrama2.getMapDisplay();
  var map = mapDisplay.getMap();

  $.ajax({
    url: terrama2.getConfig().getConfJsonServer().ProxyURL,
    dataType: 'xml',
    async: false,
    data: {
      url: terrama2.getConfig().getConfJsonServer().URL,
      params: terrama2.getConfig().getConfJsonServer().CapabilitiesParams
    },
    success: function(data) {
      var xmlText = new XMLSerializer().serializeToString(data);
      capabilities = parser.read(xmlText);
    }
  });

  var processedLayers = processLayers(capabilities.Capability.Layer);

  map.addLayer(new ol.layer.Group({
    layers: processedLayers,
    name: 'server',
    title: terrama2.getConfig().getConfJsonServer().Name
  }));

  $(document).ready(function() {

    initializeTree();

    // Handle opacity slider control
    $('input.opacity').slider();
    $('input.opacity').on('slide', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = mapDisplay.findBy(map.getLayerGroup(), 'name', layername);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('i.terrama2-check').on('click', function(e) {
      var layername = $(this).closest('li').data('layerid');

      var layer = mapDisplay.findBy(map.getLayerGroup(), 'name', layername);

      setLayerVisibility(layer);

      if (layer.getVisible()) {
        $(this).removeClass('glyphicon-unchecked').addClass('glyphicon-check');
        $(this).parent('li.parent_li').find(' > ul > li > i.terrama2-check').attr('class', 'terrama2-check glyphicon glyphicon-check');
      } else {
        $(this).removeClass('glyphicon-check').addClass('glyphicon-unchecked');
        $(this).parent('li.parent_li').find(' > ul > li > i.terrama2-check').attr('class', 'terrama2-check glyphicon glyphicon-unchecked');
      }

      var children = $(this).parent('li.parent_li').find(' > ul > li');
      var span = $(this).parent('li.parent_li').find(' > span');
      if (children.is(":visible") || !layer.getVisible()) {
        children.hide('fast');
        span.find(' > i').addClass('glyphicon-plus').removeClass('glyphicon-minus');
      } else {
        children.show('fast');
        span.find(' > i').addClass('glyphicon-minus').removeClass('glyphicon-plus');
      }
      e.stopPropagation();
    });

    $('li.parent_li ul li').on('click', function(e) {
      if($(this).hasClass("selected")) {
        $(this).removeClass("selected");
        selectedLayer = null;
      } else {
        $('li.parent_li ul li').removeClass("selected");
        $(this).addClass("selected");
        selectedLayer = $(this).attr("data-layerid");
      }
    });

    $('div.terrama2-leftbox-content').on('click', function(e) {
      var _this = $(this);

      setTimeout(function() {
        if(_this.height() > _this.parent().height()) {
          _this.parent().find('.ui-resizable-e').css('height', _this.css('height'));
        } else {
          _this.parent().find('.ui-resizable-e').css('height', _this.parent().css('height'));
        }
      }, 200);
    });

    $('.parent_li').find(' > ul > li').hide();

  });

}
