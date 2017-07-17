'use strict';

define(
  ['components/Utils', 'components/Slider', 'components/Layers', 'TerraMA2WebComponents'],
  function(Utils, Slider, Layers, TerraMA2WebComponents) {
    var loadSocketsListeners = function() {
      Utils.getWebAppSocket().on('generateFileResponse', function(result) {
        if(result.progress !== undefined && result.progress >= 100) {
          console.log('Quase lá! O arquivo está sendo preparado para o download<span>...</span>');
        } else if(result.progress !== undefined) {
          console.log(result.progress + '% Completo');
        } else {
          console.log("folder=" + result.folder + "&file=" + result.file);

          var exportLink = "http://localhost:36000/export?folder=" + result.folder + "&file=" + result.file;

          $('#exportation-iframe').attr('src', exportLink);
        }
      });
    };

    var loadEvents = function() {
      $('#export').on('click', function() {
				Utils.getWebAppSocket().emit('generateFileRequest', {
					format: $("#exportation-type").val().toString(),
					dateTimeFrom: '2017-07-10 00:00:00',
					dateTimeTo: '2017-07-12 23:59:59',
					schema: 'public',
					table: 'focos_saida',
					dateTimeField: 'data_pas',
					dataProviderId: 2
				});
			});

      $("#terrama2-sortlayers").on("click", ".terrama2-layer-tools", function() {
        var layer = Layers.getLayerById($(this).parent().data("layerid"));

        if(layer !== null) {
          $("#layer-toolbox > .layer-toolbox-body > #slider-box").empty().html("<label></label><br/><div id=\"opacity" + layer.htmlId + "\"></div>");
          //$("#layer-toolbox > .layer-toolbox-body > #exportation-box").empty().html();
          
          var currentOpacity = TerraMA2WebComponents.MapDisplay.getLayerOpacity(layer.id) * 100;
          Slider.setOpacitySlider(layer.id, currentOpacity);

          $("#layer-toolbox > .layer-toolbox-header .layer-name").text(layer.name);

          if($("#layer-toolbox").hasClass("hidden"))
            $("#layer-toolbox").removeClass("hidden");
        }
      });

      $("#layer-toolbox > .layer-toolbox-header > .btn").on("click", function() {
        $("#layer-toolbox").addClass("hidden");
      });
    };

    var init = function() {
      loadSocketsListeners();
      loadEvents();

      $("#layer-toolbox").draggable({
				containment: $('#terrama2-map')
			});
    };

    return {
      init: init
    };
  }
);