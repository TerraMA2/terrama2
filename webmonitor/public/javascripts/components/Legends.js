'use strict';

define(
  ['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents) {

    var setLegends = function(visibleLayers) {
      var allLayers = Layers.getAllLayers();
			var html = "";

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');

        var layerObject = Layers.getLayerById(layerId);
				var layerName = layerObject.name;
				var layerType = layerObject.parent;

				if(layerType !== "template" && layerType !== "custom") {
					var layerData = null;

					for(var j = 0, allLayersLength = allLayers.length; j < allLayersLength; j++) {
						if(layerId === allLayers[j].id) {
							layerData = allLayers[j];
							break;
						}
					}

					if(layerData !== null && layerData.id !== undefined && layerData.uriGeoServer !== undefined) {
						html += "<strong>" + layerName + "</strong><br/><img src='" + BASE_URL + "get-legend?layer=" + layerId + "&geoserverUri=" + layerData.uriGeoServer + "&random=" + Date.now().toString() + "'/>";
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

      $("#legend-box").on("setLegends", function(event, visibleLayers){
        setLegends(visibleLayers);
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