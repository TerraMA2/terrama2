'use strict';

define(
  ['components/MapTools', 'components/Utils', 'TerraMA2WebComponents'],
  function(MapTools, Utils, TerraMA2WebComponents) {

    var featureInfo = function() {
			TerraMA2WebComponents.MapDisplay.setGetFeatureInfoUrlOnClick($('#getAttributes > select').val(), function(url) {
				if($('#feature-info-box').hasClass('ui-dialog-content'))
					$('#feature-info-box').dialog('close');

				if(url !== null) 
					Utils.getSocket().emit('proxyRequest', { url: url, requestId: 'GetFeatureInfoTool', format: 'json', params: { layerName: $('#getAttributes > select option:selected').text() } });
			});
    };

    var activateGetFeatureInfoTool = function() {
			$('#getAttributes').addClass('active');
			$('#terrama2-map').addClass('cursor-pointer');
			$('#getAttributes').addClass('with-select');
			$('#getAttributes > select').removeClass('hidden');
			//setGetFeatureInfoToolSelect();
      featureInfo();
    };

    var setGetFeatureInfoToolSelect = function(visibleLayers) {
			$('#getAttributes > select').empty();
			var showButton = false;

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');
				var layerName = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerName");
				var layerType = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerType");

				if(layerType !== "template" && layerType !== "custom") {
					$('#getAttributes > select').append($('<option></option>').attr('value', layerId).text(layerName));
					if(!showButton) showButton = true;
				}
			}

			if(!showButton) {
				if(!$('#getAttributes').hasClass('hidden'))
					$('#getAttributes').addClass('hidden');

				MapTools.resetMapMouseTools();
				MapTools.activateMoveMapTool();
			} else {
				$('#getAttributes').removeClass('hidden');
				if(!$('#getAttributes > select').hasClass('hidden'))
					featureInfo();
			}
    };

    var loadEvents = function(){

			$('#getAttributes > select').on('change', featureInfo);

			$('#getAttributes > button').on('click', function() {
				MapTools.resetMapMouseTools();
				activateGetFeatureInfoTool();
			});

      $("#terrama2-map").on("setGetFeatureInfoToolSelect", function(event, visibleLayers){
        setGetFeatureInfoToolSelect(visibleLayers);
      });
    }

    var init = function(){
      loadEvents();
    }

    return {
      init: init
    }
  }
);