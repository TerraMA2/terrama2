'use strict';

define(['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents){

    var loadEvents = function(){
      
      $("#animated-map-box > .animated-map-header > .btn").on("click", function() {
        $("#animated-map-box").addClass("hidden");
        $("#animated-map").empty();
      });

    }

    var init = function(){
      loadEvents();
    }

    var createMap = function(layer){

      function threeHoursAgo() {
        return new Date(Math.round(Date.now() / 3600000) * 3600000 - 3600000 * 3);
      }
      var dates = layer.dateInfo.dates;
      var initialIndexDate = 0;
      var extent = ol.proj.transformExtent(layer.boundingBox, 'EPSG:4326', 'EPSG:3857');
      var frameRate = 0.5; // frames per second
      var animationId = null;
      var layers = [
        new ol.layer.Tile({
          source: new ol.source.TileWMS({
            url: "http://localhost:8080/geoserver/ows",
            serverType: "geoserver",
            params: {
              LAYERS: 'terrama2_8:view8',
              TILED: true
            }
          }),
          id: "terrama2_8:view8",
          name: "Ei",
          title: "fd",
          visible: true
        }),
        new ol.layer.Tile({
          source: new ol.source.TileWMS({
            url: layer.uriGeoServer + "/ows",
            serverType: layer.serverType,
            params: {
              LAYERS: layer.id,
              TILED: true
            }
          }),
          id: layer.id,
          name: layer.name,
          title: layer.name,
          visible: true
        })
      ];
      var map = new ol.Map({
        layers: layers,
        target: 'animated-map',
        view: new ol.View({
          center: ol.extent.getCenter(extent),
          zoom: 4
        })
      });

      function updateInfo() {
        var el = document.getElementById('info');
        el.innerHTML = dates[initialIndexDate];
      }

      function setTime() {
        initialIndexDate++;
        if (initialIndexDate >= dates.length){
          initialIndexDate = 0;
        }
        TerraMA2WebComponents.MapDisplay.updateLayerTime(layer.id, dates[initialIndexDate]);
        updateInfo();
      }
      setTime();

      var stop = function() {
        if (animationId !== null) {
          window.clearInterval(animationId);
          animationId = null;
        }
      };

      var play = function() {
        stop();
        animationId = window.setInterval(setTime, 1000 / frameRate);
      };

      var startButton = document.getElementById('play');
      startButton.addEventListener('click', play, false);

      var stopButton = document.getElementById('pause');
      stopButton.addEventListener('click', stop, false);

      updateInfo();

    }

    return {
      init: init,
      createMap: createMap
    }

  }
)