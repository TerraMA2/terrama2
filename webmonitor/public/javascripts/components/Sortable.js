'use strict';

define(
  ['components/Utils', 'TerraMA2WebComponents'],
  function(Utils, TerraMA2WebComponents) {
    var setSortable = function() {
      $('#terrama2-sortlayers').sortable({
        items: "li",
        start: function(event, ui) {
          $(this).attr('data-previndex', (ui.item.context.parentNode.childElementCount - 2) - ui.item.index());
        },
        update: function(event, ui) {
          TerraMA2WebComponents.MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), (ui.item.context.parentNode.childElementCount - 1) - ui.item.index());
          $(this).removeAttr('data-previndex');
        }
      });
    };

    var addLayerToSort = function(layerId, layerName, parent) {
      var _layerId = layerId.replace(':', '');
      var spanIcon = "<span class='terrama2-layer-tools terrama2-datepicker-icon' data-i18n=\"[title]Layer Tools\">" + (parent != 'custom' && parent != 'template' ? " <i class='glyphicon glyphicon-resize-full'></i>" : "") + " <i class='fa fa-gear'></i></span>";

      var liHtml = '<li id="' + _layerId + '" data-layerid="' + layerId + '" data-parentid="terrama2-layerexplorer" class="hide" title="' + layerName + '"><span class="layer-name">' + layerName + '</span>' + spanIcon + '</li>';
      $('#terrama2-sortlayers').children('ul').prepend(liHtml);
      Utils.translate('#terrama2-sortlayers');
    };

    return {
      setSortable: setSortable,
      addLayerToSort: addLayerToSort
    };
  }
);