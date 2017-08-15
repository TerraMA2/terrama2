'use strict';

define(['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents){

    var memberDatesRange = [];
    var memberInitialDate;
    var memberFinalDate;

    var loadEvents = function(){
      return;
    }

    var init = function(){
      loadEvents();
    }

    var setDatesSlider = function(layer){

      var valMap = layer.dateInfo.dates;
      memberInitialDate = 0;
      memberFinalDate = layer.dateInfo.dates.length - 1;

      var slider = $("#dates" + layer.id.replace(':', ''));
      var sliderParent = $(slider).parent();

      var initialLabelDate = $(sliderParent).find("#initialDate");
      var finalLabelDate = $(sliderParent).find("#finalDate");

      $(initialLabelDate).text(moment(valMap[memberInitialDate].replace('Z', '')).format('l LT'));
      $(finalLabelDate).text(moment(valMap[memberFinalDate].replace('Z', '')).format('l LT'));

      $(slider).slider({
        range: true,
        min: 0,
        max: valMap.length - 1,
        values: [0, valMap.length - 1],
        slide: function(event, ui) {
          $(initialLabelDate).text(moment(valMap[ui.values[0]].replace('Z', '')).format('l LT'));
          $(finalLabelDate).text(moment(valMap[ui.values[1]].replace('Z', '')).format('l LT'));
        },
        stop: function(event, ui) {
          memberInitialDate = ui.values[0];
          memberFinalDate = ui.values[1];
        }
      });

    }

    var createAnimation = function(layer){

      var dates = layer.dateInfo.dates;
      var initialIndexDate;
      var frameRate = 1; // frames per second
      var animationId = null;

      function updateInfo() {
        if (initialIndexDate != undefined){
          var el = $("#currentDate").find('label');
          $(el).text(moment(dates[initialIndexDate].replace('Z', '')).format("lll"));
        }
      }

      function setTime() {
        if (initialIndexDate >= memberFinalDate){
          initialIndexDate = memberInitialDate;
        }
        layer.dateInfo.initialDateIndex = initialIndexDate;
        Layers.updateDateInfo(layer.id, layer.dateInfo);
        TerraMA2WebComponents.MapDisplay.updateLayerTime(layer.id, dates[initialIndexDate]);
        updateInfo();
        initialIndexDate++;
      }

      var pause = function() {
        if (animationId !== null) {
          window.clearInterval(animationId);
          animationId = null;
        }

        if($("#playAnimation").hasClass("hidden"))
          $("#playAnimation").removeClass("hidden");

        if(!$("#pauseAnimation").hasClass("hidden"))
          $("#pauseAnimation").addClass("hidden");
      };

      var play = function() {

        pause();

        if (!initialIndexDate){
          initialIndexDate = memberInitialDate;
        }
        updateInfo();

        if($("#animate-tools").hasClass("hidden"))
          $("#animate-tools").removeClass("hidden");
      
        if($("#pauseAnimation").hasClass("hidden"))
          $("#pauseAnimation").removeClass("hidden");

        if(!$("#playAnimation").hasClass("hidden"))
          $("#playAnimation").addClass("hidden");

        animationId = window.setInterval(setTime, 1000 / frameRate);
      };

      var reload = function(){
        initialIndexDate = memberInitialDate;
      }

      var closeAnimateTools = function(){
        pause();

        if(!$("#animate-tools").hasClass("hidden"))
          $("#animate-tools").addClass("hidden");
      }

      var startButton = document.getElementById('play');
      startButton.addEventListener('click', play, false);

      var playToolButon = document.getElementById('playAnimation');
      playToolButon.addEventListener('click', play, false);

      var pauseButton = document.getElementById('pause');
      pauseButton.addEventListener('click', pause, false);

      var pauseToolButon = document.getElementById('pauseAnimation');
      pauseToolButon.addEventListener('click', pause, false);

      var reloadButon = document.getElementById('reload');
      reloadButon.addEventListener('click', reload, false);

      var reloadToolButon = document.getElementById('reloadAnimation');
      reloadToolButon.addEventListener('click', reload, false);

      var stopAnimationButton = document.getElementById('stopAnimation');
      stopAnimationButton.addEventListener('click', closeAnimateTools, false);

    }

    return {
      init: init,
      createAnimation: createAnimation,
      setDatesSlider: setDatesSlider
    }

  }
)