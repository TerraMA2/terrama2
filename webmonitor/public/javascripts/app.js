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
            } else{
                $("body").addClass('full_screen');
            }
        })
    }
}
$(function () {
    "use strict";

    var o = $.TerraMAMonitor.options;

    $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
    
});
