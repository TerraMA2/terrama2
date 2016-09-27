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
                $(".content-wrapper").height('100%');
            } else{
                $("body").addClass('full_screen');
                $("body").removeClass('sidebar-mini');
                $(".content-wrapper").css({'height':($(document).height())+'px'});
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
