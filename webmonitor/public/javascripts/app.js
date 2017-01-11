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

$.TerraMAMonitor.tree = function (menu) {
var _this = this;
$(document).off('click', menu + ' li span')
    .on('click', menu + ' li span', function (e) {
        //Get the clicked link and the next element
        var $this = $(this);
        var checkElement = $this.next();

        //Check if the next element is a menu and is visible
        if ((checkElement.is('.treeview-menu')) && (checkElement.is(':visible')) && (!$('body').hasClass('sidebar-collapse'))) {
            //Close the menu
            checkElement.slideUp(500, function () {
                checkElement.removeClass('menu-open');
            });
            checkElement.parent("li").removeClass("active");
        }
        //If the menu is not visible
        else if ((checkElement.is('.treeview-menu')) && (!checkElement.is(':visible'))) {
            //Get the parent menu
            var parent = $this.parents('ul').first();
            //Get the parent li
            var parent_li = $this.parent("li");

            //Open the target menu and add the menu-open class
            checkElement.slideDown(500, function () {
                //Add the class active to the parent li
                checkElement.addClass('menu-open');
                parent.find('li.active').removeClass('active');
                parent_li.addClass('active');
            });
        }
        //if this isn't a link, prevent the page from being redirected
        if (checkElement.is('.treeview-menu')) {
            e.preventDefault();
        }
    });
};

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
    $.TerraMAMonitor.tree('.sidebar');
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
