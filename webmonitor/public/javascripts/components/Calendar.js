'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {

    /**
     * It makes a input hidden with date limits (startDate & endDate)
     * 
     * @returns {string}
     */
    var makeHelperDatePicker = function(capability) {
      if(!capability) {
        throw new Error("Invalid capability while making date picker");
      }
      /**
       * It represents a input hidden to store min date and max date
       * @type {string}
       */
      var datepicker = "<input type='hidden' name='" + capability.name + "'";

      if(capability.extent instanceof Array) {
        datepicker += " data-min-date='" + capability.extent[0] + "' data-max-date='" + capability.extent[capability.extent.length - 1] + "'>";
      } else if(capability.extent instanceof Object) {
        datepicker += " data-min-date='" + capability.extent.startDate + "' data-max-date='" + capability.extent.endDate + "' start-date='' end-date=''" + ">";
      }

      return datepicker;
    };

    /**
     * It updates the calendar start, end date
     * @param {object} capability - object with dates 
     * @param {string} layerId - layer id to update
     */
    var updateDatePicker = function(capability, layerId) {
      var listElement = $("li[data-layerid='" + layerId + "']");
      var layerName = layerId.split(":")[1];
      var calendar = listElement.find("input[name='" + layerName + "']");

      if(capability.extent instanceof Array) {
        calendar.attr("data-min-date", capability.extent[0]);
        calendar.attr("data-max-date", capability.extent[capability.extent.length - 1]);
      } else if(capability.extent instanceof Object) {
        calendar.attr("data-min-date", capability.extent.startDate);
        calendar.attr("data-max-date", capability.extent.endDate);
      }
    };

    var init = function() {
      $("#terrama2-layerexplorer").on("click", "#terrama2-calendar", function(event) {
        var self = $(this);
        var parentLi = $(self).parent();
        var capability = $(self).attr("data-layerid");
        var calendar = $(self).find("input[id='" + capability + "']");
        var hidden = $(self).find("input[type='hidden']");
        var minDate = $(hidden).attr('data-min-date');
        var maxDate = $(hidden).attr('data-max-date');
        var startDate = $(hidden).attr('start-date');
        var endDate = $(hidden).attr('end-date');

        if(!minDate || !maxDate) {
          return;
        }
        var mMinDate = moment(minDate);
        var mMaxDate = moment(maxDate);
        var mStartDate;
        var mEndDate;
        if(!startDate || !endDate) {
          mStartDate = mMaxDate;
          mEndDate = mMaxDate;
        } else {
          mStartDate = moment(startDate);
          mEndDate = moment(endDate);
        }
        if(calendar.length === 0) {
          calendar = $("<input type='text' id='" + capability + "' value='' style='display:none;'>");
          /**
           * Important: Due date range picker implementation, we must configure startDate and endDate. Otherwise, it will display NaN
           * date values because the start date is None, so the component take on current timestamp.
           */
          $(calendar).daterangepicker({
            "timePicker": true,
            "minDate": mMinDate,
            "startDate": mStartDate,
            "endDate": mEndDate,
            "maxDate": mMaxDate,
            "timePicker24Hour": true,
            "opens": "center"
          });

          $(this).append(calendar);

          $(calendar).on('click', function(e) {
            e.stopPropagation();
          })

          $(calendar).on("apply.daterangepicker", function(ev, picker) {
            //  DO REQUEST
            var layerId = $(parentLi).attr("data-layerid");
            var timeFormat = "YYYY-MM-DDTHH:mm:ss";
            var pickerStartDate = picker.startDate.format(timeFormat);
            var pickerEndDate = picker.endDate.format(timeFormat);
            $(hidden).attr('start-date', pickerStartDate);
            $(hidden).attr('end-date', pickerEndDate);

            $("#attributes-table-select").trigger("setAttributesTable");

            var layerTime = pickerStartDate + "Z/" + pickerEndDate + "Z";
            TerraMA2WebComponents.MapDisplay.updateLayerTime( /**id */ layerId, /** time */ layerTime);
          });
        } else {

          $(calendar).daterangepicker({
            "timePicker": true,
            "minDate": mMinDate,
            "startDate": mStartDate,
            "endDate": mEndDate,
            "maxDate": mMaxDate,
            "timePicker24Hour": true,
            "opens": "center"
          });

          $(calendar).on("apply.daterangepicker", function(ev, picker) {
            //  DO REQUEST
            var layerId = $(parentLi).attr("data-layerid");
            var timeFormat = "YYYY-MM-DDTHH:mm:ss";
            var pickerStartDate = picker.startDate.format(timeFormat);
            var pickerEndDate = picker.endDate.format(timeFormat);
            $(hidden).attr('start-date', pickerStartDate);
            $(hidden).attr('end-date', pickerEndDate);

            $("#attributes-table-select").trigger("setAttributesTable", []);

            var layerTime = pickerStartDate + "Z/" + pickerEndDate + "Z";
            TerraMA2WebComponents.MapDisplay.updateLayerTime( /**id */ layerId, /** time */ layerTime);
          });
        }

        $(calendar).trigger('click');
      });
    };

    return {
      makeHelperDatePicker: makeHelperDatePicker,
      updateDatePicker: updateDatePicker,
      init: init
    };
  }
);