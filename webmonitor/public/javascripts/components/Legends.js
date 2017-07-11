'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {

    var setLegends = function(visibleLayers, allLayers) {
			var html = "";

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');
				var layerName = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerName");
				var layerType = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerType");

				if(layerType !== "template" && layerType !== "custom") {
					var layerData = null;

					for(var j = 0, allLayersLength = allLayers.length; j < allLayersLength; j++) {
						if(layerId === allLayers[j].id) {
							layerData = allLayers[j];
							break;
						}
					}

					if(layerData !== null && layerData.id !== undefined && layerData.url !== undefined) {
						html += "<strong>" + layerName + "</strong><br/><img src='" + BASE_URL + "get-legend?layer=" + layerId + "&geoserverUri=" + layerData.url + "&random=" + Date.now().toString() + "'/>";
						if(visibleLayersLength > 1 && i < (visibleLayersLength - 1)) html += "<hr/>";
					}
				}
			}

			$('#legend-box > .legend-body').html((html !== "" ? html : "<strong>No data to show.</strong>"));
    };

    var loadEvents = function(){

			$('#legendsButton > button').on('click', function() {
				if($('#legend-box').hasClass('hidden'))
					$('#legend-box').removeClass('hidden');
				else
					$('#legend-box').addClass('hidden');
			});

      $("#legend-box").on("setLegends", function(event, visibleLayers, allLayers){
        setLegends(visibleLayers, allLayers);
      });
    }

    var init = function(){
			$("#legend-box").draggable({
				containment: $('#terrama2-map')
			});

      loadEvents();
    }

    return {
      init: init
    }

  }
);