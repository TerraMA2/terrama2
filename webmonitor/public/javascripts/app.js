$.TerraMAMonitor = {};

$.TerraMAMonitor.options = {
    sidebarToggleSelector: "[full_screen='true']",
}
$.TerraMAMonitor.pushMenu = {
    activate: function(toggleBtn){
        $(document).on('click', toggleBtn, function(e){
            e.preventDefault();

            var window_height = $(window).height(); 
            var header_height = $("#terrama-header").height();
            var reduced_height = window_height - header_height;

            if($("body").hasClass('full_screen')) {
                $("body").removeClass('full_screen');
                $("body").addClass('sidebar-mini'); 

                $("#content").height(reduced_height + "px");
                $("#terrama2-map").height(reduced_height + "px");
                $("#terrama2-map").width("auto");
            } else {
                $("body").addClass('full_screen');
                $("body").removeClass('sidebar-mini');

                $("#content").height(window_height + "px");
                $("#terrama2-map").height(window_height + "px");
                $("#terrama2-map").width("100%");
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
            checkElement.slideDown(50, function () {
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
    var window_height = $(window).height(); 
    var header_height = $("#terrama-header").height();
    var reduced_height = window_height - header_height;

    if ($("body").hasClass('full_screen')) {
        var interval = window.setInterval(function() { $("#terrama2-map").width("100%"); }, 100);
        window.setTimeout(function() { clearInterval(interval); }, 2000);

        $("#terrama2-map").height(window_height + "px");
        $("#content").height(window_height + "px");
    } else {
        var interval = window.setInterval(function() { $("#terrama2-map").width($("#content").width() + "px"); }, 100);
        window.setTimeout(function() { clearInterval(interval); }, 2000);

        $("#terrama2-map").height(reduced_height + "px");
        $("#content").height(reduced_height + "px");
    }

    TerraMA2WebComponents.MapDisplay.updateMapSize();
});

$(function () {
    "use strict";

    var o = $.TerraMAMonitor.options;

    var window_height = $(window).height(); 
    var header_height = $("#terrama-header").height();
    var reduced_height = window_height - header_height;

    $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
    $.TerraMAMonitor.tree('.sidebar');

    $("#content").height(reduced_height + "px");
    $("#terrama2-map").height(reduced_height + "px");
    $("#terrama2-map").width("auto");

    $("#dynamic").find("div").each(function() {
        $(this).addClass("fa fa-clock-o");
    });
    $("#static").find("div").each(function() {
        $(this).addClass("fa fa-folder");
    });
    $("#analysis").find("div").each(function() {
        $(this).addClass("fa fa-search");
    });
    $("#alert").find("div").each(function() {
        $(this).addClass("fa fa-warning");
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
    $("#alert").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#template").children("span").each(function() {
        $(this).append(leftArrow);
    });
    $("#custom").children("span").each(function() {
        $(this).append(leftArrow);
    });
});
