'use strict';

define(['components/Layers', 'TerraMA2WebComponents'],
  function(Layers, TerraMA2WebComponents){

    // Initial date to start animation
    var memberInitialDate;
    // Final date to finish animation
    var memberFinalDate;
    // Layer member to animate
    var memberLayer;
    // Array with dates of layer
    var memberDates;
    // Current index of array date
    var memberCurrentIndexDate;

    // Function to create and set the slider to user choose de range of animation
    var setDatesSlider = function(layer){
      memberLayer = layer;
      memberDates = layer.dateInfo.dates;
      memberInitialDate = 0;
      memberFinalDate = memberDates.length - 1;
      memberCurrentIndexDate = 0;
      var slider = $("#dates" + memberLayer.id.replace(':', ''));
      var sliderParent = $(slider).parent();

      var initialLabelDate = $(sliderParent).find("#initialDate");
      var finalLabelDate = $(sliderParent).find("#finalDate");

      $(initialLabelDate).text(moment(memberDates[memberInitialDate].replace('Z', '')).format('L LT'));
      $(finalLabelDate).text(moment(memberDates[memberFinalDate].replace('Z', '')).format('L LT'));

      $(slider).slider({
        range: true,
        min: 0,
        max: memberDates.length - 1,
        values: [0, memberDates.length - 1],
        slide: function(event, ui) {
          $(initialLabelDate).text(moment(memberDates[ui.values[0]].replace('Z', '')).format('L LT'));
          $(finalLabelDate).text(moment(memberDates[ui.values[1]].replace('Z', '')).format('L LT'));
        },
        stop: function(event, ui) {
          memberInitialDate = ui.values[0];
          memberCurrentIndexDate = ui.values[0];
          memberFinalDate = ui.values[1];
        }
      });

    }

    // Update info of current date
    function updateInfo() {
      if (memberCurrentIndexDate != undefined){
        var el = $("#currentDate").find('label');
        $(el).text(moment(memberDates[memberCurrentIndexDate].replace('Z', '')).format("lll"));
      }
    }

    // Function to update layer time
    function setTime() {
      if (memberCurrentIndexDate > memberFinalDate){
        memberCurrentIndexDate = memberInitialDate;
      }
      memberLayer.dateInfo.initialDateIndex = memberCurrentIndexDate;
      Layers.updateDateInfo(memberLayer.id, memberLayer.dateInfo);
      TerraMA2WebComponents.MapDisplay.updateLayerTime(memberLayer.id, memberDates[memberCurrentIndexDate]);
      updateInfo();
      memberCurrentIndexDate++;
    }

    // Id used to get id of setInterval() and use to stop the animation in clearInterval()
    var animationId = null;

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

      if (!memberCurrentIndexDate){
        memberCurrentIndexDate = memberInitialDate;
      }
      updateInfo();

      if($("#animate-tools").hasClass("hidden"))
        $("#animate-tools").removeClass("hidden");
    
      if($("#pauseAnimation").hasClass("hidden"))
        $("#pauseAnimation").removeClass("hidden");

      if(!$("#playAnimation").hasClass("hidden"))
        $("#playAnimation").addClass("hidden");

      animationId = window.setInterval(setTime, 1000);
    };

    var reload = function(){
      memberCurrentIndexDate = memberInitialDate;
    }

    var closeAnimateTools = function(){
      pause();

      if(!$("#animate-tools").hasClass("hidden"))
        $("#animate-tools").addClass("hidden");
    }

    var loadEvents = function(){
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

    var init = function(){
      loadEvents();
    }

    return {
      init: init,
      setDatesSlider: setDatesSlider
    }
  }
)