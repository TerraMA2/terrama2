'use strict';

define(
  ['components/Calendar', 'components/Layers', 'components/Utils', 'components/Sortable', 'TerraMA2WebComponents'],
  function(Calendar, Layers, Utils, Sortable, TerraMA2WebComponents) {

    var memberCapabilities;
    var memberSelectedLayers = [];

    var saveLayers = function() {
      var url = document.getElementById("wmsUri").value;
      var parser = document.createElement('a');
      parser.href = url;
      var geoUrl = parser.protocol + '//' + parser.host + parser.pathname;

      memberSelectedLayers.forEach(addInLayerExplorer);

      function addInLayerExplorer(element, index, array) {
        var allLayers = Layers.getAllLayers();
        for(var i in memberCapabilities) {
          if(memberCapabilities[i].name == element) {
            if(allLayers.map(function(l) {
                return l.id
              }).indexOf(memberCapabilities[i].name) > 0) {
              continue;
            }
            if(TerraMA2WebComponents.MapDisplay.addImageWMSLayer(memberCapabilities[i].name, memberCapabilities[i].title, memberCapabilities[i].title, geoUrl, "geoserver", false, false, "terrama2-layerexplorer", {
                version: "1.1.0"
              })) {
              TerraMA2WebComponents.LayerExplorer.addLayersFromMap(memberCapabilities[i].name, "custom", true, "treeview unsortable terrama2-truncate-text", null);
              allLayers.push({
                id: memberCapabilities[i].name,
                name: memberCapabilities[i].title,
                extent: memberCapabilities[i].extent,
                url: geoUrl
              });
              Sortable.addLayerToSort(memberCapabilities[i].name, memberCapabilities[i].title, memberCapabilities[i].parent);

              var span = "";
              var listElement = $("li[data-layerid='custom']");
              var li = $(listElement).find("li[data-layerid='" + memberCapabilities[i].name + "']");

              if(li.length === 0)
                continue;

              li.find('input').click();

              if(!memberCapabilities[i].extent)
                continue;

              var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':', '') + "'></div></div>";
              $(li).append(sliderDiv);

              if(memberCapabilities[i].extent instanceof Array) {
                span += "<span id='terrama2-slider' class='terrama2-datepicker-icon'> <i class='fa fa-sliders'></i></span>";
              } else if(memberCapabilities[i].extent instanceof Object) {
                span += "<span id='terrama2-calendar' class='terrama2-datepicker-icon'> <i class='fa fa-calendar'></i></span>";
              }
              $(li).append($(span));
            }
          }
        }

        if(!$("#custom").hasClass('open')) {
          $("#custom").addClass('open');
        }
        var groupSpanName = $('#custom').find(' span.group-name');
        groupSpanName.click();
      }

      $("#layersModal").modal('hide');
    };

    var fillModal = function(capabilities) {
      memberCapabilities = capabilities;
      var lis = "";
      var check = "<input type='checkbox' class='terrama2-layerviews-checkbox'/>";
      for(var i in memberCapabilities) {
        lis += '<li data-layerid="' + memberCapabilities[i].name + '">' + check + '<span>' + memberCapabilities[i].title + '</span>' + '</li>';
      }

      var htmlList = '<ul id="layersList">' + lis + '</ul>';
      $('#layersModalBody').append(htmlList);
    };

    var addLayers = function() {
      var url = document.getElementById("wmsUri").value;
      if(!url) return;

      var jsonData = {
        url: url,
        format: 'xml'
      }
      Utils.getSocket().emit('proxyRequest', jsonData);
    };

    var loadEvents = function() {
      $("#wmsUri").on('keyup', function(e) {
        if(e.keyCode == 13) {
          e.preventDefault();
          addLayers();
        }
      });

      document.getElementById("saveLayers").addEventListener("click", saveLayers);

      document.getElementById("addLayers").addEventListener("click", addLayers);

      $('#layersModalBody').on('click', 'input.terrama2-layerviews-checkbox', function() {
        var layerid = $(this).closest('li').data('layerid');
        var index = memberSelectedLayers.indexOf(layerid);

        if(index > -1)
          memberSelectedLayers.splice(index, 1);
        else
          memberSelectedLayers.push(layerid);
      });

      $('#layersModal').on('hidden.bs.modal', function(e) {
        memberSelectedLayers = [];
        $("#layersList").remove();
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      init: init,
      fillModal: fillModal
    };
  }
);