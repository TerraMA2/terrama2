$.TerraMAMonitor = {};

$.TerraMAMonitor.options = {
    sidebarToggleSelector: "[full_screen='true']",
}
$.TerraMAMonitor.pushMenu = {
    activate: function(toggleBtn){
        $(document).on('click', toggleBtn, function(e){
            e.preventDefault();
            if ($("body").hasClass('full_screen')){
                $("body").removeClass('full_screen');
                $("body").addClass('sidebar-mini'); 
                
                var body_height = $("body").height(); 
                var header_height = $(".main-header").height();
                $(".content-wrapper").css({'height': body_height - header_height});
                $(".content").css({'height':($(".content-wrapper").height())});
                $(".terrama2-map").width("auto");
            } else{
                $("body").addClass('full_screen');
                $("body").removeClass('sidebar-mini');
                $("body").css({'height':($("html").height())+'px'});
                
                $(".content-wrapper").css({'height':($("body").height())+'px'});
                $(".content").css({'height':($(".content-wrapper").height())});
                $(".terrama2-map").width("100%");
            }
            
			TerraMA2WebComponents.MapDisplay.updateMapSize();
        })
    }
}

$('#mini-toggle').click(function(){
    TerraMA2WebComponents.MapDisplay.updateMapSize();
});

/**
 * When window resize the map must follow the width
 */
$(window).resize(function() {
    if ($("body").hasClass('full_screen')){
        $(".terrama2-map").width("100%");
    }
    else {
        $(".terrama2-map").css({'width':($(".content").width())});
		TerraMA2WebComponents.MapDisplay.updateMapSize();
    }
});

$(function () {
    "use strict";

    var o = $.TerraMAMonitor.options;

    $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
    $(".terrama2-map").height("100%");
    $(".terrama2-map").width("auto");
    $(".content").css({'height':($(".content-wrapper").height())+'px'});

    $("#dynamic").find("div").each(function() {
        $(this).addClass("fa fa-exchange");
    });
    $("#static").find("div").each(function() {
        $(this).addClass("fa fa-folder-o");
    });
    $("#analysis").find("div").each(function() {
        $(this).addClass("fa fa-tasks");
    });
    $("#template").find("div").each(function() {
        $(this).addClass("fa fa-map");
    });
    $("#custom").find("div").each(function() {
        $(this).addClass("fa fa-link");
    });

    var leftArrow = "<span class='pull-right-container'> <i class='fa fa-angle-left pull-right'></i> </span>";
    
    $("#dynamic").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#static").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#analysis").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#template").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#custom").children("span").each(function() {
        $(this).append(leftArrow);
    });
});
