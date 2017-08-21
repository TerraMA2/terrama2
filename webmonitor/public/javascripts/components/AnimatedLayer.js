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
    var memberDatesList;
    // Object with dates of layer
    var memberDatesObject;
    // Type of dates - continuous or list
    var memberDateType;
    // Current date of animation - index of type if list, date string if continuous
    var memberCurrentDate;
    // Period date to animation
    var memberPeriodDate = 1;
    // Id used to get id of setInterval() and use to stop the animation in clearInterval()
    var animationId = null;

    // Set layer info to animate
    var setLayerToAnimate = function(layer){
      pause();
      memberLayer = layer;
      if (Array.isArray(memberLayer.dateInfo.dates))
        memberDateType = 'list';
      else
        memberDateType = 'continuous';
    }

    // Set info when type of dates is continuous
    var setDatesCalendar = function(){
      memberDatesObject = memberLayer.dateInfo.dates;
      memberInitialDate = memberDatesObject.startDate;
      memberFinalDate = memberDatesObject.endDate;
      memberCurrentDate = memberInitialDate;
    }

    // Function to create and set the slider to user choose de range of animation
    var setDatesSlider = function(){
      memberDatesList = memberLayer.dateInfo.dates;
      memberInitialDate = 0;
      memberFinalDate = memberDatesList.length - 1;
      memberCurrentDate = 0;
      var slider = $("#dates" + memberLayer.id.replace(':', ''));
      var sliderParent = $(slider).parent();

      var initialLabelDate = $(sliderParent).find("#initialDate");
      var finalLabelDate = $(sliderParent).find("#finalDate");

      $(initialLabelDate).text(moment(memberDatesList[memberInitialDate].replace('Z', '')).format('L LT'));
      $(finalLabelDate).text(moment(memberDatesList[memberFinalDate].replace('Z', '')).format('L LT'));

      $(slider).slider({
        range: true,
        min: 0,
        max: memberDatesList.length - 1,
        values: [0, memberDatesList.length - 1],
        slide: function(event, ui) {
          $(initialLabelDate).text(moment(memberDatesList[ui.values[0]].replace('Z', '')).format('L LT'));
          $(finalLabelDate).text(moment(memberDatesList[ui.values[1]].replace('Z', '')).format('L LT'));
        },
        stop: function(event, ui) {
          memberInitialDate = ui.values[0];
          memberCurrentDate = ui.values[0];
          memberFinalDate = ui.values[1];
        }
      });

    }

    // Update info of current date
    function updateInfo() {
      var el = $("#currentDate").find('label');
      $(el).text(getDateStringInfo());
    }

    // Get date string to show current date of animation
    function getDateStringInfo(){
      if (memberDateType == 'list'){
        return moment(memberDatesList[memberCurrentDate].replace('Z', '')).format("lll");
      } else {
        return moment(memberCurrentDate).format("lll") + " - " + getEndTime().format("lll");
      }
    }

    // Get end time when type of date is continuous
    function getEndTime(){
      var endDate = moment(memberCurrentDate.replace('Z', '')).add(1, 'days');
      return endDate;
    }

    // Function to update layer time
    function setTime() {
      if (memberDateType == 'list')
        setListTime();
      else 
        setCountinuousTime();
    }

    // Set new layer time when date type is list
    function setListTime(){
      if (memberCurrentDate > memberFinalDate){
        memberCurrentDate = memberInitialDate;
      }
      memberLayer.dateInfo.initialDateIndex = memberCurrentDate;
      Layers.updateDateInfo(memberLayer.id, memberLayer.dateInfo);
      TerraMA2WebComponents.MapDisplay.updateLayerTime(memberLayer.id, memberDatesList[memberCurrentDate]);
      updateInfo();
      memberCurrentDate++;
    }

    // Set new layer time when date type is continuous
    function setCountinuousTime(){
      if (memberCurrentDate > memberFinalDate){
        memberCurrentDate = memberInitialDate;
      }
      var timeFormat = "YYYY-MM-DDTHH:mm:ss";
      var endDate = getEndTime().format(timeFormat);
      var rangeDate = memberCurrentDate + "/" + endDate + 'Z';
      TerraMA2WebComponents.MapDisplay.updateLayerTime(memberLayer.id, rangeDate);
      updateInfo();
      memberCurrentDate = endDate + 'Z';
    }

    // Pause animation
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

    // Start animation
    var play = function() {

      pause();

      if (!memberCurrentDate){
        memberCurrentDate = memberInitialDate;
      }
      updateInfo();

      showAnimationTools();

      animationId = window.setInterval(setTime, 1000);
    };

    // Reset the animation
    var reload = function(){
      memberCurrentDate = memberInitialDate;
    }

    // Close animation tools
    var closeAnimateTools = function(){
      pause();

      if(!$("#animate-tools").hasClass("hidden"))
        $("#animate-tools").addClass("hidden");
    }

    // Show animation tools and adjust css
    var showAnimationTools = function(){

      if($("#animate-tools").hasClass("hidden"))
        $("#animate-tools").removeClass("hidden");
    
      if($("#pauseAnimation").hasClass("hidden"))
        $("#pauseAnimation").removeClass("hidden");

      if(!$("#playAnimation").hasClass("hidden"))
        $("#playAnimation").addClass("hidden");

      if (memberDateType == 'list'){
        $("#currentDate").css('width', '145px');
        $("#currentDate > div").css('width', '139px');
        $("#reloadAnimation").css('left', '160px');
        $("#playAnimation").css('left', '200px');
        $("#pauseAnimation").css('left', '200px');
        $("#stopAnimation").css('left', '240px');
      } else {
        $("#currentDate").css('width', '300px');
        $("#currentDate > div").css('width', '294px');
        $("#reloadAnimation").css('left', '315px');
        $("#playAnimation").css('left', '355px');
        $("#pauseAnimation").css('left', '355px');
        $("#stopAnimation").css('left', '395px');
      }

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
      setDatesSlider: setDatesSlider,
      setLayerToAnimate: setLayerToAnimate,
      setDatesCalendar: setDatesCalendar
    }
  }
)