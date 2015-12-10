var ToolBox = function(terrama2) {

  $("#terrama2-map").append(terrama2.getConfig().getConfJsonHTML().TMA2Toolbox);
  $("#terrama2-map-toolbox").append(terrama2.getConfig().getConfJsonHTML().TMA2ToolboxControlButton);

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
}
