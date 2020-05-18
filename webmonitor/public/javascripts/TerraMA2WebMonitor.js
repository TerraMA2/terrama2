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

    //Function to update slider date on initial request
    var updateMapSliderDate = function(layerId, date){
      var timeFormat = moment(date.replace('Z', '')).format("YYYY-MM-DDThh:mm:ss") + "Z";
      TerraMA2WebComponents.MapDisplay.updateLayerTime(layerId, date);
    }

    var changeLanguage = function(language) {
      i18next.changeLanguage(language, function() {
        $.post(BASE_URL + "languages", { locale: language }, function() {
          $(".dropdown-btn").css("display", "none");
          $(".dropdown-btn." + language + "-img").css("display", "");
          $(".dropdown-btn ~ span").text("");
          $(".dropdown-btn." + language + "-img + span").text($("#language-"+language).text());

          $(".close-slider").click();

          Utils.changeLanguage(language);
          Utils.translate("body");
        });
      });
    };

    var loadEvents = function() {

      $('#projects').on('change', changeProjects);

      $('#mini-toggle').click(function() {
        TerraMA2WebComponents.MapDisplay.updateMapSize();
      });

      changeLanguage(USER_CONFIG.language);

      // Language change events

      $("#language-pt").parent().on("click", function() {
        changeLanguage("pt");
      });

      $("#language-es").parent().on("click", function() {
        changeLanguage("es");
      });

      $("#language-en").parent().on("click", function() {
        changeLanguage("en");
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

        $(".sidebar-menu").height((memberWindowHeight - 195) + "px");
      });

      $('#close-alert').on('click', function() {
        var alertDiv = $(this).closest('div');
        alertDiv.addClass('hide');
      });

      $('#terrama2-layerexplorer').on('click', '.checkbox input', function() {

        var layerid = $(this).closest('li').data('layerid');
        var layerObject = Layers.getLayerById(layerid);
        var isVisible = layerObject.visible;
        var hiddenLayer = false;

        if(isVisible) {
          $('#terrama2-sortlayers').find('li#' + layerObject.htmlId).addClass('hide');
          Layers.changeLayerVisible(layerObject.id, false);
          hiddenLayer = true;
        } else {
          $('#terrama2-sortlayers').find('li#' + layerObject.htmlId).removeClass('hide');
          Layers.changeLayerVisible(layerObject.id, true);

          if (layerObject.subLayers) {
            for(let subLayer of layerObject.subLayers) {
              TerraMA2WebComponents.MapDisplay.addImageWMSLayer(subLayer.id, subLayer.name, subLayer.name, subLayer.uriGeoServer + '/ows', subLayer.serverType, true, false, "terrama2-layerexplorer", { version: "1.1.0" })
            }
          }
        }

        $("#terrama2-map").trigger("setGetFeatureInfoToolSelect");
        $("#terrama2-map").trigger("createAttributesTable");
        $("#legend-box").trigger("setLegends");

        if(hiddenLayer){
          TerraMA2WebComponents.MapDisplay.getMap().getLayers().array_.forEach(e =>{
            if(e.values_.name === layerid){
              TerraMA2WebComponents.MapDisplay.getMap().removeLayer(e);
            }
          });
          hiddenLayer = false;
        }

        if(layerObject.status == LayerStatusEnum.NEW || layerObject.status == LayerStatusEnum.ALERT) {
          TerraMA2WebComponents.MapDisplay.updateLayerSourceParams(layerObject.id, {
            "": Date.now().toString()
          }, true);
          Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
        }

      });

      $('#terrama2-layerexplorer').on('click', 'li.layer', function() {
        var layerId = this.getAttribute('data-layerid');
        var layerObject = Layers.getLayerById(layerId);
        if(!layerObject)
          return;

        if(layerObject.status == LayerStatusEnum.NEW || layerObject.status == LayerStatusEnum.ALERT)
          Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.ONLINE);
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

      $("#loginButton").on("click", function() {
        if($('#authentication-div').hasClass('hidden'))
          $('#authentication-div').removeClass('hidden');
        else
          $('#authentication-div').addClass('hidden');
      });

      $('#about-btn').on('click', function() {
        $('#about-dialog').dialog({
          resizable: true,
          draggable: true,
          width: 600,
          height: 500,
          title: "",
          closeOnEscape: true,
          closeText: "",
          position: { my: 'center', at: "center", of: $("#content") },
          open: function() {
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-image', 'url(images/close.png)');
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-position', 'center');
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-size', '20px');
            $(this).parent().find('.ui-dialog-title').append('<span id=\'about-dialog-title-prefix\'></span>');
            Utils.setTagContent('.ui-dialog-title > #about-dialog-title-prefix', 'About');
          },
          close: function() {
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-image', '');
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-position', '');
            $(this).parent().find('.ui-dialog-titlebar-close').css('background-size', '');
          }
        });
      });

      $('.template .checkbox').mousedown(function() {
        if(!$(this).is(':checked')) {
          var checked = $(".template .checkbox input:checked");
          for(var i = 0, checkedLength = checked.length; i < checkedLength; i++){
            checked[i].click();
          }
        }
      });

      $('#auto-update-on').click(function(){
        $('#auto-update-on').addClass("hidden");
        $('#auto-update-off').removeClass("hidden");
      });

      $('#auto-update-off').click(function(){
        $('#auto-update-off').addClass("hidden");
        $('#auto-update-on').removeClass("hidden");
      });
    };

    var checkIfAutoUpdate = function(layer){
      var isAutoUpdate = $('#auto-update-off').hasClass("hidden");
      var isVisible = $("#" + layer.htmlId + " input").is(":checked");
      if (isAutoUpdate && isVisible){
        $("#" + layer.htmlId + " .checkbox input").trigger("click");
        $("#" + layer.htmlId + " .checkbox input").trigger("click");
        Layers.changeLayerStatus(layer.id, LayerStatusEnum.NEW);
        Layers.changeParentLayerStatus(layer.parent, LayerStatusEnum.NEW);
      }
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

      Utils.getWebAppSocket().on('viewReceived', async function() {
        var allLayers = Layers.getAllLayers();
        var viewsToSend = {};

        for(var i = 0, allLayersLength = allLayers.length; i < allLayersLength; i++)
          viewsToSend[allLayers[i].id] = allLayers[i].private;

        let flag = false;
        try {
          flag = await Utils.isAuthenticated();
        } catch (err) {
          console.warn("Error checking authentication", err);
        }

        Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, views: viewsToSend, token: flag ? Utils.getToken(): "" });
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
          var currentProject = $("#projects").val();

          if(layerObject && layerObject.projectId == currentProject) {
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

      Utils.getSocket().on('retrieveViewsError', (errMsg) => (
        console.error(errMsg)
      ));

      Utils.getSocket().on("retrieveViewsResponse", function(viewsData) {
        if ($.isEmptyObject(viewsData)) {
          console.error(`Could not retrieve views from WebApplication.`);
          return;
        }

        if(viewsData.projects) {
          $('#projects').empty();

          for(var i = 0, projectsLength = viewsData.projects.length; i < projectsLength; i++)
            $('#projects').append($('<option></option>').attr('value', viewsData.projects[i].id).text(viewsData.projects[i].name));
        }

        var defaultProject = USER_CONFIG.project;
        if (defaultProject){
          $("#projects").val($('#projects option').filter(function () { return $(this).html() == defaultProject; }).val());
        }

        var currentProject = $("#projects").val();
        for(var i = 0, viewsLength = viewsData.views.length; i < viewsLength; i++) {
          const currentView = viewsData.views[i];

          for(let layer of currentView.layers) {
            const workspace = viewsData.views[i].workspace;

            const layerName = workspace ? `${workspace}:${layer}` : layer;

            viewsData.views[i].properties = [
              {
                key: "Layer Name",
                value: layerName
              }
            ];

            var layerObject = Layers.createLayerObject(currentView);
            var createdLayer = Layers.getLayerById(layerObject.id);

            if(!createdLayer) {
              Layers.addLayer(layerObject);

              if(!viewsData.initialRequest && layerObject.projectId == currentProject) {
                Layers.fillLayersData([layerObject]);
                Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
                Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
              }
            } else if(layerObject.projectId == currentProject) {
              if (!createdLayer.subLayers)
                createdLayer.subLayers = [];

              const { subLayers } = createdLayer;
              const foundSubLayer = subLayers.find(internalLayer => internalLayer.id === layerObject.id);
              if (!foundSubLayer) {
                const subLayer = Layers.createLayerObject(currentView, layerName);
                subLayer.id = layerName;
                subLayer.name = layerName;
                subLayers.push(subLayer);
                currentView.subLayers = subLayers;
              }

              Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.NEW);
              Layers.changeParentLayerStatus(layerObject.parent, LayerStatusEnum.NEW);
              Layers.getLayerCapabilities(layerObject.uriGeoServer, layerObject.workspace, layerObject.nameId, layerObject.id, layerObject.parent, true);
              checkIfAutoUpdate(layerObject);
            }
          }
        }

        if(viewsData.initialRequest)
          Layers.fillLayersData();

        // Layers.addLayersToSort();
        const userLayers = USER_CONFIG.selectedLayers;
        for (let i = 0; i < userLayers.length; i++) {
          const layer = userLayers[i];
          $("li[title='"+layer+"'] input").trigger('click');
        }
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
            Utils.setTagContent(listElement, "Map Server is not responding", "title");

            if(inputElement.is(':checked'))
              inputElement.trigger("click");

            if(!inputElement.hasClass("disabled-content"))
              inputElement.addClass("disabled-content");

            Layers.changeLayerStatus(layerObject.id, LayerStatusEnum.OFFLINE);
            Layers.changeParentLayerStatus(parent, LayerStatusEnum.OFFLINE);
          } else {
            listElement[0].setAttribute("title", layerObject.name);
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
              if(layerCapabilities[layerIndex].extent.length >= 1 && (!data.update || !$(li).has("#terrama2-slider").length)) {
                var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':', '') + "'></div></div>";
                $(li).append(sliderDiv);
                span += "<i id='terrama2-slider' class='fa fa-sliders'></i>";
              }
              dateObject.initialDateIndex = dateObject.dates.length - 1;

              var layerObject = Layers.getLayerById(data.layerId);
              if (data.update){
                if ($('#auto-update-off').hasClass("hidden")){
                  dateObject.initialDateIndex = dateObject.dates.length - 1;
                  updateMapSliderDate(data.layerId, dateObject.dates[dateObject.initialDateIndex]);
                } else {
                  dateObject.initialDateIndex = layerObject.dateInfo.initialDateIndex;
                }
              } else {
                dateObject.initialDateIndex = dateObject.dates.length - 1;
                updateMapSliderDate(data.layerId, dateObject.dates[dateObject.initialDateIndex]);
              }

            } else if(layerCapabilities[layerIndex].extent instanceof Object) {
              if(!data.update || !$(li).has("#terrama2-calendar").length){
                span += "<i id='terrama2-calendar' class='fa fa-calendar'></i>";
              }
              if (data.update){
                if ($('#auto-update-off').hasClass("hidden")){
                  dateObject.startFilterDate = getInitialDateToCalendar(layerCapabilities[layerIndex].extent);
                  dateObject.endFilterDate = layerCapabilities[layerIndex].extent.endDate;
                  updateMapDate(data.layerId, dateObject);
                } else {
                  var layerObject = Layers.getLayerById(data.layerId);
                  dateObject.startFilterDate = layerObject.dateInfo.startFilterDate;
                  dateObject.endFilterDate = layerObject.dateInfo.endFilterDate;
                }
              } else {
                dateObject.startFilterDate = getInitialDateToCalendar(layerCapabilities[layerIndex].extent);
                dateObject.endFilterDate = layerCapabilities[layerIndex].extent.endDate;
                updateMapDate(data.layerId, dateObject);
              }

            }
            $("li[data-layerid='" + data.layerId + "'] .dropdown-layer-tools ul").append(span);
            // $(li).append($(span));
            Layers.updateDateInfo(dateObject, data.layerId);
          }

          if(layerCapabilities[layerIndex].boundingBox !== undefined) {
            Layers.updateBoundingBox(layerCapabilities[layerIndex].boundingBox, data.layerId);

            var layer = Layers.getLayerById(data.layerId);

            var bbox = layerCapabilities[layerIndex].boundingBox[0] + "," + layerCapabilities[layerIndex].boundingBox[1] + "," + layerCapabilities[layerIndex].boundingBox[2] + "," + layerCapabilities[layerIndex].boundingBox[3];
            var getMapUrl = layer.uriGeoServer + "/wms?request=GetMap&service=WMS&version=1.1.1&layers=" + data.layerId + "&width=500&height=500&format=image/png&bbox=" + bbox;
            var getFeatureUrl = layer.uriGeoServer + "/wfs?service=wfs&version=1.1.0&request=GetFeature&typeName=" + data.layerId + "&outputFormat=application/json&maxFeatures=1";

            Layers.addProperty({
              key: "WMS",
              value: "<a href=\"" + getMapUrl + "\" target=\"_blank\">GetMap</a>"
            }, data.layerId);

            var jsonData = {
              url: getFeatureUrl,
              format: "json",
              requestId: "GetFeature",
              params: {
                key: "WFS",
                value: "<a href=\"" + getFeatureUrl + "\" target=\"_blank\">GetFeature</a>",
                layerId: data.layerId
              }
            };

            const zoomToLayer = USER_CONFIG.zoomToLayer;
            if (layerCapabilities[layerIndex].title == zoomToLayer) {
              $("li[title='"+zoomToLayer+"'] input").trigger('click');
              TerraMA2WebComponents.MapDisplay.zoomToExtent(layerCapabilities[layerIndex].boundingBox);
            }
            Utils.getSocket().emit('proxyRequest', jsonData);
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

          var layerId = data.params.layerId;
          var layerData = Layers.getLayerById(layerId);
          var layerType = layerData.parent;

          const tableName = layerData.exportation.table;
          const viewId = layerData.viewId;

          $.get(BASE_URL + 'get-columns', {
            layer: layerData.id,
            geoserverUri: layerData.uriGeoServer
          },
            function(response) {
              if(response.fields.length > 0) {                
                var fields = [];
                response.fields.forEach(function(field){
                  fields.push(field.name);
                });

                $.get(ADMIN_URL + 'api/datasetid', {
                  tableName: tableName,
                  viewId: viewId
                },
                  function(response) {
                    if(response[0].data_set_id){
                      var dataSetid = response[0].data_set_id;
                      $.get(ADMIN_URL + 'api/attributes', {
                        dataSetid: dataSetid,
                        viewId: viewId
                      },
                        function(response) {
                          var attributesResponseStr = null;
                          var attributesJson = null;                    
      
                          if(typeof response !== 'undefined' && response.length > 0 && response[0].value){
                            var attributesResponseStr = response[0].value;
                            var attributesJson = JSON.parse(attributesResponseStr);

                            if (layerType == "static"){

                              attributesJson.forEach(e => {
                                  if(e.name == "gid"){
                                    e.visible = false;
                                  }
                              });

                            }
                          }
                          
                          if(featuresLength > 0) {
                            var firesAttributes = "";
                            for(var i = 0; i < featuresLength; i++) {
                              var imageUrl = null;
      
                              firesAttributes += "<table class=\"table table-striped\"><tbody>";
                              var firesAttributesRows = "";
      
                              if(attributesJson != null){

                                if(attributesJson.length !== Object.keys(featureInfo.features[i].properties).length){
                                  attributesJson.forEach(function(columnFilter){
                                    if(Object.keys(featureInfo.features[i].properties).includes(columnFilter.name)){
                                      if(featureInfo.features[i].properties[columnFilter.name] === "picture") {
                                        imageUrl = featureInfo.features[i].properties[columnFilter.name];
                                      }else{
                                        if(featureInfo.features[i].properties[columnFilter.name] != null && typeof featureInfo.features[i].properties[columnFilter.name] == 'object'){                                            
                                          if(typeof featureInfo.features[i].properties[columnFilter.name].type !== 'undefined'){
                                            if(columnFilter.alias != "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter.name].type + "</td></tr>";
                                            }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter.name].type + "</td></tr>";
                                            }
                                          }else{
                                            if(columnFilter.alias != "" && columnFilter.visible == true){
                                              let propertiesObj = featureInfo.features[i].properties[columnFilter.name];
                                              for(let key in propertiesObj){
                                                firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td>" + propertiesObj[key] + "</td></tr>";
                                                break;
                                              }
                                            }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                              let propertiesObj = featureInfo.features[i].properties[columnFilter.name];
                                              for(let key in propertiesObj){
                                                firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td>" + propertiesObj[key] + "</td></tr>";
                                                break;
                                              }
                                            }
                                          }
                                        } else{
                                          if(columnFilter.alias != "" && columnFilter.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter.name] + "</td></tr>";
                                          }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter.name] + "</td></tr>";
                                          }                                        
                                        }
                                      }
                                    } else{
                                      if(columnFilter.name !== "fid"){
                                        if(layerType == "static" && featureInfo.features[i].geometry !== null){
                                          if(columnFilter.alias != "" && columnFilter.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                          }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                          }
                                        }else{
                                          if(typeof featureInfo.features[i].geometry_name !== 'undefined' && featureInfo.features[i].geometry_name !== null){
                                            if(columnFilter.alias != "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                            }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                            }
                                          } else{
                                            if(columnFilter.alias != "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.alias + "</strong></td><td> </td></tr>";
                                            }else if(columnFilter.alias == "" && columnFilter.visible == true){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter.name + "</strong></td><td> </td></tr>";
                                            }
                                          } 
                                        }
                                      }
                                    }
                                  });
                                } else{
                                  for(var key in featureInfo.features[i].properties) {
                                    if(key === "picture") {
                                      imageUrl = featureInfo.features[i].properties[key];
                                    } else {
                                      attributesJson.forEach(function(jsonElement){
                                        if(key == jsonElement.name){
                                          if(jsonElement.alias != "" && jsonElement.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + jsonElement.alias + "</strong></td><td>" + featureInfo.features[i].properties[key] + "</td></tr>";
                                          }else if(jsonElement.alias == "" && jsonElement.visible == true){
                                            firesAttributesRows += "<tr><td><strong>" + key + "</strong></td><td>" + featureInfo.features[i].properties[key] + "</td></tr>";
                                          }
                                        }
                                      });  
                                    }
                                  }
                                }      
                              }else{
                                if(fields.length !== Object.keys(featureInfo.features[i].properties).length){
                                  fields.forEach(function(columnFilter){
                                    if(Object.keys(featureInfo.features[i].properties).includes(columnFilter)){
                                      if(featureInfo.features[i].properties[columnFilter.name] === "picture") {
                                        imageUrl = featureInfo.features[i].properties[columnFilter.name];
                                      }else{
                                        if(featureInfo.features[i].properties[columnFilter] != null && typeof featureInfo.features[i].properties[columnFilter] == 'object'){                                            
                                          if(typeof featureInfo.features[i].properties[columnFilter].type !== 'undefined'){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter].type + "</td></tr>";
                                          }else{
                                            let propertiesObj = featureInfo.features[i].properties[columnFilter];
                                            for(let key in propertiesObj){
                                              firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td>" + propertiesObj[key] + "</td></tr>";
                                              break;
                                            }
                                          }
                                        } else{
                                          firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td>" + featureInfo.features[i].properties[columnFilter] + "</td></tr>";
                                        }
                                      }
                                    } else{
                                      if(columnFilter !== "fid"){
                                        if(layerType == "static" && featureInfo.features[i].geometry !== null){
                                          firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                        }else{
                                          if(typeof featureInfo.features[i].geometry_name !== 'undefined' && featureInfo.features[i].geometry_name !== null){
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td>" + featureInfo.features[i].geometry.type + "</td></tr>";
                                          } else{
                                            firesAttributesRows += "<tr><td><strong>" + columnFilter + "</strong></td><td> </td></tr>";
                                          } 
                                        }
                                      }
                                    }
                                  });
                                } else{
                                  for(var key in featureInfo.features[i].properties) {
                                    if(key === "picture") {
                                      imageUrl = featureInfo.features[i].properties[key];
                                    } else {
                                      firesAttributesRows += "<tr><td><strong>" + key + "</strong></td><td>" + featureInfo.features[i].properties[key] + "</td></tr>";
                                    }
                                  }
                                }
                              }                         
      
                              if(imageUrl) {
                                firesAttributes += "<tr><td colspan=\"2\"><a target=\"_blank\" href=\"" + imageUrl + "\"><img style=\"width: 100%;\" src=\"" + imageUrl + "\"/></a></td></tr>";
                              }
                
                              firesAttributes += firesAttributesRows + "</tbody></table>";
                              if(featuresLength > (i + 1)) firesAttributes += "<hr/>";
                            
                            }
      
                            $('#feature-info-box').html(firesAttributes);
      
                            $('#feature-info-box').dialog({
                              dialogClass: "feature-info-box",
                              title: "",
                              width: 400,
                              height: 380,
                              maxWidth: 600,
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
                                $(this).parent().find('.ui-dialog-content').css('white-space', 'normal');
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-image', 'url(images/close.png)');
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-position', 'center');
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-size', '20px');
                                $(this).parent().find('.ui-dialog-title').append('<span id=\'feature-info-dialog-title-prefix\'></span>' + data.params.layerName);
      
                                Utils.setTagContent('.ui-dialog-title > #feature-info-dialog-title-prefix', 'ATTRIBUTES-OF-LAYER-COLON');
                              },
                              close: function() {
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-image', '');
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-position', '');
                                $(this).parent().find('.ui-dialog-titlebar-close').css('background-size', '');
                              }
                            });
                          }
                        }
                      );  
                    }             
                  }
                );
              }
          });

          
        } else if(data.requestId == "GetFeature") {
          if(data.msg.hasOwnProperty("totalFeatures") && data.msg.totalFeatures > 0) {
            Layers.addProperty({
              key: data.params.key,
              value: data.params.value
            }, data.params.layerId);
          }
        } else {
          try {
            var capabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
            AddLayerByUri.fillModal(capabilities);
          } catch(e) {
            $('#layersModal').modal('hide');
            Utils.setTagContent("#terrama2Alert > p > strong", "Invalid URL!");
            Utils.setTagContent("#terrama2Alert > p > span", "CAPABILITIES-ERROR");
            $("#terrama2Alert").removeClass('hide');
          }
        }
      });
    };

    var loadLayout = function() {
      $.TerraMAMonitor = {};

      $.TerraMAMonitor.options = {
      };

      $(".custom-scrollbar-light").mCustomScrollbar({
        theme: "light-thick"
      });

      $(".custom-scrollbar-dark").mCustomScrollbar({
        theme: "dark-thick"
      });

      $.TerraMAMonitor.tree = function(menu) {
        var _this = this;
        $(document).off('click', menu + ' li span').on('click', menu + ' li span', function(e) {
          //Get the clicked link and the next element
          var $this = $(this);
          var checkElement = $this.next();

          //Check if the next element is a menu and is visible
          if((checkElement.is('.treeview-menu')) && (checkElement.is(':visible')) && (!$('body').hasClass('sidebar-collapse'))) {
            //Close the menu
            checkElement.slideUp(50, function() {
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

      $.TerraMAMonitor.tree('.sidebar');

      $("#dynamic > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-clock-o");
      });

      $("#static > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-folder");
      });

      $("#analysis > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-search");
      });

      $("#alert > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-warning");
      });

      $("#template > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-map");
      });

      $("#custom > .group-name > .sidebar-item-text").find("div").each(function() {
        $(this).addClass("fa fa-link");
      });

      var downArrow = "<i class='fa fa-angle-down'></i>";

      $("#dynamic > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
      });

      $("#static > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
      });

      $("#analysis > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
      });

      $("#alert > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
      });

      $("#template > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
      });

      $("#custom > .group-name > .sidebar-item-icon").each(function() {
        $(this).append(downArrow);
        $(this).append("<i class='fa fa-plus' style='margin-top: 1px;'></i>");
      });
    };

    var init = function() {

      if(message != "") {
        $("#terrama2Alert > p > strong").text('');
        $("#terrama2Alert > p > span").text(message);
        $("#terrama2Alert").removeClass('hide');
      }

      // if(TerraMA2WebComponents.MapDisplay.addLayerGroup("custom", "", "terrama2-layerexplorer")) {
      //   TerraMA2WebComponents.LayerExplorer.addLayersFromMap("custom", "terrama2-layerexplorer", null, "treeview unsortable", null);
      //   var layerObject = Layers.createLayerObject({
      //     layers: ["custom"],
      //     name: "Externals",
      //     description: null
      //   });
      //   Layers.addLayer(layerObject);
      // }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("template", "Templates", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("template", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["template"],
          name: "Templates",
          description: null
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("static", "Static data", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("static", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["static"],
          name: "Static Data",
          description: null
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("dynamic", "Dynamic data", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("dynamic", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["dynamic"],
          name: "Dynamic Data",
          description: null
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("analysis", "Analysis", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("analysis", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["analysis"],
          name: "Analysis",
          description: null
        });
        Layers.addLayer(layerObject);
      }

      if(TerraMA2WebComponents.MapDisplay.addLayerGroup("alert", "Alerts", "terrama2-layerexplorer")) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("alert", "terrama2-layerexplorer", null, "treeview unsortable", null);
        var layerObject = Layers.createLayerObject({
          layers: ["alert"],
          name: "Alerts",
          description: null
        });
        Layers.addLayer(layerObject);
      }

      //Adding open map street
      if(TerraMA2WebComponents.MapDisplay.addOSMLayer("osm", "OpenStreetMap", "OpenStreetMap", false, "terrama2-layerexplorer", false)) {
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("osm", "template", null, "treeview unsortable terrama2-truncate-text sidebar-subitem template", null);
        var layerObject = Layers.createLayerObject({
          layers: ["osm"],
          name: "OpenStreetMap",
          type: "template",
          description: null
        });
        Layers.addLayer(layerObject);
        LayerStatus.addLayerStatusIcon("osm");
      }

      var gebcoUrl = "http://www.gebco.net/data_and_products/gebco_web_services/web_map_service/mapserv?request=getmap&service=wms";
      if(TerraMA2WebComponents.MapDisplay.addTileWMSLayer("gebco_08_grid", "GEBCO", "GEBCO", gebcoUrl, "mapserver", false, false, "terrama2-layerexplorer", { version: "1.3.0", format: "image/jpeg" })){
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("gebco_08_grid", "template", null, "treeview unsortable terrama2-truncate-text sidebar-subitem template", null);
        var layerObject = Layers.createLayerObject({
          layers: ["gebco_08_grid"],
          name: "GEBCO",
          type: "template",
          description: null
        });
        Layers.addLayer(layerObject);
        LayerStatus.addLayerStatusIcon("gebco_08_grid");
      }

      var sentinelURL = "https://b.s2maps-tiles.eu/wms?";
      if(TerraMA2WebComponents.MapDisplay.addTileWMSLayer("s2cloudless", "Sentinel 2", "Sentinel 2", sentinelURL, "mapserver", false, false, "terrama2-layerexplorer", { version: "1.1.1", format: "image/jpeg" })){
        TerraMA2WebComponents.LayerExplorer.addLayersFromMap("s2cloudless", "template", null, "treeview unsortable terrama2-truncate-text sidebar-subitem template", null);
        var layerObject = Layers.createLayerObject({
          layers: ["s2cloudless"],
          name: "Senrinel 2",
          type: "template",
          description: null
        });
        Layers.addLayer(layerObject);
        LayerStatus.addLayerStatusIcon("s2cloudless");
      }

      addTreeviewMenuClass();
      LayerStatus.addGroupSpanIcon();
      Layers.addLayersToSort();
      Sortable.setSortable();

      // Setting the names of the layers groups
      Utils.setTagContent("#custom > span > span:nth-child(3n)", "Externals");
      Utils.setTagContent("#template > span > span:nth-child(3n)", "Templates");
      Utils.setTagContent("#static > span > span:nth-child(3n)", "Static Data");
      Utils.setTagContent("#dynamic > span > span:nth-child(3n)", "Dynamic Data");
      Utils.setTagContent("#analysis > span > span:nth-child(3n)", "Analysis");
      Utils.setTagContent("#alert > span > span:nth-child(3n)", "Alerts");

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

      var defaultTemplate = 'osm';
      var userConfigTemplate = USER_CONFIG.template;
      if (userConfigTemplate == 'osm') {
        defaultTemplate = 'osm';
      } else if (userConfigTemplate == 'gebco') {
        defaultTemplate = 'gebco_08_grid';
      } else if (userConfigTemplate == 'sentinel') {
        defaultTemplate = 's2cloudless';
      }
      $("#"+defaultTemplate+" input").trigger("click");
      Utils.isAuthenticated()
        .then(flag => {
          Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, initialRequest: true, token: flag ? Utils.getToken() : "" });
        })
        .catch(error => console.error(error));
    };

    return {
      init: init
    };
  }
);
