(function() {
  'use strict';

  $("#layersModalBody").on("click", "li[data-layerid]", function(event) {
    var self = $(this);
    var capability = $(self).attr("data-layerid");
    var calendar = $(self).find("input[id='"+ capability +"']");
    var hidden = $(self).find("input[type='hidden']");
    var minDate = $(hidden).attr('data-min-date');
    var maxDate = $(hidden).attr('data-max-date');
    if (!minDate || !maxDate) {
      return;
    }
    if (calendar.length === 0) {
      calendar = $("<input type='text' id='" + capability + "' value='' style='display:none;'>");

      var mMinDate = moment(minDate);
      var mMaxDate = moment(maxDate);

      $(calendar).daterangepicker({
        "timePicker": true,
        "minDate": mMinDate,
        "startDate": mMinDate,
        "endDate": mMaxDate,
        "maxDate": mMaxDate,
        "timePicker24Hour": true,
        "opens": "center"
      });

      $(this).append(calendar);

      $(calendar).on('click', function(e) {
        e.stopPropagation();
      })
    }

    $(calendar).trigger('click');
  });

  // $('#demo').daterangepicker({
  //     "timePicker": true,
  //     "timePicker24Hour": true,
  //     "startDate": "10/07/2016",
  //     "endDate": "10/13/2016"
  // }, function(start, end, label) {
  //   console.log("New date range selected: ' + start.format('YYYY-MM-DD') + ' to ' + end.format('YYYY-MM-DD') + ' (predefined range: ' + label + ')");
  // });
} ());