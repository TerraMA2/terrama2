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
        var layer = Layers.getLayerById($(this).data("layerid"));

        if(layer.exportation !== null) {
          Utils.getWebAppSocket().emit('generateFileRequest', {
            format: $("#exportation-type").val().toString(),
            dateTimeFrom: '2017-07-10 00:00:00',
            dateTimeTo: '2017-07-12 23:59:59',
            schema: layer.exportation.schema,
            table: layer.exportation.table,
            dateTimeField: layer.exportation.dateField,
            dataProviderId: layer.exportation.dataProviderId
          });
        }
			});

      $("#terrama2-sortlayers").on("click", ".terrama2-layer-tools", function() {
        var layer = Layers.getLayerById($(this).parent().data("layerid"));

        if(layer !== null) {
          $("#layer-toolbox > .layer-toolbox-body > #slider-box").empty().html("<label></label><br/><div id=\"opacity" + layer.htmlId + "\"></div>");
          $("#export").data("layerid", layer.id);
          
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