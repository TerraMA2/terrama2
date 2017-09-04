"use strict";

define(
  ['components/Calendar',
    'components/Capabilities',
    'components/Slider',
    'components/Utils',
    'components/LayerStatus',
    'components/Layers',
    'components/AddLayerByUri',
    'components/Sortable',
    'components/Login',
    'enums/LayerStatusEnum',
    'TerraMA2WebComponents'
  ],
  function(Calendar, Capabilities, Slider, Utils, LayerStatus, Layers, AddLayerByUri, Sortable, Login, LayerStatusEnum, TerraMA2WebComponents) {

    var visibleLayers = [];
    var memberWindowHeight;
    var memberReducedHeight;

    // function to add class treeview-menu to work lte admin css behavior when sidebar is mini
    var addTreeviewMenuClass = function() {
      var customParentLi = $(".parent_li").children('ul');
      for(var i = 0; i < customParentLi.length; i++) {
        customParentLi[i].className += ' treeview-menu';
      }
    };

    var changeProjects = function() {
      var allLayers = Layers.getAllLayers();
      allLayers.forEach(function(layer) {
        if(layer.projectId) {
          Layers.removeLayerOfExplorer(layer);
        }
      });
      Layers.fillLayersData();
    };

    // Function to get initial date of the day
    var getInitialDateToCalendar = function(dates){
      var timeFormat = "YYYY-MM-DDTHH:mm:ss";
      var initialDate = moment(dates.endDate.replace("Z",""));
      initialDate.set({hour:0,minute:0,second:0,millisecond:0});
      var initialDateString = initialDate.format(timeFormat) + "Z";
      if (initialDate < moment(dates.startDate.replace("Z",""))){
        initialDateString = dates.startDate;
      }
      return initialDateString;
    };

    //Function to update map date on initial request
    var updateMapDate = function(layerId, dates){
      var layerTime = dates.startFilterDate + "/" + dates.endFilterDate;
      TerraMA2WebComponents.MapDisplay.updateLayerTime( /**id */ layerId, /** time */ layerTime);
    }

    var loadEvents = function() {
      $('#projects').on('change', changeProjects);

      $('#mini-toggle').click(function() {
        TerraMA2WebComponents.MapDisplay.updateMapSize();
      });

      /**
       * When window resize the map must follow the width
       */
      $(window).resize(function() {
        memberWindowHeight = $(window).height();
        memberReducedHeight = memberWindowHeight - $(".footer-monitor").outerHeight();

        $("#terrama2-map").height(memberReducedHeight + "px");
        $("#content").height(memberWindowHeight + "px");
        $(".content-wrapper").css('min-height', memberWindowHeight + "px");

        window.setTimeout(function() {
          $("#terrama2-map").width(($("body").hasClass('full_screen') ? "100%" : $("#content").width() + "px"));
          TerraMA2WebComponents.MapDisplay.updateMapSize();
        }, 100);

        $(".sidebar-menu").height((memberWindowHeight - 195) + "px");        
      });

      $('#close-alert').on('click', function() {
        var alertDiv = $(this).closest('div');
        alertDiv.addClass('hide');
      });

      $('#terrama2-layerexplorer').on('click', 'input.terrama2-layerexplorer-checkbox', function() {

        var layerid = $(this).closest('li').data('layerid');
        var layerObject = Layers.getLayerById(layerid);
        var isVisible = layerObject.visible;
        if(isVisible) {
          $('#terrama2-sortlayers').find('li#' + layerObject.htmlId).addClass('hide');
          Layers.changeLayerVisible(layerObject.id, false);
        } else {
          $('#terrama2-sortlayers').find('li#' + layerObject.htmlId).removeClass('hide');
          Layers.changeLayerVisible(layerObject.id, true);
        }

        $("#terrama2-map").trigger("setGetFeatureInfoToolSelect");
        $("#terrama2-map").trigger("createAttributesTable");
        $("#legend-box").trigger("setLegends");

        if(layerObject.status == LayerStatusEnum.NEW || layerObject.status == LayerStatusEnum.ALERT) {
          TerraMA2WebComponents.MapDisplay.updateLayerSourceParams(layerObject.id, {
            "": Date.now().toString()
          }, true);
          Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
        }

      });

      //change status icon when close the group layer
      $('.parent_li').on('click', function() {
        var parent = this.getAttribute('id');
        var parentLi = $("#" + parent);
        var parentLayer = Layers.getLayerById(parent);
        if(!parentLayer)
          return;

        if(parentLi.hasClass('open')) {
          if(parentLayer.status == LayerStatusEnum.NEW || parentLayer.status == LayerStatusEnum.ALERT)
            Layers.changeParentLayerStatus(parentLayer.id, LayerStatusEnum.ONLINE);
          else if(parentLayer.status == LayerStatusEnum.NEW_OFFLINE || parentLayer.status == LayerStatusEnum.ALERT)
            Layers.changeParentLayerStatus(parentLayer.id, LayerStatusEnum.OFFLINE);
        }
      });

      $("#inpe-image").on('click', function() {
        window.open('http://www.inpe.br/', '_blank');
      });

      $("#programa-queimadas-image").on('click', function() {
        window.open('http://www.inpe.br/queimadas/', '_blank');
      });

      $("#defra-image").on('click', function() {
        window.open('https://www.gov.uk/government/organisations/department-for-environment-food-rural-affairs', '_blank');
      });

      $("#world-bank-image").on('click', function() {
        window.open('http://www.worldbank.org/', '_blank');
      });

      $("#loginButton").on("click", function() {
        if($('#authentication-div').hasClass('hidden'))
          $('#authentication-div').removeClass('hidden');
        else
          $('#authentication-div').addClass('hidden');
      });

      $('#about-btn').on('click', function() {
        $('#about-dialog').dialog({
          width: 800,
          height: $(window).outerHeight() - 30,
          closeOnEscape: true,
          closeText: "",
          position: { my: 'top', at: 'top+15' },
          open: function() {
            $('.ui-dialog-titlebar-close').css('background-image', 'url(images/close.png)');
            $('.ui-dialog-titlebar-close').css('background-position', 'center');
            $('.ui-dialog-titlebar-close').css('background-size', '20px');
          },
          close: function() {
            $('.ui-dialog-titlebar-close').css('background-image', '');
            $('.ui-dialog-titlebar-close').css('background-position', '');
            $('.ui-dialog-titlebar-close').css('background-size', '');
          }
        });
      });
    };

    var loadSocketsListeners = function() {
      Utils.getWebAppSocket().on("notifyView", function() {
        Utils.getSocket().emit('retrieveNotifiedViews', { clientId: Utils.getWebAppSocket().id });
      });

      Utils.getWebAppSocket().on("removeView", function() {
        var allLayers = Layers.getAllLayers();
        var viewsToSend = {};

        for(var i = 0, allLayersLength = allLayers.length; i < allLayersLength; i++)
          viewsToSend[allLayers[i].id] = allLayers[i].private;

        Utils.getSocket().emit('retrieveRemovedViews', { clientId: Utils.getWebAppSocket().id, views: viewsToSend });
      });

      Utils.getWebAppSocket().on('viewReceived', function() {
        var allLayers = Layers.getAllLayers();
        var viewsToSend = {};

        for(var i = 0, allLayersLength = allLayers.length; i < allLayersLength; i++)
          viewsToSend[allLayers[i].id] = allLayers[i].private;

        Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, views: viewsToSend });
      });

      Utils.getWebAppSocket().on('projectReceived', function(project) {
        var newProject = true;

        $("#projects > option").each(function() {
          if(parseInt(this.value) === parseInt(project.id)) {
            this.text = project.name;
            newProject = false;
            return;
          }
        });

        if(newProject)
          $('#projects').append($('<option></option>').attr('value', project.id).text(project.name));
      });

      Utils.getWebAppSocket().on('projectDeleted', function(project) {
        $("#projects > option").each(function() {
          if(parseInt(this.value) === parseInt(project.id)) {
            var oldValue = $("#projects").val();

            this.remove();

            if(parseInt(this.value) === parseInt(oldValue))
              changeProjects();

            return;
          }
        });
      });

      Utils.getSocket().on("retrieveNotifiedViewsResponse", function(data) {
        for(var i = 0, viewsLength = data.views.length; i < viewsLength; i++) {
          var layerId = data.views[i].workspace + ":" + data.views[i].layer.name;
          var layerObject = Layers.getLayerById(layerId);

          if(layerObject) {
            Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ALERT);
            Layers.changeParentLayerStatus("alert", LayerStatusEnum.ALERT);
          }
        }
      });

      Utils.getSocket().on("retrieveRemovedViewsResponse", function(data) {
        for(var i = 0, viewsLength = data.views.length; i < viewsLength; i++) {
          var layerIdOrig = (data.views[i].layer && data.views[i].layer.name ? data.views[i].layer.name : data.views[i].layers[0]);
          var layerId = (data.views[i].workspace ? data.views[i].workspace + ":" + layerIdOrig : layerIdOrig);
          var layerObject = Layers.getLayerById(layerId);

          if(layerObject)
            Layers.removeLayer(layerObject);
        }
      });

      Utils.getSocket().on("retrieveViewsResponse", function(viewsData) {
        if(viewsData.projects) {
          $('#projects').empty();

          for(var i = 0, projectsLength = viewsData.projects.length; i < projectsLength; i++)
            $('#projects').append($('<option></option>').attr('value', viewsData.projects[i].id).text(viewsData.projects[i].name));
        }

        var currentProject = $("#projects").val();

        for(var i = 0, viewsLength = viewsData.views.length; i < viewsLength; i++) {
          var layerObject = Layers.createLayerObject(viewsData.views[i]);
          var newLayer = Layers.getLayerById(layerObject.id) == null ? true : false;

          if(newLayer) {
            Layers.addLayer(layerObject);

            if(!viewsData.initialRequest && layerObject.projectId == currentProject) {
              Layers.fillLayersData([layerObject]);
              Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
              Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
            }
          } else {
            Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
            Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
            Layers.getLayerCapabilities(layerObject.uriGeoServer, layerObject.workspace, layerObject.nameId, layerObject.id, layerObject.parent, true);
          }
        }

        if(viewsData.initialRequest)
          Layers.fillLayersData();
      });

      // Checking map server connection response
      Utils.getSocket().on('connectionResponse', function(data) {
        if(data.url) {
          //getting element to disable if there are no connection with map server
          var listElement = $("li[data-layerid='" + data.requestId + "'].treeview");
          var inputElement = listElement.find('input');
          var parent = listElement.attr('data-parentid')
          var layerObject = Layers.getLayerById(data.requestId);
          if(!layerObject)
            return;

          //if not connected disabled the layer selection
          if(!data.connected) {
            listElement.prop("title", "Map Server is not responding");
            if(inputElement.is(':checked'))
              inputElement.trigger("click");

            if(!inputElement.hasClass("disabled-content"))
              inputElement.addClass("disabled-content");

            Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.OFFLINE);
            Layers.changeParentLayerStatus(parent, LayerStatusEnum.OFFLINE);
          } else {
            listElement[0].removeAttribute("title");
            if(inputElement.hasClass("disabled-content"))
              inputElement.removeClass("disabled-content");

            var lastStatus = layerObject.status;
            if(lastStatus == LayerStatusEnum.OFFLINE) {
              Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
              Layers.changeParentLayerStatus(parent, LayerStatusEnum.ONLINE);

              Layers.getLayerCapabilities(layerObject.uriGeoServer, layerObject.workspace, layerObject.nameId, layerObject.id, layerObject.parent, true);
            }
          }
        }
      });

      Utils.getSocket().on('proxyResponseCapabilities', function(data) {
        try {
          var layerCapabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
          var layerIndex = layerCapabilities.map(function(c) {
            return c.name
          }).indexOf(data.layerName);

          if(layerIndex < 0)
            return;

          var listElement = $("li[data-layerid='" + data.parent + "']");
          var li = $(listElement).find("li[data-layerid='" + data.layerId + "']");

          if(li.length === 0)
            return;

          if(layerCapabilities[layerIndex].extent !== undefined) {
            var dateObject = {
              dates: layerCapabilities[layerIndex].extent
            };

            var span = "";

            if(layerCapabilities[layerIndex].extent instanceof Array) {
              if(layerCapabilities[layerIndex].extent.length > 1 && (!data.update || !$(li).has("#terrama2-slider").length)) {
                var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':', '') + "'></div></div>";
                $(li).append(sliderDiv);
                span += "<span id='terrama2-slider' class='terrama2-datepicker-icon'> <i class='fa fa-sliders'></i></span>";
              }
              dateObject.initialDateIndex = dateObject.dates.length - 1;
            } else if(layerCapabilities[layerIndex].extent instanceof Object) {
              if(!data.update || !$(li).has("#terrama2-calendar").length)
                span += "<span id='terrama2-calendar' class='terrama2-datepicker-icon'> <i class='fa fa-calendar'></i></span>";

              dateObject.startFilterDate = getInitialDateToCalendar(layerCapabilities[layerIndex].extent);
              dateObject.endFilterDate = layerCapabilities[layerIndex].extent.endDate;
              updateMapDate(data.layerId, dateObject);
            }

            $(li).append($(span));
            Layers.updateDateInfo(dateObject, data.layerId);
          }

          if(layerCapabilities[layerIndex].boundingBox !== undefined) {
            Layers.updateBoundingBox(layerCapabilities[layerIndex].boundingBox, data.layerId);
          }
        } catch(e) {
          console.log(e);
          return;
        }
      });

      Utils.getSocket().on('proxyResponse', function(data) {
        if(data.requestId == "GetFeatureInfoTool") {
          var featureInfo = data.msg;
          var featuresLength = featureInfo.features.length;

          if(featuresLength > 0) {
            var firesAttributes = "";

            for(var i = 0; i < featuresLength; i++) {
              firesAttributes += "<table class=\"table table-striped\"><tbody>";

              for(var key in featureInfo.features[i].properties) {
                firesAttributes += "<tr><td><strong>" + key + "</strong></td><td>" + featureInfo.features[i].properties[key] + "</td></tr>";
              }

              firesAttributes += "</tbody></table>";
              if(featuresLength > (i + 1)) firesAttributes += "<hr/>";
            }

            $('#feature-info-box').html(firesAttributes);

            $('#feature-info-box').dialog({
              dialogClass: "feature-info-box",
              title: "Attributes of layer: " + data.params.layerName,
              width: 400,
              height: 380,
              modal: false,
              resizable: true,
              draggable: true,
              closeOnEscape: true,
              closeText: "",
              position: {
                my: 'top',
                at: 'top+75'
              },
              open: function() {
                $('.ui-dialog-titlebar-close').css('background-image', 'url(images/close.png)');
                $('.ui-dialog-titlebar-close').css('background-position', 'center');
                $('.ui-dialog-titlebar-close').css('background-size', '20px');
              },
              close: function() {
                $('.ui-dialog-titlebar-close').css('background-image', '');
                $('.ui-dialog-titlebar-close').css('background-position', '');
                $('.ui-dialog-titlebar-close').css('background-size', '');
              }
            });
          }
        } else {
          try {
            var capabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
            AddLayerByUri.fillModal(capabilities);
          } catch(e) {
            $('#layersModal').modal('hide');
            $("#terrama2Alert > p > strong").text('Invalid URL!');
            $("#terrama2Alert > p > span").text('Error to find capabilities.');
            $("#terrama2Alert").removeClass('hide');
          }
        }
      });
    };

    var loadLayout = function() {
      memberWindowHeight = $(window).height();
      memberReducedHeight = memberWindowHeight - $(".footer-monitor").outerHeight();

      $.TerraMAMonitor = {};

      $.TerraMAMonitor.options = {
        sidebarToggleSelector: "[full_screen='true']",
      };

      $.TerraMAMonitor.pushMenu = {
        activate: function(toggleBtn) {
          $(document).on('click', toggleBtn, function(e) {
            e.preventDefault();

            if($("body").hasClass('full_screen')) {
              $("body").removeClass('full_screen');
              $("body").addClass('sidebar-mini');

              $("#terrama2-map").width("auto");

              $('.logo').css('margin-top', '');
            } else {
              $("body").addClass('full_screen');
              $("body").removeClass('sidebar-mini');

              $("#terrama2-map").width("100%");

              $('.logo').css('margin-top', '-2px');
            }

            TerraMA2WebComponents.MapDisplay.updateMapSize();
          })
        }
      };

      $.TerraMAMonitor.tree = function(menu) {
        var _this = this;
        $(document).off('click', menu + ' li span').on('click', menu + ' li span', function(e) {
          //Get the clicked link and the next element
          var $this = $(this);
          var checkElement = $this.next();

          //Check if the next element is a menu and is visible
          if((checkElement.is('.treeview-menu')) && (checkElement.is(':visible')) && (!$('body').hasClass('sidebar-collapse'))) {
            //Close the menu
            checkElement.slideUp(500, function() {
              checkElement.removeClass('menu-open');
            });
            checkElement.parent("li").removeClass("active");
          }
          //If the menu is not visible
          else if((checkElement.is('.treeview-menu')) && (!checkElement.is(':visible'))) {
            //Get the parent menu
            var parent = $this.parents('ul').first();
            //Get the parent li
            var parent_li = $this.parent("li");

            //Open the target menu and add the menu-open class
            checkElement.slideDown(50, function() {
              //Add the class active to the parent li
              checkElement.addClass('menu-open');
              parent.find('li.active').removeClass('active');
              parent_li.addClass('active');
            });
          }
          //if this isn't a link, prevent the page from being redirected
          if(checkElement.is('.treeview-menu')) {
            e.preventDefault();
          }
        });
      };

      var o = $.TerraMAMonitor.options;

      $.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
      $.TerraMAMonitor.tree('.sidebar');

      $("#content").height(memberWindowHeight + "px");
      $(".content-wrapper").css('min-height', memberWindowHeight + "px");
      $("#terrama2-map").height(memberReducedHeight + "px");
      $(".sidebar-menu").height((memberWindowHeight - 195) + "px");

      var mapWidthInterval = window.setInterval(function() {
        $("#terrama2-map").width($("#content").width() + "px");
      }, 100);
      window.setTimeout(function() {
        clearInterval(mapWidthInterval);
      }, 2000);

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
        $(this).append("<span class='pull-right-container'> <i class='fa fa-plus pull-right' style='margin-top: 1px;'></i> </span>");
      });
    };

    var init = function() {
      if(message != "") {
        $("#terrama2Alert > p > strong").text('');
        $("#terrama2Alert > p > span").text(message);
        $("#terrama2Alert").removeClass('hide');
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("custom", "Custom", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("custom", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["custom"],
          name: "Custom"
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("template", "Template", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("template", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["template"],
          name: "Template"
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("static", "Static Data", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("static", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["static"],
          name: "Static Data"
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("dynamic", "Dynamic Data", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("dynamic", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["dynamic"],
          name: "Dynamic Data"
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("analysis", "Analysis", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("analysis", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["analysis"],
          name: "Analysis"
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("alert", "Alert", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("alert", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["alert"],
          name: "Alert"
        });
        Layers.addLayer(layerObject);
      }

      //Adding open map street
      if(TerraMA2WebComponents.MapDisplay.addOSMLayer("osm", "OpenStreetMap", "OpenStreetMap", false, "terrama2-layerexplorer", false)) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("osm", "template", null, "treeview unsortable terrama2-truncate-text", null);
        var layerObject = Layers.createLayerObject({
          layers: ["osm"],
          name: "OpenStreetMap",
          type: "template"
        });
        Layers.addLayer(layerObject);
        LayerStatus.addLayerStatusIcon("osm");
        Layers.changeLayerStatus("osm", LayerStatusEnum.ONLINE);
      }

      addTreeviewMenuClass();
      LayerStatus.addGroupSpanIcon();
      Layers.addLayersToSort();
      Sortable.setSortable();
      Layers.changeParentLayerStatus("template", LayerStatusEnum.ONLINE);

      // Check connections every 30 seconds
      var intervalID = setInterval(function() {
        var allLayers = Layers.getAllLayers();

        allLayers.forEach(function(layerObject) {
          if($("#projects").val() == layerObject.projectId) {
            Utils.getSocket().emit('checkConnection', {
              url: layerObject.uriGeoServer,
              requestId: layerObject.id
            });
          }
        });

        $.post(BASE_URL + "check-authentication", function(data) {
          if(data.isAuthenticated && $("#loginButton .fa-circle").hasClass("hidden"))
            Login.signin(null, data.username);
          else if(!data.isAuthenticated && $("#loginButton .fa-times").hasClass("hidden"))
            Login.signout();
        });
      }, 30000);

      loadSocketsListeners();
      loadEvents();
      loadLayout();
      $("#osm input").trigger("click");

      Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, initialRequest: true });
    };

    return {
      init: init
    };
  }
);