"use strict";

define(
  ['components/Calendar', 'components/Capabilities', 'components/Slider', 'components/Utils', 'TerraMA2WebComponents'],
  function(Calendar, Capabilities, Slider, Utils, TerraMA2WebComponents) {

		var allLayers = [];
		var visibleLayers = [];
		var capabilities;
		var sliderCapabilities = [];
		var selectedLayers = [];
		var socket;
		var wepappsocket;
		var memberWindowHeight;
		var memberReducedHeight;

    // Add layers in layers explorer menu
    var fillLayersData = function(data) {
			for(var i in data) {
				if(!data[i].private || (data[i].private && userLogged)) {
					if (data[i].params && data[i].params.hasOwnProperty('mask') && data[i].layers.length > 1){
						var layerWithDate = Utils.getLayersListWithDate(data[i].params.mask, data[i].layers);
						var workspace = data[i].workspace;
						var layerName = data[i].name;
						var uriGeoServer = data[i].uriGeoserver;
						var serverType = data[i].serverType;
						var layerId = workspace + ":" + layerWithDate[layerWithDate.length - 1].layer;
						socket.emit('checkConnection', {url: uriGeoServer, requestId: layerId});
						var parent = data[i].type;
						if (allLayers.map(function (l){return l.id}).indexOf(layerId) > 0){
							continue;
						}
						if (TerraMA2WebComponents.MapDisplay.addTileWMSLayer(layerId, layerName, layerName, uriGeoServer + '/ows', serverType, false, false, "terrama2-layerexplorer", {version: "1.1.0"})){
							TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, data[i].type, null, "treeview unsortable terrama2-truncate-text", null);
							TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerType", data[i].type);
							TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerName", layerName);
							allLayers.push({id: layerId, name: layerName, url: uriGeoServer});

							var span = "";
							var listElement = $("li[data-layerid='" + data[i].type + "']");
							var li = $(listElement).find("li[data-layerid='" + layerId.split('.').join('\\.') +"']");

							if(li.length === 0)
								continue;

							var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':','') + "'></div></div>";
							$(li).append(sliderDiv);

							span += "<span id='terrama2-slider-mask' class='terrama2-datepicker-icon'><i class='fa fa-sliders'></i></span>";
							$(li).append($(span));

							setTemporarySlider(layerWithDate, layerId, layerName, workspace, uriGeoServer + '/ows', serverType, parent, 0);
						}
						changeGroupStatusIcon(parent, "working");
						addLayerStatusIcon(layerId.split('.').join('\\.'));
						changeLayerStatusIcon(layerId.split('.').join('\\.'), "working");
					} else {
						var workspace = data[i].workspace;
						var layerName = data[i].name;
						var uriGeoServer = data[i].uriGeoserver;
						var serverType = data[i].serverType;
						var layerId = workspace + ":" + data[i].layers[0];
						socket.emit('checkConnection', {url: uriGeoServer, requestId: layerId});
						if (allLayers.map(function (l){return l.id}).indexOf(layerId) > 0)
							continue;

						if (TerraMA2WebComponents.MapDisplay.addTileWMSLayer(layerId, layerName, layerName, uriGeoServer + '/ows', serverType, false, false, "terrama2-layerexplorer", {version: "1.1.0"})){
							TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, data[i].type, null, "treeview unsortable terrama2-truncate-text", null);
							TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerType", data[i].type);
							TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerName", layerName);
							allLayers.push({id: layerId, name: layerName, url: uriGeoServer});
							if (data[i].type == 'analysis' || data[i].type == 'dynamic'){
								var url = uriGeoServer + '/' + workspace + '/' + data[i].layers[0] + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';
								var getCapabilitiesUrl = {
									layerName: data[i].layers[0],
									layerId: layerId,
									parent: data[i].type,
									url: url,
									format: 'xml',
									update: false
								}
								socket.emit('proxyRequestCapabilities', getCapabilitiesUrl);
							}
						}
						changeGroupStatusIcon(data[i].type, "working");
						addLayerStatusIcon(layerId);
						changeLayerStatusIcon(layerId, "working");
					}
				}
			}
    };

    var addLayerWithSlider = function(listLayers, layerId, layerName, workspace, server, serverType, parent, initialValue) {
			if (TerraMA2WebComponents.MapDisplay.addTileWMSLayer(layerId, layerName, layerName, server + '/ows', serverType, false, false, "terrama2-layerexplorer", {version: "1.1.0"})){
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap(layerId, parent, null, "unsortable terrama2-truncate-text", null);
				TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerType", parent);
				TerraMA2WebComponents.MapDisplay.setLayerProperty(layerId, "layerName", layerName);
				addLayerToSort(layerId, layerName);
				allLayers.push({id: layerId, name: layerName, url: server + '/ows'});

				var span = "";
				var listElement = $("li[data-layerid='" + parent + "']");
				var li = $(listElement).find("li[data-layerid='" + layerId.split('.').join('\\.') +"']");
				
				if (li.length === 0) {
					return;
				}
				var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':','') + "'></div></div>";
				$(li).append(sliderDiv);

				span += "<span id='terrama2-slider-mask' class='terrama2-datepicker-icon'><i class='fa fa-sliders'></i></span>";
				$(li).append($(span));
				// .find(".terrama2-layerexplorer-checkbox-span")

				setTemporarySlider(listLayers, layerId, layerName, workspace, server, serverType, parent, initialValue);

				addLayerStatusIcon(layerId.split('.').join('\\.'));
				changeLayerStatusIcon(layerId.split('.').join('\\.'), "working");

				if (!$("#"+ parent).hasClass('open')){
					$("#"+ parent).addClass('open');
				}
				var children = $('#'+ parent).find(' > ul > li');
				children.show('fast');
				li.find('input').click();
			}
    };

    var setTemporarySlider = function(rangeDate, layerId, layerName, workspace, server, serverType, parent, initialValue) {
			var valMap = rangeDate;

			var slider = $("#slider" + layerId.replace(':','').split('.').join('\\.'));
			var sliderParent = $(slider).parent();

			var labelDate = $(sliderParent).find("label");
			$(labelDate).text(moment(rangeDate[initialValue].date).format("lll"));
			
			$(slider).slider({
				min: 0,
				max: valMap.length - 1,
				value: initialValue,
				slide: function(event, ui) {
					$(labelDate).text(moment(rangeDate[ui.value].date).format("lll"));
				},
				stop: function(event, ui) {
					var layer = valMap[ui.value];

					TerraMA2WebComponents.MapDisplay.removeLayer(layerId, "terrama2-layerexplorer");
					$("#terrama2-layerexplorer").find('li#' + layerId.replace(':','').split('.').join('\\.')).remove();
					$("#terrama2-sortlayers").find('li#' + layerId.replace(':','').split('.').join('\\.')).remove();

					var elementIndex = allLayers.map(function (l){return l.id}).indexOf(layerId);
					if (elementIndex >= 0){
						allLayers.splice(elementIndex, 1);
					}

					var elementVisibleIndex = visibleLayers.indexOf(layerId.replace(':',''));
					if (elementVisibleIndex >= 0){
						visibleLayers.splice(elementVisibleIndex, 1);
						setGetFeatureInfoToolSelect();
					}

					addLayerWithSlider(valMap, workspace + ":" + layer.layer, layerName, workspace, server, serverType, parent, ui.value);
				}
			});
    };

    var setSlider = function(rangeDate, layerId) {
			var valMap = rangeDate;

			var slider = $("#slider" + layerId.replace(':',''));
			var sliderParent = $(slider).parent();
			if (!$(sliderParent).is(":visible")) {
				$(sliderParent).show();
			} else {
				$(sliderParent).hide();
			}

			var labelDate = $(sliderParent).find("label");
			$(labelDate).text(moment(rangeDate[0]).format("lll"));
			
			$(slider).slider({
				min: 0,
				max: valMap.length - 1,
				value: 0,
				slide: function(event, ui) {
					$(labelDate).text(moment(rangeDate[ui.value]).format("lll"));
				},
				stop: function(event, ui) {                        
					Slider.doSlide(layerId, rangeDate[ui.value]);
				}       
			});
    };

    var addLayers = function() {
			var url = document.getElementById("wmsUri").value;
			if (!url) return;
			
			var jsonData = {
				url: url,
				format: 'xml'
			}
			socket.emit('proxyRequest', jsonData);
    };

    var resetMapMouseTools  = function() {
			TerraMA2WebComponents.MapDisplay.unsetMapSingleClickEvent();
			$('.mouse-function-btn').removeClass('active');
			$('#terrama2-map').removeClass('cursor-move');
			$('#terrama2-map').removeClass('cursor-pointer');
			$('#getAttributes').removeClass('with-select');
			$('#getAttributes > select').addClass('hidden');
			if($('#feature-info-box').hasClass('ui-dialog-content'))
				$('#feature-info-box').dialog('close');
    };

    var activateMoveMapTool = function() {
			$('#moveMap').addClass('active');
			$('#terrama2-map').addClass('cursor-move');
    };

    var featureInfo = function() {
			TerraMA2WebComponents.MapDisplay.setGetFeatureInfoUrlOnClick($('#getAttributes > select').val(), function(url) {
				if($('#feature-info-box').hasClass('ui-dialog-content'))
					$('#feature-info-box').dialog('close');

				if(url !== null) 
					socket.emit('proxyRequest', { url: url, requestId: 'GetFeatureInfoTool', format: 'json', params: { layerName: $('#getAttributes > select option:selected').text() } });
			});
    };

    var activateGetFeatureInfoTool = function() {
			$('#getAttributes').addClass('active');
			$('#terrama2-map').addClass('cursor-pointer');
			$('#getAttributes').addClass('with-select');
			$('#getAttributes > select').removeClass('hidden');
			setGetFeatureInfoToolSelect();
    };

    var setGetFeatureInfoToolSelect = function() {
			$('#getAttributes > select').empty();
			var showButton = false;

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');
				var layerName = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerName");
				var layerType = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerType");

				if(layerType !== "template" && layerType !== "custom") {
					$('#getAttributes > select').append($('<option></option>').attr('value', layerId).text(layerName));
					if(!showButton) showButton = true;
				}
			}

			if(!showButton) {
				if(!$('#getAttributes').hasClass('hidden'))
					$('#getAttributes').addClass('hidden');

				resetMapMouseTools();
				activateMoveMapTool();
			} else {
				$('#getAttributes').removeClass('hidden');
				if(!$('#getAttributes > select').hasClass('hidden'))
					featureInfo();
			}
    };

    var createAttributesTable = function() {
			var showButton = false;

			$('#attributes-table-select > select').empty();

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');
				var layerName = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerName");
				var layerType = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerType");

				if(layerType !== "template" && layerType !== "custom") {
					$('#attributes-table-select > select').append($('<option></option>').attr('value', layerId).text(layerName));
					if(!showButton) showButton = true;
				}
			}

			if(!showButton) {
				$("#attributes-table-div").empty();

				if(!$('#table-div > div.main-div').hasClass('hidden'))
					$('#table-div > div.main-div').addClass('hidden');

				$("#table-div").css('display', 'none');

				if(!$('#tableButton').hasClass('hidden'))
					$('#tableButton').addClass('hidden');

				resetMapMouseTools();
				activateMoveMapTool();
			} else {
				$('#tableButton').removeClass('hidden');
			}
    };

    var setAttributesTable = function() {
			if($('#attributes-table-select > select').val() !== null) {
				var layerData = getLayerData($('#attributes-table-select > select').val());

				if(layerData !== null && layerData.id !== undefined && layerData.url !== undefined) {
					if($("#tableButton").hasClass('hidden'))
						$("#tableButton").removeClass('hidden');

					if($('#table-div > div.main-div').hasClass('hidden'))
						$('#table-div > div.main-div').removeClass('hidden');

					$.get(BASE_URL + 'get-columns',
						{
							layer: layerData.id,
							geoserverUri: layerData.url
						},
						function(response) {
							var columns = response.fields;
							var columnsLength = columns.length;
							var columnsArray = [];
							var titles = "";

							for(var i = 0; i < columnsLength; i++) {
								columnsArray.push({ "name": columns[i].name });
								titles += "<th>" + columns[i].name + "</th>";
							}

							$("#attributes-table-div").empty().html("<table class=\"table table-bordered table-hover\" id=\"attributes-table\"><thead>" + titles + "</thead><tfoot>" + titles + "</tfoot></table>");

							$('#attributes-table').DataTable({
								"order": [[0, "asc"]],
								"processing": true,
								"serverSide": true,
								"ajax": {
									"url": BASE_URL + "get-attributes-table",
									"type": "POST",
									"data": function(data) {
										data.layer = $('#attributes-table-select > select').val();
										data.geoserverUri = layerData.url;
									}
								},
								"columns": columnsArray,
								"language": {
									"emptyTable": "<p class='text-center'>Nenhum registro a ser exibido</p>",
									"info": "Exibindo _START_ at&eacute; _END_ de _TOTAL_ registros",
									"infoEmpty": "Exibindo 0 at&eacute; 0 de 0 registros",
									"infoFiltered": "(filtrado de _MAX_ registros)",
									"lengthMenu": "Exibir _MENU_ registros",
									"loadingRecords": "Carregando...",
									"processing": "Processando...",
									"search": "Pesquisa:",
									"zeroRecords": "<p class='text-center'>Nenhum registro encontrado</p>",
									"paginate": {
										"first": "Primeira",
										"last": "&Uacute;ltima",
										"next": "Pr&oacute;xima",
										"previous": "Anterior"
									}
								}
							});
						}
					);
				} else {
					$("#attributes-table-div").empty();

					if(!$('#table-div > div.main-div').hasClass('hidden'))
						$('#table-div > div.main-div').addClass('hidden');

					$("#table-div").css('display', 'none');

					if(!$("#tableButton").hasClass('hidden'))
						$("#tableButton").addClass('hidden');
				}
			} else {
				$("#attributes-table-div").empty();

				if(!$('#table-div > div.main-div').hasClass('hidden'))
					$('#table-div > div.main-div').addClass('hidden');

				$("#table-div").css('display', 'none');

				if(!$("#tableButton").hasClass('hidden'))
					$("#tableButton").addClass('hidden');
			}
    };

    var setLegends = function() {
			var html = "";

			for(var i = 0, visibleLayersLength = visibleLayers.length; i < visibleLayersLength; i++) {
				var layerId = $('#' + visibleLayers[i]).data('layerid');
				var layerName = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerName");
				var layerType = TerraMA2WebComponents.MapDisplay.getLayerProperty(layerId, "layerType");

				if(layerType !== "template" && layerType !== "custom") {
					var layerData = getLayerData(layerId);

					if(layerData !== null && layerData.id !== undefined && layerData.url !== undefined) {
						html += "<strong>" + layerName + "</strong><br/><img src='" + BASE_URL + "get-legend?layer=" + layerId + "&geoserverUri=" + layerData.url + "&random=" + Date.now().toString() + "'/>";
						if(visibleLayersLength > 1 && i < (visibleLayersLength - 1)) html += "<hr/>";
					}
				}
			}

			$('#legend-box > .legend-body').html((html !== "" ? html : "<strong>No data to show.</strong>"));
    };

    var getLayerData = function(layerId) {
			for(var i = 0, allLayersLength = allLayers.length; i < allLayersLength; i++) {
				if(layerId === allLayers[i].id) {
					return allLayers[i];
				}
			}

			return null;
    };

    var fillModal = function(capabilities) {
			var lis = "";
			var check = "<input type='checkbox' class='terrama2-layerviews-checkbox'/>";
			for (var i in capabilities) {
				lis += '<li data-layerid="' + capabilities[i].name +'">' + check + '<span>' + capabilities[i].title + '</span>' + '</li>';
			}
			
			var htmlList = '<ul id="layersList">' + lis + '</ul>';
			$('#layersModalBody').append(htmlList);
    };

    var saveLayers = function() {
			var url = document.getElementById("wmsUri").value;
			var parser = document.createElement('a');
			parser.href = url;
			var geoUrl = parser.protocol + '//' + parser.host + parser.pathname;

			selectedLayers.forEach(addInLayerExplorer);

			function addInLayerExplorer(element, index, array){
				for (var i in capabilities){
					if (capabilities[i].name == element){
						if (allLayers.map(function (l){return l.id}).indexOf(capabilities[i].name) > 0){
							continue;
						}
						if (TerraMA2WebComponents.MapDisplay.addTileWMSLayer(capabilities[i].name, capabilities[i].title, capabilities[i].title, geoUrl, "geoserver", false, false, "terrama2-layerexplorer", {version: "1.1.0"})) {
							TerraMA2WebComponents.LayerExplorer.addLayersFromMap(capabilities[i].name, "custom", true, "treeview unsortable terrama2-truncate-text", null);
							TerraMA2WebComponents.MapDisplay.setLayerProperty(capabilities[i].name, "layerType", "custom");
							TerraMA2WebComponents.MapDisplay.setLayerProperty(capabilities[i].name, "layerName", capabilities[i].title);
							allLayers.push({id: capabilities[i].name, name: capabilities[i].title, extent: capabilities[i].extent, url: geoUrl});
							addLayerToSort(capabilities[i].name, capabilities[i].title);

							var span = "";
							var listElement = $("li[data-layerid='custom']");
							var li = $(listElement).find("li[data-layerid='" + capabilities[i].name +"']");

							if(li.length === 0)
								continue;

							li.find('input').click();

							if(!capabilities[i].extent)
								continue;

							var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':','') + "'></div></div>";
							$(li).append(sliderDiv);

							if(capabilities[i].extent instanceof Array) {
								span += "<span id='terrama2-slider' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(capabilities[i]) + "<i class='fa fa-sliders'></i></span>";
							} else if (capabilities[i].extent instanceof Object) {
								span += "<span id='terrama2-calendar' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(capabilities[i]) + "<i class='fa fa-calendar'></i></span>";
							}
							$(li).append($(span));
						}
					}
				}

				if (!$("#custom").hasClass('open')){
					$("#custom").addClass('open');
				}
				var groupSpanName = $('#custom').find(' span.group-name');
				groupSpanName.click();
			}
			$("#layersModal").modal('hide');
    };

    var addLayersToSort = function() {
      var itens = "";
      var allLayersLength = allLayers.length;
      for (var i = allLayersLength -1; i >= 0; i--){
        var layerId = allLayers[i].id;
        if (layerId.includes(':')){
          layerId = layerId.replace(':','')
        }

        var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='opacity" + layerId + "'></div></div>";
        var spanIcon = "<span id='terrama2-opacity-slider' class='terrama2-datepicker-icon' data-toggle='tooltip' title='Change opacity'> <i class='fa fa-sliders'></i></span>"; 
        
        itens += '<li id="' + layerId + '" data-layerid="' + allLayers[i].id + '" data-parentid="terrama2-layerexplorer" class="hide">' + allLayers[i].name + sliderDiv + spanIcon + '</li>';
      }
      var list = '<ul>' + itens + '</ul>';
      $('#terrama2-sortlayers').append(list);
    };

    var setSortable = function() {
      $('#terrama2-sortlayers').sortable({
        items: "li",
        start: function(event, ui) {
        $(this).attr('data-previndex', (ui.item.context.parentNode.childElementCount - 2) - ui.item.index());
        },
        update: function(event, ui) {
        TerraMA2WebComponents.MapDisplay.alterLayerIndex(ui.item.attr('data-parentid'), $(this).attr('data-previndex'), (ui.item.context.parentNode.childElementCount - 1) - ui.item.index());
        $(this).removeAttr('data-previndex');
        }
      });
    };

    var addLayerToSort = function(layerId, layerName) {
      var _layerId = layerId.replace(':','');
      var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='opacity" + _layerId + "'></div></div>";
      var spanIcon = "<span id='terrama2-opacity-slider' class='terrama2-datepicker-icon' data-toggle='tooltip' title='Change opacity'> <i class='fa fa-sliders'></i></span>";

      var liHtml = '<li id="' + _layerId + '" data-layerid="' + layerId + '" data-parentid="terrama2-layerexplorer" class="hide">' + layerName + sliderDiv + spanIcon + '</li>';
      $('#terrama2-sortlayers').children('ul').prepend(liHtml);
    };

    // function to add class treeview-menu to work lte admin css behavior when sidebar is mini
    var addTreeviewMenuClass = function() {
      var customParentLi = $(".parent_li").children('ul');
      for (var i = 0; i < customParentLi.length; i++){
        customParentLi[i].className += ' treeview-menu';
      }
    };

    var addGroupSpanIcon = function() {
      var groupElement = $(".parent_li").children(".group-name");
      var span = "<span class='span-group-icon pull-left'> <img id='image-group-icon' src=''> </span>";
      groupElement.prepend(span);
    };

    var changeGroupStatusIcon = function(parent, status) {
      var statusElement = $("#"+parent).find('#image-group-icon');
      statusElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      statusElement.attr('src', statusImage);
    };

    var addLayerStatusIcon = function(layerId) {
      var _layerId = layerId.replace(":","");
      var layerElement = $('#terrama2-layerexplorer').find("#"+_layerId);
      var span = "<span class='span-layer-icon'> <img id='image-icon' src=''> </span>";
      layerElement.prepend(span);
    };

    var changeLayerStatusIcon = function(layerId, status) {
      var _layerId = layerId.replace(":","");
      var layerElement = $("#"+_layerId).find("#image-icon");
      layerElement.addClass("status-icon");
      var statusImage = getStatusIconUrl(status);
      layerElement.attr('src', statusImage);
    };

    var getStatusIconUrl = function(status) {
      switch(status){
        case "new":
          return BASE_URL + "images/status/yellow-black.gif";
        case "newerraccess":
          return BASE_URL + "images/status/yellow-gray.gif";
        case "working":
          return BASE_URL + "images/status/green_icon.svg";
        case "erraccess":
          return BASE_URL + "images/status/gray_icon.svg";
        case "alert":
          return BASE_URL + "images/status/red-black.gif";
        default:
        	return "";
      }
    };

    var loadEvents = function() {
			$('#mini-toggle').click(function(){
				TerraMA2WebComponents.MapDisplay.updateMapSize();
			});

			/**
			 * When window resize the map must follow the width
			 */
			$(window).resize(function() {
				if ($("body").hasClass('full_screen')) {
					var interval = window.setInterval(function() { $("#terrama2-map").width("100%"); }, 100);
					window.setTimeout(function() { clearInterval(interval); }, 2000);

					$("#terrama2-map").height(memberWindowHeight + "px");
					$("#content").height(memberWindowHeight + "px");
				} else {
					var interval = window.setInterval(function() { $("#terrama2-map").width($("#content").width() + "px"); }, 100);
					window.setTimeout(function() { clearInterval(interval); }, 2000);

					$("#terrama2-map").height(memberReducedHeight + "px");
					$("#content").height(memberReducedHeight + "px");
				}

				TerraMA2WebComponents.MapDisplay.updateMapSize();
			});

			$("#wmsUri").on('keyup', function (e) {
				if (e.keyCode == 13) {
					e.preventDefault();
					addLayers();
				}
			});

			$('#attributes-table-select > select').on('change', setAttributesTable);

			$('#tableButton > button').on('click', function() {
				if($('#table-div').css('display') === 'none') {
					$('#table-div').css('display', '');
					$('#table-div > div.main-div').removeClass('hidden');
					setAttributesTable();
				} else {
					if(!$('#table-div > div.main-div').hasClass('hidden'))
						$('#table-div > div.main-div').addClass('hidden');

					$('#table-div').css('display', 'none');
				}
			});

			$('#getAttributes > select').on('change', featureInfo);

			$('#moveMap').on('click', function() {
				resetMapMouseTools();
				activateMoveMapTool();
			});

			$('#getAttributes > button').on('click', function() {
				resetMapMouseTools();
				activateGetFeatureInfoTool();
			});

			$('#legendsButton > button').on('click', function() {
				if($('#legend-box').hasClass('hidden'))
					$('#legend-box').removeClass('hidden');
				else
					$('#legend-box').addClass('hidden');
			});

			document.getElementById("addLayers").addEventListener("click", addLayers);

			$('#layersModalBody').on('click', 'input.terrama2-layerviews-checkbox', function() {
				var layerid = $(this).closest('li').data('layerid');
				var index = selectedLayers.indexOf(layerid);

				if(index > -1)
					selectedLayers.splice(index, 1);
				else
					selectedLayers.push(layerid);
			});

			$('#layersModal').on('hidden.bs.modal', function(e) {
				selectedLayers = [];
				$("#layersList").remove();
			})

			$('#close-alert').on('click', function(){
				var alertDiv = $(this).closest('div');
				alertDiv.addClass('hide');
			})

			document.getElementById("saveLayers").addEventListener("click", saveLayers);

			$('#terrama2-layerexplorer').on('click', 'input.terrama2-layerexplorer-checkbox', function(){
				var completeLayerId = $(this).closest('li').data('layerid');
				var layerid = $(this).closest('li').data('layerid');

				if(layerid.includes(':')) {
					layerid = layerid.replace(':','')
				}

				var index = visibleLayers.indexOf(layerid);

				if(index > -1) {
					$('#terrama2-sortlayers').find('li#' + layerid.split('.').join('\\.')).addClass('hide');
					visibleLayers.splice(index, 1);
				} else {
					$('#terrama2-sortlayers').find('li#' + layerid.split('.').join('\\.')).removeClass('hide');
					visibleLayers.push(layerid);
				}

				setGetFeatureInfoToolSelect();
				createAttributesTable();
				setLegends();

				var imageElement = $(this).closest('li').find("#image-icon");

				if(imageElement.attr("src") == BASE_URL + "images/status/yellow-black.gif" || imageElement.attr("src") == BASE_URL + "images/status/red-black.gif") {
					TerraMA2WebComponents.MapDisplay.updateLayerSourceParams(completeLayerId, { "": Date.now().toString() }, true);
					changeLayerStatusIcon(layerid.split('.').join('\\.'), "working");
				}
			});

			//change status icon when close the group layer
			$('.parent_li').on('click', function() {
				var parent = this.getAttribute('id');
				var parentLi = $("#" + parent);

				if(parentLi.hasClass('open')) {
					var imageStatus = parentLi.find("#image-group-icon");
					var actualStatus = imageStatus.attr("src");

					if(actualStatus == BASE_URL + "images/status/yellow-black.gif" || actualStatus == BASE_URL + "images/status/red-black.gif")
						changeGroupStatusIcon(parent, "working");
					else if (actualStatus == BASE_URL + "images/status/yellow-gray.gif" || actualStatus == BASE_URL + "images/status/red-black.gif")
						changeGroupStatusIcon(parent, "erraccess");
				}
			});
    };

    var loadSocketsListeners = function() {
			wepappsocket.on("viewResponse", function(data) {
				fillLayersData(data);
				addLayersToSort();
				setSortable();
				$("#osm input").trigger("click");
				changeGroupStatusIcon("template", "working");
			});

			// When receive a new view, add in layers component
			wepappsocket.on('viewReceived', function(data){
				if(!data.private || (data.private && userLogged)) {
					var layerId = data.workspace + ":" + data.layers[0];
					var layerName = data.name;
					var parent = data.type;
					var newLayer = true;
					var layerIdToSlide;
					data.layers.forEach(function(layer){
						var _layerId = data.workspace + ":" + layer;
						if(allLayers.map(function(l){return l.id}).indexOf(_layerId) > 0) {
							newLayer = false;
							layerIdToSlide = _layerId.split('.').join('\\.');
							layerId = _layerId;
						}
					});
					if(!newLayer) {
						changeLayerStatusIcon(layerIdToSlide, "new");
						changeGroupStatusIcon(parent, "new");
						var workspace = data.workspace;
						var uriGeoServer = data.uriGeoserver;
						var serverType = data.serverType;
						if(data.params.mask) {
							var layerWithDate = Utils.getLayersListWithDate(data.params.mask, data.layers);
							setTemporarySlider(layerWithDate, layerId, layerName, workspace, uriGeoServer + '/ows', serverType, parent, 0);
						} else {
							var url = uriGeoServer + '/' + workspace + '/' + data.layers[0] + '/wms?service=WMS&version=1.1.0&request=GetCapabilities';
							var getCapabilitiesUrl = {
								layerName: data.layers[0],
								layerId: layerId,
								parent: data.type,
								url: url,
								format: 'xml',
								update: true
							}
							socket.emit('proxyRequestCapabilities', getCapabilitiesUrl);
						}
						return;
					}
					fillLayersData([data]);
					addLayerToSort(layerId, layerName);
					changeLayerStatusIcon(layerId, "new");
					changeGroupStatusIcon(parent, "new");
				}
			});

			wepappsocket.on("notifyView", function(data) {
				var layerId = data.workspace + ":" + data.layer.name;
				changeLayerStatusIcon(layerId, "alert");
				changeGroupStatusIcon("alert", "alert");
				console.log(data);
			});

			// Checking map server connection response
			socket.on('connectionResponse', function(data) {
				if(data.url) {
					//getting element to disable if there are no connection with mapr server
					var listElement = $("li[data-layerid='"+ data.requestId +"']");
					var inputElement = listElement.find('input');
					var parent = listElement.attr('data-parentid')

					//if not connected disabled the layer selection
					if(!data.connected) {
						listElement.prop("title", "Map Server is not responding");
						if(inputElement.is(':checked'))
							inputElement.trigger("click");

						if(!inputElement.hasClass("disabled-content"))
							inputElement.addClass("disabled-content");

						changeLayerStatusIcon(data.requestId.split('.').join('\\.'), "erraccess");
						changeGroupStatusIcon(parent, "erraccess");
					} else {
						listElement.removeProp("title");
						if (inputElement.hasClass("disabled-content"))
							inputElement.removeClass("disabled-content");
						
						var imageElement = listElement.find("#image-icon");
						var lastStatus = imageElement.attr("src");
						if(lastStatus == BASE_URL + "images/status/gray_icon.svg") {
							changeLayerStatusIcon(data.requestId.split('.').join('\\.'), "working");
							changeGroupStatusIcon(parent, "working");
						}
					}
				}
			});

			socket.on('proxyResponseCapabilities', function(data) {
				try {
					var layerCapabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
					var layerIndex = layerCapabilities.map(function (c){return c.name}).indexOf(data.layerName);

					if(layerIndex < 0)
						return;

					if(!layerCapabilities[layerIndex].extent)
						return;

					var span = "";
					var listElement = $("li[data-layerid='"+ data.parent +"']");
					var li = $(listElement).find("li[data-layerid='" + data.layerId +"']");

					if(li.length === 0)
						return;

					if(data.update) {
						Calendar.updateDatePicker(layerCapabilities[layerIndex], data.layerId);
					} else {
						var sliderDiv = "<div class='slider-content' style='display:none;'><label></label><button type='button' class='close close-slider'>×</button><div id='slider" + $(li).attr("data-layerid").replace(':','') + "'></div></div>";
						$(li).append(sliderDiv);

						if(layerCapabilities[layerIndex].extent instanceof Array) {
							sliderCapabilities.push(layerCapabilities[layerIndex]);
							span += "<span id='terrama2-slider' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(layerCapabilities[layerIndex]) + "<i class='fa fa-sliders'></i></span>";
						} else if (layerCapabilities[layerIndex].extent instanceof Object) {
							span += "<span id='terrama2-calendar' class='terrama2-datepicker-icon'>" + Calendar.makeHelperDatePicker(layerCapabilities[layerIndex]) + "<i class='fa fa-calendar'></i></span>";
						}

						$(li).append($(span));
					}
				} catch(e) {
					return;
				}
			});

			socket.on('proxyResponse', function(data){
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
							position: { my: 'top', at: 'top+75' },
							open: function () {
								$('.ui-dialog-titlebar-close').css('background-image', 'url(../images/close.png)');
								$('.ui-dialog-titlebar-close').css('background-position', 'center');
								$('.ui-dialog-titlebar-close').css('background-size', '20px');
							},
							close: function () {
								$('.ui-dialog-titlebar-close').css('background-image', '');
								$('.ui-dialog-titlebar-close').css('background-position', '');
								$('.ui-dialog-titlebar-close').css('background-size', '');
							}
						});
					}
				} else {
					try {
						capabilities = Capabilities.getMapCapabilitiesLayers(data.msg);
						fillModal(capabilities);
						$('#layersModal').modal('show');
					} catch(e) {
						$("#terrama2Alert > p > strong").text('Invalid URL!');
						$("#terrama2Alert > p > span").text('Error to find capabilities.');
						$("#terrama2Alert").removeClass('hide');
					}
				}
			});
    };

		var loadLayout = function() {
			memberWindowHeight = $(window).height();
			memberReducedHeight = memberWindowHeight - $("#terrama-header").height();

			$.TerraMAMonitor = {};

			$.TerraMAMonitor.options = {
				sidebarToggleSelector: "[full_screen='true']",
			};

			$.TerraMAMonitor.pushMenu = {
				activate: function(toggleBtn){
					$(document).on('click', toggleBtn, function(e){
					e.preventDefault();

					if($("body").hasClass('full_screen')) {
						$("body").removeClass('full_screen');
						$("body").addClass('sidebar-mini'); 

						$("#content").height(memberReducedHeight + "px");
						$("#terrama2-map").height(memberReducedHeight + "px");
						$("#terrama2-map").width("auto");
					} else {
						$("body").addClass('full_screen');
						$("body").removeClass('sidebar-mini');

						$("#content").height(memberWindowHeight + "px");
						$("#terrama2-map").height(memberWindowHeight + "px");
						$("#terrama2-map").width("100%");
					}
					
					TerraMA2WebComponents.MapDisplay.updateMapSize();
					})
				}
			};

			$.TerraMAMonitor.tree = function(menu) {
				var _this = this;
				$(document).off('click', menu + ' li span').on('click', menu + ' li span', function (e) {
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

			var o = $.TerraMAMonitor.options;

			$.TerraMAMonitor.pushMenu.activate(o.sidebarToggleSelector);
			$.TerraMAMonitor.tree('.sidebar');

			$("#content").height(memberReducedHeight + "px");
			$("#terrama2-map").height(memberReducedHeight + "px");
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

			$("#legend-box").draggable({
				containment: $('#terrama2-map')
			});

			$("#table-div").resizable({
				minHeight: 400,
				handles: "n"
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
				allLayers.push({id: "custom", name: "Custom"});
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("template", "Template", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("template", "terrama2-layerexplorer", null, "treeview unsortable", null);
				allLayers.push({id: "template", name: "Template"});
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("static", "Static Data", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("static", "terrama2-layerexplorer", null, "treeview unsortable", null);
				allLayers.push({id: "static", name: "Static Data"});
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("dynamic", "Dynamic Data", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("dynamic", "terrama2-layerexplorer", null, "treeview unsortable", null);
				allLayers.push({id: "dynamic", name: "Dynamic Data"});
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("analysis", "Analysis", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("analysis", "terrama2-layerexplorer", null, "treeview unsortable", null);
				allLayers.push({id: "analysis", name: "Analysis"});
			}

			if(TerraMA2WebComponents.MapDisplay.addLayerGroup("alert", "Alert", "terrama2-layerexplorer")) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("alert", "terrama2-layerexplorer", null, "treeview unsortable", null);
				allLayers.push({id: "alert", name: "Alert"});
			}

			//Adding open map street
			if(TerraMA2WebComponents.MapDisplay.addOSMLayer("osm", "Open street", "Open street", false, "terrama2-layerexplorer", false)) {
				TerraMA2WebComponents.LayerExplorer.addLayersFromMap("osm", "template", null, "treeview unsortable terrama2-truncate-text", null);
				TerraMA2WebComponents.MapDisplay.setLayerProperty("osm", "layerType", "template");
				TerraMA2WebComponents.MapDisplay.setLayerProperty("osm", "layerName", "Open street");
				allLayers.push({id: "osm", name: "Open street"});
				addLayerStatusIcon("osm");
				changeLayerStatusIcon("osm", "working");
			}

			addTreeviewMenuClass();
			addGroupSpanIcon();

			if(webmonitorHostInfo && webmonitorHostInfo.basePath) {
				socket = io.connect(window.location.origin, {
					path: webmonitorHostInfo.basePath + 'socket.io'
				});
			} else {
				socket = io.connect(":36001");
			}

			if(webadminHostInfo && webadminHostInfo.host && webadminHostInfo.port && webadminHostInfo.basePath) {
				wepappsocket = io.connect(webadminHostInfo.host + ":" + webadminHostInfo.port, {
					path: webadminHostInfo.basePath + 'socket.io'
				});
			} else {
				wepappsocket = io.connect(":36000");
			}

			wepappsocket.emit('viewRequest');

			// Check connections every 30 seconds
			var intervalID = setInterval(function(){
				allLayers.forEach(function(layerObject){
					socket.emit('checkConnection', {url: layerObject.url, requestId: layerObject.id});
				});
			}, 30000);

			loadSocketsListeners();
			loadEvents();
			loadLayout();
    };

    return {
      init: init
    };
  }
);