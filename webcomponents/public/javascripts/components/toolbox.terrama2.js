"use strict";

/** @class ToolBox - Tool box located at the bottom of the map. */
TMA2WebComponents.components.ToolBox = (function() {

  var init = function() {
    $("#terrama2-map").append(TMA2WebComponents.Config.getConfJsonHTML().Toolbox);
    $("#terrama2-map-toolbox").append(TMA2WebComponents.Config.getConfJsonHTML().ToolboxControlButton);

    $(document).ready(function() {
      $("#terrama2-toolbox-control-button").on("click", function() {
        if($("#terrama2-map-toolbox").css("height") === "19px") {
          $("#terrama2-map-toolbox").animate({ height:'80px' }, { duration: 300, queue: false });
          $(this).find(' > i').attr("class", "glyphicon glyphicon-chevron-down");
        } else {
          $("#terrama2-map-toolbox").animate({ height:'19px' }, { duration: 300, queue: false });
          $(this).find(' > i').attr("class", "glyphicon glyphicon-chevron-up");
        }
      })
    });
  };

  return {
    init: init
  };
})();
