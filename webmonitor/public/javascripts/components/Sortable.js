'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {
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

    var addLayerToSort = function(layerId, layerName) {
      var _layerId = layerId.replace(':', '');
      var spanIcon = "<span class='terrama2-layer-tools terrama2-datepicker-icon' data-toggle='tooltip' title='Layer Tools'> <i class='fa fa-gears'></i></span>";

      var liHtml = '<li id="' + _layerId + '" data-layerid="' + layerId + '" data-parentid="terrama2-layerexplorer" class="hide">' + layerName + spanIcon + '</li>';
      $('#terrama2-sortlayers').children('ul').prepend(liHtml);
    };

    return {
      setSortable: setSortable,
      addLayerToSort: addLayerToSort
    };
  }
);