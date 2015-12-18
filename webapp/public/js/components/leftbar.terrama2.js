var LeftBar = function(terrama2) {

  //$('#terrama2-leftbar-container').append(terrama2.getConfig().getConfJsonHTML().LeftBarVisibilityControlButton);

  $(document).ready(function() {
    $('.terrama2-leftbar-button').on('click', function(e) {
      if($(this).hasClass("terrama2-hide-leftbar-button") || $(this).hasClass("terrama2-show-leftbar-button")) {
        if($("#terrama2-leftbar").hasClass("terrama2-hide-leftbar")) {
          $("#terrama2-leftbar").removeClass("terrama2-hide-leftbar");

          $(this).removeClass("terrama2-show-leftbar-button");
          $(this).addClass("terrama2-hide-leftbar-button");
          $(this).find(' > i').attr("class", "glyphicon glyphicon-chevron-left");

          $(".terrama2-leftbox").removeClass("hide");
        } else {
          $("#terrama2-leftbar").addClass("terrama2-hide-leftbar");

          $(this).removeClass("terrama2-hide-leftbar-button");
          $(this).addClass("terrama2-show-leftbar-button");
          $(this).find(' > i').attr("class", "glyphicon glyphicon-chevron-right");

          $(".terrama2-leftbox").addClass("hide");
        }
      } else {
        var box = $(this).attr('terrama2-box');


        if($("#" + box).hasClass("terrama2-leftbox-left")) {

          if($("#" + box).css("left") < '0px') {
            $(".terrama2-right-arrow").remove();
            $('.terrama2-leftbar-button').removeClass("active");
            $(".terrama2-leftbox").removeClass("active");
            $("#" + box).addClass("active");
            $(".terrama2-leftbox").animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300, queue: false });

            $("#" + box).animate({ left:'51px', opacity: 1 }, { duration: 300, queue: false });

            $(this).append(terrama2.getConfig().getConfJsonHTML().LeftBarButtonArrow);
            $(this).addClass("active");
          } else {
            $(".terrama2-right-arrow").remove();

            $("#" + box).removeClass("active");
            $("#" + box).animate({ left: '-100%', opacity: 0 }, { duration: 300 });
            $(this).removeClass("active");
          }
        } else {


          if($("#" + box).css("width") === '0px') {
            $(".terrama2-right-arrow").remove();
            $('.terrama2-leftbar-button').removeClass("active");
            $(".terrama2-leftbox").removeClass("active");
            $("#" + box).addClass("active");
            $(".terrama2-leftbox").animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300, queue: false });
            $(".terrama2-leftbox-left").animate({ left: '-100%', opacity: 0 }, { duration: 300 });

            if($("#" + box).hasClass("terrama2-leftbox-fullscreen")) {
              var width = $("#terrama2-container").parent().width() - 51;
              $("#" + box).animate({ width:width, padding:'1em', opacity:1, left:'51px' }, { duration: 300, queue: false });
            } else {
              $("#" + box).animate({ width:'25em', padding:'1em', opacity:1 }, { duration: 300, queue: false });
            }

            $(this).append(terrama2.getConfig().getConfJsonHTML().LeftBarButtonArrow)
            $(this).addClass("active");
          } else {
            $(".terrama2-right-arrow").remove();

            $("#" + box).removeClass("active");
            $("#" + box).animate({ width: 0, padding: 0, opacity: 0 }, { duration: 300 });
            $(this).removeClass("active");
          }


      }
    }

      e.stopPropagation();
    });

    $(window).resize(function(){
      $(".terrama2-leftbox-fullscreen").each(function() {
        if($(this).width() !== 0) {
          var width = $("#terrama2-container").parent().width() - 51;
          $(this).css({ width:width });
        }
      });
    });

    $(".terrama2-resizable-horizontal").resizable({
      handles: 'e'
    });
  });
}
