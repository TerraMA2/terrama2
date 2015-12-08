var LayerExplorer = function(terrama2) {

  var selectedLayer = null;

  var processLayers = function(arrLayers) {
    var tilesWMSLayers = [];

    var arrLayersLength = arrLayers.Layer.length;

    for(var i = 0; i < arrLayersLength; i++) {
      if(arrLayers.Layer[i].hasOwnProperty('Layer')) {

        var subLayersLength = arrLayers.Layer[i].Layer.length;
        for(var j = 0; j < subLayersLength; j++) {
          //alert(JSON.stringify(arrLayers.Layer[i].Layer));
          //tilesWMSLayers.push(createTileWMS(arrLayers[j].Name));

          tilesWMSLayers.push(mapDisplay.createTileWMS(arrLayers.Layer[i].Layer[j].Name, arrLayers.Layer[i].Layer[j].Title));
          map.addLayer(mapDisplay.createTileWMS(arrLayers.Layer[i].Layer[j].Name, arrLayers.Layer[i].Layer[j].Title));
        }
      } else {
        tilesWMSLayers.push(mapDisplay.createTileWMS(arrLayers.Layer[i].Name, arrLayers.Layer[i].Title));
        map.addLayer(mapDisplay.createTileWMS(arrLayers.Layer[i].Name, arrLayers.Layer[i].Title));
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
  var buildLayerTree = function(layer) {
    var elem;
    var name = layer.get('name') ? layer.get('name') : "Group";
    var title = layer.get('title') ? layer.get('title') : "Group";

    var div1 = terrama2.getConfig().getConfJsonHTML().TMA2LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().TMA2LiLayer2 +
    "<span>" + title + "</span>" +
    terrama2.getConfig().getConfJsonHTML().TMA2CheckLayer +
    terrama2.getConfig().getConfJsonHTML().TMA2OpacitySlider;

    var div2 = terrama2.getConfig().getConfJsonHTML().TMA2LiLayer1 + name + terrama2.getConfig().getConfJsonHTML().TMA2LiLayer2 +
    "<span><i class='glyphicon glyphicon-plus'></i> " + title + "</span>&nbsp;&nbsp;" +
    terrama2.getConfig().getConfJsonHTML().TMA2CheckLayer +
    terrama2.getConfig().getConfJsonHTML().TMA2OpacitySlider;

    if (layer.getLayers) {
      var sublayersElem = '';
      var layers = layer.getLayers().getArray(),
      len = layers.length;
      for (var i = len - 1; i >= 0; i--) {
        sublayersElem += buildLayerTree(layers[i]);
      }
      elem = div2 + " <ul>" + sublayersElem + "</ul></li>";
    } else {
      elem = div1 + " </li>";
    }
    return elem;
  }

  /**
  * Initialize the tree from the map layers
  * @returns {undefined}
  */
  var initializeTree = function() {

    var elem = buildLayerTree(map.getLayerGroup());
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

  /**right
  * @param {any} value
  * @returns {ol.layer.Base}
  */
  var findBy = function(layer, key, value) {

    if (layer.get(key) === value) {
      return layer;
    }

    // Find recursively if it is a group
    if (layer.getLayers) {
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

  var glbArrLayersGroups = [];
  var glbOlParser = new ol.format.WMSCapabilities();
  var jsnCapabilities = null;
  var mapDisplay = terrama2.getMapDisplay();
  var map = mapDisplay.getMap();

  $.ajax({
    url: 'http://localhost/bdqueimadas/proxy.php',
    dataType: 'xml',
    async: false,
    data: {
      url: "http://sigma.cptec.inpe.br/cgi-bin/mapserv",
      map : "/extra2/sigma/www/webservice/relatorio_queimadas.map",
      service : "WMS",
      request : "getCapabilities",
      version : "1.3.0"
    },
    success: function(data) {
      var xmlText = new XMLSerializer().serializeToString(data);
      jsnCapabilities = glbOlParser.read(xmlText);
      //console.log(JSON.stringify(jsnCapabilities));
    }
  });

  var arrProcessedLayers = processLayers(jsnCapabilities.Capability.Layer);

  //alert(jsnCapabilities.Capability.Layer.Layer);

  glbArrLayersGroups.push(new ol.layer.Group({
    layers: arrProcessedLayers,
    name: 'Server',
    title: 'Server'
  }));

  $(document).ready(function() {

    initializeTree();

    // Handle opacity slider control
    $('input.opacity').slider();
    $('input.opacity').on('slide', function(ev) {
      var layername = $(this).closest('li').data('layerid');
      var layer = findBy(map.getLayerGroup(), 'name', layername);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('i.terrama2-check').on('click', function(e) {
      var layername = $(this).closest('li').data('layerid');

      var layer = findBy(map.getLayerGroup(), 'name', layername);

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
      }, 0);
    });

    $('.parent_li').find(' > ul > li').hide();

  });

}
