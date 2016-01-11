var LeftBar = function(terrama2) {

  var loadEvents = function() {
    $('.terrama2-leftbar-button').on('click', function(e) {
      var box = $(this).attr('terrama2-box');

      if($("#" + box).css("width") === '0px' || ($("#" + box).hasClass("terrama2-leftbox-fullscreen-fullwidth") && $("#" + box).css("opacity") === '0')) {
        $(".terrama2-right-arrow").remove();
        $('.terrama2-leftbar-button').removeClass("active");
        $(".terrama2-leftbox").removeClass("active");
        $("#" + box).addClass("active");
        $(".terrama2-leftbox").animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300, queue: false });

        if($("#" + box).hasClass("terrama2-leftbox-fullscreen")) {
          if($("#" + box).hasClass("terrama2-leftbox-fullscreen-fullwidth")) {
            $("#" + box).removeClass("terrama2-leftbox-fullscreen-fullwidth");
          }
          var width = $("#terrama2-container").parent().width() - 51;
          $("#" + box).animate({ width: width, padding: '1em', opacity: 1, left: '51px' }, { duration: 300, queue: false });
        } else {
          $("#" + box).animate({ width: '25em', padding: '1em', opacity: 1 }, { duration: 300, queue: false });
        }

        $(this).append(terrama2.getConfig().getConfJsonHTML().LeftBarButtonArrow)
        $(this).addClass("active");
      } else {
        $(".terrama2-right-arrow").remove();

        $("#" + box).removeClass("active");
        $("#" + box).animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300, queue: false });
        $(this).removeClass("active");
      }

      e.stopPropagation();
    });

    $(window).resize(function() {
      $(".terrama2-leftbox-fullscreen").each(function() {
        if($(this).width() !== 0) {
          var width = $("#terrama2-container").parent().width() - 51;
          $(this).css({ width:width });
        }
      });
    });
  }

  $(document).ready(function() {
    $(".terrama2-leftbar-button").append('<div class="terrama2-leftbar-button-separate"></div>');
    loadEvents();
  });
}
