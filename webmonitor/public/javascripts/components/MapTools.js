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
      $('.mouse-function-btn').removeClass('active');
      $('#terrama2-map').removeClass('cursor-move');
      $('#terrama2-map').removeClass('cursor-pointer');
      $('#getAttributes').removeClass('with-select');
      $('#getAttributes > select').addClass('hidden');
      if($('#feature-info-box').hasClass('ui-dialog-content'))
        $('#feature-info-box').dialog('close');
    };

    var loadEvents = function() {
      $('#moveMap').on('click', function() {
        resetMapMouseTools();
        activateMoveMapTool();
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