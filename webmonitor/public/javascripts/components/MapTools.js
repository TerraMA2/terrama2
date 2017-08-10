'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {
    var activateMoveMapTool = function() {
      $('#moveMap').addClass('active');
      $('#terrama2-map').addClass('cursor-move');
    };

    var resetMapMouseTools = function() {
      TerraMA2WebComponents.MapDisplay.unsetMapSingleClickEvent();
      TerraMA2WebComponents.MapDisplay.removeZoomDragBox();
      $('.mouse-function-btn').removeClass('active');
      $('#terrama2-map').removeClass('cursor-move');
      $('#terrama2-map').removeClass('cursor-pointer');
      $('#terrama2-map').removeClass('cursor-crosshair');
      $('#getAttributes').removeClass('with-select');
      $('#getAttributes > select').addClass('hidden');
      if($('#feature-info-box').hasClass('ui-dialog-content'))
        $('#feature-info-box').dialog('close');
    };

    var activateDragboxTool = function() {
      $('#dragbox').addClass('active');
      $('#terrama2-map').addClass('cursor-crosshair');
      TerraMA2WebComponents.MapDisplay.addZoomDragBox();
    };

    var loadEvents = function() {
      $('#moveMap').on('click', function() {
        resetMapMouseTools();
        activateMoveMapTool();
      });

      $('#dragbox').on('click', function() {
        resetMapMouseTools();
        activateDragboxTool();
      });

      TerraMA2WebComponents.MapDisplay.setZoomDragBoxEndEvent(function() {
        var dragBoxExtent = TerraMA2WebComponents.MapDisplay.getZoomDragBoxExtent();
        TerraMA2WebComponents.MapDisplay.zoomToExtent(dragBoxExtent);
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      init: init,
      resetMapMouseTools: resetMapMouseTools,
      activateMoveMapTool: activateMoveMapTool
    };
  }
);