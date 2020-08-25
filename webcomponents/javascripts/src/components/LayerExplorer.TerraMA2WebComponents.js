"use strict";

/**
 * Component responsible for presenting an organized list of layers.
 * @class LayerExplorer
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {string} memberSelectedLayer - Selected layer.
 * @property {object} memberMapDisplay - MapDisplay object.
 * @property {object} memberMap - Map object.
 */
define(
  ['TerraMA2WebComponentsPath/components/MapDisplay.TerraMA2WebComponents'],
  function(MapDisplay) {

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
     * @memberof LayerExplorer
     * @inner
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
     * @param {string} classes - Classes to be used in the li element
     * @param {string} style - Style to be used in the li element
     * @returns {string} html - HTML code of the layer group
     *
     * @private
     * @function createLayerGroup
     * @memberof LayerExplorer
     * @inner
     */
    var createLayerGroup = function(id, name, parent, layers, classes, style) {
      classes = classes !== '' ? ' ' + classes : classes;
      return `
        <li data-layerid="${id}" data-parentid="${parent}" id="${id.replace(':', '')}" class="parent_li${classes}" style="${style}">
          <span class='group-name'>
            <div class="sidebar-item-text">
              <div class='group-icon'></div>
              <span data-i18n="${name}">${name}</span>
            </div>
            <div class="sidebar-item-icon"></div>
          </span>
          <ul class='children sidebar-subitem-container'>
            ${layers}
          </ul>
        </li>`;
    };

    /**
     * Creates a layer.
     * @param {string} id - Layer id
     * @param {string} name - Layer name
     * @param {string} title - Layer title
     * @param {string} parent - Parent id
     * @param {boolean} visible - Flag that indicates if the layer should be visible when created
     * @param {boolean} disabled - Flag that indicates if the layer should be disabled when created
     * @param {string} classes - Classes to be used in the li element
     * @param {string} style - Style to be used in the li element
     * @returns {string} html - HTML code of the layer
     *
     * @private
     * @function createLayer
     * @memberof LayerExplorer
     * @inner
     */
    var createLayer = function(id, name, title, parent, visible, disabled, classes, style) {
      classes += disabled ? " layer disabled-content" : " layer"

      let showCog = parent=='template'?"style='display:none'":'';
      return `<li data-layerid="${id}" data-parentid="${parent}" data-container="body" data-toggle="popover" data-content="${title}" id="${id.replace(':', '')}" class="${classes}" style="${style}">
                <div class="sidebar-subitem-text">
                  <div class="checkbox">
                    <label>
                      <input type="checkbox" ${visible?"checked":""} />
                      ${name}
                    </label>
                  </div>
                </div>
                <div ${showCog} class="sidebar-subitem-icon">
                  <div class="dropdown dropdown-layer-tools">
                    <div class="fa fa-cog dropdown-toggle" id="dropdownMenu" data-toggle="dropdown"></div>
                    <ul class="dropdown-menu"></ul>
                  </div>
                </div>
              </li>
      `;
    };

    /**
     * Adds a layer or a layer group to the layer explorer with data from the map.
     * @param {string} id - Layer or layer group id
     * @param {string} parent - Parent id
     * @param {boolean} appendAtTheEnd - Flag that indicates if the element should be inserted as last element of the parent, if the parameter isn't provided, it's set to false
     * @param {string} classes - Classes to be used in the li element
     * @param {string} style - Style to be used in the li element
     *
     * @function addLayersFromMap
     * @memberof LayerExplorer
     * @inner
     */
    var addLayersFromMap = function(id, parent, appendAtTheEnd, classes, style, description = '') {
      appendAtTheEnd = (appendAtTheEnd !== null && appendAtTheEnd !== undefined) ? appendAtTheEnd : false;
      classes = (classes !== null && classes !== undefined) ? classes : '';
      style = (style !== null && style !== undefined) ? style : '';

      var data = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', id);

      if(data !== null) {
        data['classes'] = classes;
        data['style'] = style;

        var elem = buildLayersFromMap(data, parent, description);

        if(parent === 'terrama2-layerexplorer') {
          if(appendAtTheEnd) $('#' + parent).append(elem);
          else $('#' + parent).prepend(elem);
        } else {
          if(appendAtTheEnd) $('#' + parent + ' > ul.children').append(elem);
          else $('#' + parent + ' > ul.children').prepend(elem);
        }
        // Handle opacity slider control
        if(typeof $('input.opacity').slider === "function")
        $('input.opacity').slider();

        $('.parent_li').find(' > ul > li').hide();

        setSortable();
      }
    };

    /**
     * Removes a layer with a given id from the LayerExplorer and calls the removeLayer method of the MapDisplay.
     * @param {string} layerId - Layer id
     * @param {string|undefined} parentGroupId - Parent group id
     *
     * @function removeLayer
     * @memberof LayerExplorer
     * @inner
     */
    var removeLayer = function(layerId, parentGroupId) {
      $('#' + layerId.replace(':', '').split('.').join('\\.')).remove();
      memberMapDisplay.removeLayer(layerId, parentGroupId);
    };

    /**
     * Builds a layer or a layer group with data from the map.
     * @param {ol.layer} layer - Layer or layers group to be used in the layer explorer
     * @param {string} parent - Parent id
     * @returns {string} elem - String containing the HTML code to the layers
     *
     * @private
     * @function buildLayersFromMap
     * @memberof LayerExplorer
     * @inner
     */
    var buildLayersFromMap = function(layer, parent, description = '') {
      var elem = "";
      if(layer.getLayers) {
        var sublayersElem = '',
            layers = layer.getLayers().getArray(),
            len = layers.length;

        for(var i = 0; i < len; i++) {
          var sublayersElemTmp = sublayersElem;
          sublayersElem = buildLayersFromMap(layers[i], layer.get('id')) + sublayersElemTmp;
        }

        if(!$("#" + layer.get('id').replace(':', '')).length)
          elem = createLayerGroup(layer.get('id'), layer.get('name'), parent, sublayersElem, layer['classes'], layer['style']);
      } else {
        if(!$("#" + layer.get('id').replace(':', '')).length)
          elem = createLayer(layer.get('id'), layer.get('name'), description, parent, layer.get('visible'), layer.get('disabled'), layer['classes'], layer['style']);
      }

      return elem;
    };

    /**
     * Loads the DOM events.
     *
     * @private
     * @function loadEvents
     * @memberof LayerExplorer
     * @inner
     */
    var loadEvents = function() {
      $('#terrama2-layerexplorer').on('click', 'span.group-name', function(ev) {
        let children = $(this).parent('li.parent_li').find(' > ul > li');
        if(children.is(":visible")) {
          children.hide('fast');
          $(this).find('.sidebar-item-icon > .fa').addClass('fa-angle-down').removeClass('fa-angle-up');
          $(this).parent('li.parent_li').removeClass('open');
        } else {
          children.show('fast');
          $(this).find('.sidebar-item-icon > .fa').addClass('fa-angle-up').removeClass('fa-angle-down');
          $(this).parent('li.parent_li').addClass('open');
        }
      });

      $('input.opacity').on('slide', function(ev) {
        var layerid = $(this).closest('li').data('layerid');
        var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', layerid);

        layer.setOpacity(ev.value);
      });

      // Handle visibility control
      $('#terrama2-layerexplorer').on('click', '.checkbox input', function(ev) {
        var layerid = $(this).closest('li').data('layerid');
        var layer = memberMapDisplay.findBy(memberMap.getLayerGroup(), 'id', layerid);

        if(layer !== null) {
          memberMapDisplay.setLayerVisibility(layer);

          var children = $(this).parent('li.parent_li').find(' > ul > li');
          var div = $(this).parent('li.parent_li > .group-name > .sidebar-item-icon .fa');
          if(children.is(":visible") || !layer.getVisible()) {
            children.hide('fast');
            div.addClass('fa-angle-up').removeClass('fa-angle-down');
          } else {
            children.show('fast');
            div.addClass('fa-angle-down').removeClass('fa-angle-up');
          }
        }

        ev.stopPropagation();
      });

      /*$('#terrama2-layerexplorer').on('click', 'li.layer', function() {
        if($(this).hasClass("selected")) {
          $(this).removeClass("selected");
          memberSelectedLayer = null;
        } else {
          $('li.parent_li ul li').removeClass("selected");
          $(this).addClass("selected");
          memberSelectedLayer = $(this).attr("data-layerid");
        }
      });*/
    };

    /**
     * Sets the sortable elements.
     *
     * @private
     * @function setSortable
     * @memberof LayerExplorer
     * @inner
     */
    var setSortable = function() {
      if(typeof $('.children').sortable === "function") {
        $('.children').sortable({
          items: "li:not(.unsortable)",
          start: function(event, ui) {
            $(this).attr('data-previndex', (ui.item.context.parentNode.childElementCount - 2) - ui.item.index());
          },
          update: function(event, ui) {
            MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), (ui.item.context.parentNode.childElementCount - 1) - ui.item.index());
            $(this).removeAttr('data-previndex');
          }
        });

        $('.children').disableSelection();

        $('#terrama2-layerexplorer').sortable({
          items: "li:not(.unsortable)",
          start: function(event, ui) {
            $(this).attr('data-previndex', (ui.item.context.parentNode.childElementCount - 2) - ui.item.index());
          },
          update: function(event, ui) {
            MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), (ui.item.context.parentNode.childElementCount - 1) - ui.item.index());
            $(this).removeAttr('data-previndex');
          }
        });

        $('#terrama2-layerexplorer').disableSelection();
      }
    };

    /**
     * Initializes the necessary features.
     *
     * @function init
     * @memberof LayerExplorer
     * @inner
     */
    var init = function() {
      memberMapDisplay = MapDisplay;
      memberMap = memberMapDisplay.getMap();
      loadEvents();
    };

    return {
      getSelectedLayer: getSelectedLayer,
      addLayersFromMap: addLayersFromMap,
      removeLayer: removeLayer,
      init: init
    };
  }
);
