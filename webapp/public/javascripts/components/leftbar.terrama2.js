/** @class LeftBar - Left menu bar where is possible to add buttons to trigger content boxes. */
var LeftBar = function(terrama2) {

  /**
   * Load the LeftBar events
   */
  var loadEvents = function() {
    $('.terrama2-leftbar-button').on('click', function(e) {
      var box = $(this).attr('terrama2-box');

      // if true, the user wants to open the left box, otherwise, he wants to close it
      if($("#" + box).css("width") === '0px' || ($("#" + box).hasClass("terrama2-leftbox-fullscreen-fullwidth") && $("#" + box).css("opacity") === '0')) {
        $(".terrama2-right-arrow").remove();
        $('.terrama2-leftbar-button').removeClass("active");
        $(".terrama2-leftbox").removeClass("active");
        $("#" + box).addClass("active");
        $(".terrama2-leftbox").animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300, queue: false });

        if($("#" + box).hasClass("terrama2-leftbox-fullscreen")) {
          if($("#" + box).hasClass("terrama2-leftbox-fullscreen-fullwidth")) {
            $("#" + box).removeClass("terrama2-leftbox-fullscreen-fullwidth").addClass("terrama2-leftbox-fullscreen-fullwidth_");
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

    // Window resize event
    $(window).resize(function() {
      // Loop through the full screen left boxes
      $(".terrama2-leftbox-fullscreen").each(function() {
        // Code block to resize the full screen left boxes
        if($(this).width() !== 0) {
          var width = $("#terrama2-container").parent().width() - 51;
          $(this).css({ width:width });
        }

        // Code block to restore the full width boxes according to the new size of the screen
        if($(this).hasClass("terrama2-leftbox-fullscreen-fullwidth_")) {
          $(this).removeClass("terrama2-leftbox-fullscreen-fullwidth_").addClass("terrama2-leftbox-fullscreen-fullwidth");
        }
      });
    });
  }

  $(document).ready(function() {
    $(".terrama2-leftbar-button").append('<div class="terrama2-leftbar-button-separate"></div>');
    loadEvents();
  });
}
