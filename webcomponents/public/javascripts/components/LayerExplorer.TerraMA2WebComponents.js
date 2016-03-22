"use strict";

/**
 * Component responsible for presenting an organized list of layers.
 * @module LayerExplorer
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {string} memberSelectedLayer - Selected layer.
 * @property {object} memberMapDisplay - MapDisplay object.
 * @property {object} memberMap - Map object.
 */
TerraMA2WebComponents.webcomponents.LayerExplorer = (function() {

  // Selected layer
  var memberSelectedLayer = null;
  // MapDisplay object
  var memberMapDisplay = null;
  // Map object
  var memberMap = null;

  /**
   * Returns the selected layer.
   * @returns {string} memberSelectedLayer - Layer id
   *
   * @function getSelectedLayer
   */
  var getSelectedLayer = function() {
    return memberSelectedLayer;
  };

  /**
   * Creates a layer group.
   * @param {string} id - Layer group id
   * @param {string} name - Layer group name
   * @param {string} parent - Parent id
   * @param {string} layers - HTML code of the layers that will be inside of the layer group
   * @returns {string} html - HTML code of the layer group
   *
   * @private
   * @function createLayerGroup
   */
  var createLayerGroup = function(id, name, parent, layers) {
    return "<li data-layerid='" + id + "' data-parentid='" + parent + "' id='" + id.replace(':', '') + "' class='parent_li'><span class='group-name'><div class='terrama2-layerexplorer-plus'>+</div>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + name + "</span><ul class='children'>" + layers + "</ul></li>";
  };

  /**
   * Creates a layer.
   * @param {string} id - Layer id
   * @param {string} name - Layer name
   * @param {string} parent - Parent id
   * @param {boolean} visible - Flag that indicates if the layer should be visible when created
   * @returns {string} html - HTML code of the layer
   *
   * @private
   * @function createLayer
   */
  var createLayer = function(id, name, parent, visible) {
    var check = visible ? "<input type='checkbox' class='terrama2-layerexplorer-checkbox' checked/>" : "<input type='checkbox' class='terrama2-layerexplorer-checkbox'/>";

    return "<li data-layerid='" + id + "' data-parentid='" + parent + "' id='" + id.replace(':', '') + "' class='layer'>" + check + "<span class='terrama2-layerexplorer-checkbox-span'>" + name + "</span></li>";
  };

  // remove

  /**
   * Adds a layer group to the layer explorer.
   * @param {string} id - Layer group id
   * @param {string} name - Layer group name
   * @param {string} layers - HTML code of the layers that will be inside of the layer group
   * @param {string} parent - Parent element id
   *
   * @function addLayerGroup
   */
  var addLayerGroup = function(id, name, layers, parent) {
    if($('#' + parent.replace(':', '')).hasClass('parent_li')) {
      $('#' + parent.replace(':', '') + ' > ul.children').append(createLayerGroup(id, name, parent, layers));

      if(!$('#' + parent.replace(':', '')).hasClass('open'))
        $('#' + parent.replace(':', '')).find(' > ul > li').hide();
    } else {
      $('#' + parent.replace(':', '')).append(createLayerGroup(id, name, parent, layers));
    }

    setSortable();
  };

  /**
   * Adds a layer to the layer explorer.
   * @param {string} id - Layer id
   * @param {string} name - Layer name
   * @param {boolean} visible - Flag that indicates if the layer should be visible when created
   * @param {string} parent - Parent element id
   *
   * @function addLayer
   */
  var addLayer = function(id, name, visible, parent) {
    if($('#' + parent.replace(':', '')).hasClass('parent_li')) {
      $('#' + parent.replace(':', '') + ' > ul.children').append(createLayer(id, name, parent, visible));

      if(!$('#' + parent.replace(':', '')).hasClass('open'))
        $('#' + id.replace(':', '')).hide();
    } else {
      $('#' + parent.replace(':', '')).append(createLayer(id, name, parent, visible));
    }

    setSortable();
  };

  // remove

  /**
   * Adds a layer or a layer group to the layer explorer with data from the map.
   * @param {string} id - Layer or layer group id
   *
   * @function addLayersFromMap
   */
  var addLayersFromMap = function(id) {
    var data = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', id);

    if(data !== null) {
      var elem = buildLayersFromMap(data, 'root');
      $('#terrama2-layerexplorer').append(elem);

      // Handle opacity slider control
      $('input.opacity').slider();

      $('.parent_li').find(' > ul > li').hide();

      setSortable();
    }
  };

  /**
   * Builds a layer or a layer group with data from the map.
   * @param {ol.layer} layer - Layer or layers group to be used in the layer explorer
   * @param {string} parent - Parent id
   * @returns {string} elem - String containing the HTML code to the layers
   *
   * @private
   * @function buildLayersFromMap
   */
  var buildLayersFromMap = function(layer, parent) {
    if(layer.getLayers) {
      var sublayersElem = '',
          layers = layer.getLayers().getArray(),
          len = layers.length;

      for(var i = 0; i < len; i++)
        sublayersElem += buildLayersFromMap(layers[i], layer.get('id'));

      var elem = createLayerGroup(layer.get('id'), layer.get('name'), parent, sublayersElem);
    } else {
      var elem = createLayer(layer.get('id'), layer.get('name'), parent, layer.get('visible'));
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
    $('#terrama2-layerexplorer').on('click', 'span.group-name', function(ev) {
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
      var layerid = $(this).closest('li').data('layerid');
      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', layerid);

      layer.setOpacity(ev.value);
    });

    // Handle visibility control
    $('#terrama2-layerexplorer').on('click', 'input.terrama2-layerexplorer-checkbox', function(ev) {
      var layerid = $(this).closest('li').data('layerid');
      var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', layerid);

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
   * Sets the sortable elements.
   *
   * @private
   * @function setSortable
   */
  var setSortable = function() {
    $('.children').sortable({
      start: function(event, ui) {
        $(this).attr('data-previndex', ui.item.index());
      },
      update: function(event, ui) {
        TerraMA2WebComponents.webcomponents.MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), ui.item.index());
        $(this).removeAttr('data-previndex');
      }
    });

    $('.children').disableSelection();

    ///

    $('#terrama2-layerexplorer').sortable({
      start: function(event, ui) {
        $(this).attr('data-previndex', ui.item.index());
      },
      update: function(event, ui) {
        TerraMA2WebComponents.webcomponents.MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), ui.item.index());
        $(this).removeAttr('data-previndex');
      }
    });

    $('#terrama2-layerexplorer').disableSelection();
  };

  /**
   * Initializes the necessary features.
   *
   * @function init
   */
  var init = function() {
    memberMapDisplay = TerraMA2WebComponents.webcomponents.MapDisplay;
    memberMap = memberMapDisplay.getMap();
    loadEvents();
  };

  return {
    getSelectedLayer: getSelectedLayer,
    addLayerGroup: addLayerGroup,
    addLayer: addLayer,
    addLayersFromMap: addLayersFromMap,
    init: init
  };
})();
