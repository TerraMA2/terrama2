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
            } else{
                $("body").addClass('full_screen');
                $("body").removeClass('sidebar-mini');
                $("body").css({'height':($("html").height())+'px'});
                
                $(".content-wrapper").css({'height':($("body").height())+'px'});
                $(".content").css({'height':($(".content-wrapper").height())});
            }
            
			TerraMA2WebComponents.MapDisplay.updateMapSize();
        })
    }
}

$('#mini-toggle').click(function(){
    TerraMA2WebComponents.MapDisplay.updateMapSize();
});

$(function () {
    "use strict";

    var o = $.TerraMAMonitor.options;

    $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
    $(".terrama2-map").height("100%");
    $(".terrama2-map").width("auto");
    $(".content").css({'height':($(".content-wrapper").height())+'px'});

    $("#Dynamic").find("div").each(function() {
        $(this).addClass("fa fa-exchange");
    });
    $("#Static").find("div").each(function() {
        $(this).addClass("fa fa-folder-o");
    });
    $("#Analysis").find("div").each(function() {
        $(this).addClass("fa fa-tasks");
    });
    $("#Template").find("div").each(function() {
        $(this).addClass("fa fa-map");
    });
    $("#Custom").find("div").each(function() {
        $(this).addClass("fa fa-link");
    });

    var leftArrow = "<span class='pull-right-container'> <i class='fa fa-angle-left pull-right'></i> </span>";
    
    $("#Dynamic").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#Static").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#Analysis").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#Template").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#Custom").children("span").each(function() {
        $(this).append(leftArrow);
    });
});
