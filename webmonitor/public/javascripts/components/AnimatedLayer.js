'use strict';

define(
  ['components/Layers', 'components/Utils', 'TerraMA2WebComponents'],
  function(Layers, Utils, TerraMA2WebComponents) {

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
    // Current date of animation - index of type if list, date string if continuous (used for translation)
    var memberTranslationDate;
    // Period date to animation
    var memberPeriodDate = 1;
    // Id used to get id of setInterval() and use to stop the animation in clearInterval()
    var memberAnimationId = null;

    // Wind layer identified to animate
    var windLayer;
    // Initial size of the animated arrows (wind data)
    var arrowInitialLength = 0;
    // Final size of the animated arrows (wind data)
    var arrowFinalLength = 4;
    // Current size of the animated arrows (wind data)
    var arrowCurrentLength = 0;
    // Array with values to animate wind data 
    var arrowLengthList = [0.5, 0.6, 0.7, 0.9, 1]; //[6, 9, 12, 14, 16]; //[0.2, 0.4, 0.6, 0.75, 1];    
    // Set layer info to animate
    var setLayerToAnimate = function(layer) {
      pause();
      memberLayer = layer;
      if(Array.isArray(memberLayer.dateInfo.dates))
        memberDateType = 'list';
      else
        memberDateType = 'continuous';
    };

    // Set info when type of dates is continuous
    var setDatesCalendar = function() {
      memberDatesObject = memberLayer.dateInfo.dates;
      memberInitialDate = memberDatesObject.startDate;
      memberFinalDate = memberDatesObject.endDate;
      memberCurrentDate = memberTranslationDate = memberInitialDate;
      setDateRangePicker();
    };

    var setDateRangePicker = function() {
      var calendar = $('#animation-calendar');
      calendar.daterangepicker({
        "timePicker": true,
        "minDate": moment(memberDatesObject.startDate),
        "startDate": moment(memberInitialDate),
        "endDate": moment(memberFinalDate),
        "maxDate": moment(memberDatesObject.endDate),
        "timePicker24Hour": true,
        "opens": "right",
        "locale": {
          "format": Utils.getTranslatedString("DATE-FORMAT"),
          "separator": " - ",
          "applyLabel": Utils.getTranslatedString("Apply"),
          "cancelLabel": Utils.getTranslatedString("Cancel"),
          "fromLabel": Utils.getTranslatedString("From"),
          "toLabel": Utils.getTranslatedString("To"),
          "customRangeLabel": Utils.getTranslatedString("Custom"),
          "weekLabel": Utils.getTranslatedString("WEEK-LABEL"),
          "daysOfWeek": [
            Utils.getTranslatedString("DAY-SU"),
            Utils.getTranslatedString("DAY-MO"),
            Utils.getTranslatedString("DAY-TU"),
            Utils.getTranslatedString("DAY-WE"),
            Utils.getTranslatedString("DAY-TH"),
            Utils.getTranslatedString("DAY-FR"),
            Utils.getTranslatedString("DAY-SA"),
          ],
          "monthNames": [
            Utils.getTranslatedString("January"),
            Utils.getTranslatedString("February"),
            Utils.getTranslatedString("March"),
            Utils.getTranslatedString("April"),
            Utils.getTranslatedString("May"),
            Utils.getTranslatedString("June"),
            Utils.getTranslatedString("July"),
            Utils.getTranslatedString("August"),
            Utils.getTranslatedString("September"),
            Utils.getTranslatedString("October"),
            Utils.getTranslatedString("November"),
            Utils.getTranslatedString("December")
          ],
          "firstDay": 1
        }
      });

      $(calendar).on("apply.daterangepicker", function(ev, picker) {
        var timeFormat = "YYYY-MM-DDTHH:mm:ss.SSS";
        memberInitialDate = picker.startDate.format(timeFormat) + 'Z';
        memberFinalDate = picker.endDate.format(timeFormat) + 'Z';
        memberCurrentDate = memberTranslationDate = picker.startDate.format(timeFormat) + 'Z';
      });
    };

    // Function to create and set the slider to user choose de range of animation
    var setDatesSlider = function() {
      memberDatesList = memberLayer.dateInfo.dates;
      memberInitialDate = 0;
      memberFinalDate = memberDatesList.length - 1;
      memberCurrentDate = memberTranslationDate = 0;
      setSlider();
    };

    var setSlider = function() {
      var slider = $("#dates" + memberLayer.id.replace(':', ''));
      var sliderParent = $(slider).parent();

      var initialLabelDate = $(sliderParent).find("#initialDate");
      var finalLabelDate = $(sliderParent).find("#finalDate");

      $(initialLabelDate).text(moment(memberDatesList[memberInitialDate].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));
      $(finalLabelDate).text(moment(memberDatesList[memberFinalDate].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));

      $(slider).slider({
        range: true,
        min: 0,
        max: memberDatesList.length - 1,
        values: [0, memberDatesList.length - 1],
        slide: function(event, ui) {
          $(initialLabelDate).text(moment(memberDatesList[ui.values[0]].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));
          $(finalLabelDate).text(moment(memberDatesList[ui.values[1]].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm"));
        },
        stop: function(event, ui) {
          memberInitialDate = ui.values[0];
          memberCurrentDate = memberTranslationDate = ui.values[0];
          memberFinalDate = ui.values[1];
        }
      });
    };

    // Update info of current date
    var updateInfo = function() {
      var el = $("#currentDate").find('label');
      $(el).text(getDateStringInfo());
    };

    // Get date string to show current date of animation
    var getDateStringInfo = function() {
      if(memberDateType == 'list') {
        return moment(memberDatesList[memberCurrentDate].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm");
      } else {
        return moment(memberCurrentDate.replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm") + " - " + getEndTime().format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm");
      }
    };

    // Get end time when type of date is continuous
    var getEndTime = function() {
      var frequencyValue = document.getElementById('frequency').value;
      var unitValue = document.getElementById('unitTime').value;
      var endDate = moment(memberCurrentDate.replace('Z', '')).add(parseInt(frequencyValue), unitValue);
      return endDate;
    };

    // Function to update layer time
    var setTime = function() {
      if (memberDateType == 'list')
        setListTime();
      else 
        setCountinuousTime();
    };

    // Set new layer time when date type is list
    var setListTime = function() {
      if(memberCurrentDate > memberFinalDate)
        memberCurrentDate = memberInitialDate;

      memberLayer.dateInfo.initialDateIndex = memberCurrentDate;
      Layers.updateDateInfo(memberLayer.id, memberLayer.dateInfo);
      TerraMA2WebComponents.MapDisplay.updateLayerTime(memberLayer.id, memberDatesList[memberCurrentDate]);
      updateInfo();

      memberTranslationDate = memberCurrentDate;

      memberCurrentDate++;
    };

    //Set new layer time when maximum length is reached (wind data)
    var setListLengthWind = function() {
      // if(TerraMA2WebMonitor.viewsData.views.style == "wind_style"){
      // }

      if(arrowCurrentLength > arrowFinalLength)
        arrowCurrentLength = arrowInitialLength;

      TerraMA2WebComponents.MapDisplay.updateLayerLength(windLayers, arrowLengthList[arrowCurrentLength]);
      arrowCurrentLength++;
    } 
      

    // Set new layer time when date type is continuous
    var setCountinuousTime = function() {
      if(memberCurrentDate > memberFinalDate)
        memberCurrentDate = memberInitialDate;

      var timeFormat = "YYYY-MM-DDTHH:mm:ss";
      var endDate = getEndTime().format(timeFormat);
      var rangeDate = memberCurrentDate + "/" + endDate + 'Z';
      TerraMA2WebComponents.MapDisplay.updateLayerTime(memberLayer.id, rangeDate);
      updateInfo();

      memberTranslationDate = memberCurrentDate;

      memberCurrentDate = endDate + 'Z';
    };

    // Pause animation
    var pause = function() {
      if (memberAnimationId !== null) {
        window.clearInterval(memberAnimationId);
        memberAnimationId = null;
      }

      if($("#playAnimation").hasClass("hidden"))
        $("#playAnimation").removeClass("hidden");

      if(!$("#pauseAnimation").hasClass("hidden"))
        $("#pauseAnimation").addClass("hidden");
    };

    // Start animation
    var play = function() {
      pause();

      if(!memberCurrentDate || memberCurrentDate > memberFinalDate)
        memberCurrentDate = memberTranslationDate = memberInitialDate;

      updateInfo();

      showAnimationTools();

      memberAnimationId = window.setInterval(setTime, 1000);
    };

    //Start wind animation
    var windLayers = undefined;
    var windStart = function(layerId){
      windLayers = layerId;
      setSizeArrowsTimer = window.setInterval(setListLengthWind, 1000);
    };

    //stop wind animation when the layer is disabled
    var windStop = function(layerId){
      if(setSizeArrowsTimer !== null){
        window.clearInterval(setSizeArrowsTimer);
        setSizeArrowsTimer = null;
      }      
    };

    // Reset the animation
    var reload = function() {
      memberCurrentDate = memberTranslationDate = memberInitialDate;
    };

    // Close animation tools
    var closeAnimateTools = function() {
      pause();

      if(!$("#animate-tools").hasClass("hidden"))
        $("#animate-tools").addClass("hidden");
    };

    // Show animation tools and adjust css
    var showAnimationTools = function() {
      if($("#animate-tools").hasClass("hidden"))
        $("#animate-tools").removeClass("hidden");
    
      if($("#pauseAnimation").hasClass("hidden"))
        $("#pauseAnimation").removeClass("hidden");

      if(!$("#playAnimation").hasClass("hidden"))
        $("#playAnimation").addClass("hidden");

      if(memberDateType == 'list') {
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
    };

    var loadEvents = function() {
      $("#play").on("click", play);

      $("#playAnimation").on("click", play);

      $("#pause").on("click", pause);

      $("#pauseAnimation").on("click", pause);

      $("#reload").on("click", reload);

      $("#reloadAnimation").on("click", reload);

      $("#stopAnimation").on("click", closeAnimateTools);

      $(document).on("setDatesCalendar", function() {
        if(memberLayer && memberLayer.dateInfo && memberLayer.dateInfo.dates) {
          if(!$("#animate-tools").hasClass("hidden") && $("#pauseAnimation").hasClass("hidden")) {
            if(memberDateType == 'list')
              var currentDateValue = moment(memberDatesList[memberTranslationDate].replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm");
            else {
              var frequencyValue = $('#frequency').val();
              var unitValue = $('#unitTime').val();
              var endDate = moment(memberTranslationDate.replace('Z', '')).add(parseInt(frequencyValue), unitValue);
              var currentDateValue = moment(memberTranslationDate.replace('Z', '')).format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm") + " - " + endDate.format(Utils.getTranslatedString("DATE-FORMAT") + " HH:mm");
            }

            $("#currentDate").find('label').text(currentDateValue);
          }

          if(!$("#layer-toolbox").hasClass("hidden")) {
            if(Array.isArray(memberLayer.dateInfo.dates))
              setSlider();
            else if(typeof memberLayer.dateInfo.dates === "object")
              setDateRangePicker();
          }
        }
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      init: init,
      setDatesSlider: setDatesSlider,
      setLayerToAnimate: setLayerToAnimate,
      setDatesCalendar: setDatesCalendar,
      setListLengthWind,
      windStart,
      windStop,
    };
  }
);