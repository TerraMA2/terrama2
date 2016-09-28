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
            
        })
    }
}
$(function () {
    "use strict";

    var o = $.TerraMAMonitor.options;

    $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
    $(".map").height("100%");
    $(".content").css({'height':($(".content-wrapper").height())+'px'});
    
});
