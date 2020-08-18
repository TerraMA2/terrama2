"use strict";

/**
 * Controller responsible for returning the attributes table data accordingly with the received parameters.
 * @class GetAttributesTableController
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberHttp - 'http' module.
 * @property {string} memberDescribeFeatureTypeTemplateURL - Url template for the GeoServer 'DescribeFeatureType' request.
 * @property {string} memberGetFeatureTemplateURL - Url template for the GeoServer 'GetFeature' request.
 * @property {string} memberGetLegendGraphicTemplateURL - Url template for the GeoServer 'GetLegendGraphic' request.
 * @property {object} memberCurrentLayer - Object responsible for keeping the current layer data.
 */
var GetAttributesTableController = function(app) {

  // 'http' module
  var memberHttp = require('http');
  const common = require('./../utils/common');
  // Url template for the GeoServer 'DescribeFeatureType' request
  var memberDescribeFeatureTypeTemplateURL = "/wms?service=WFS&version=1.0.0&request=DescribeFeatureType&outputFormat=application/json&typename={{LAYER_NAME}}";
  // Url template for the GeoServer 'GetFeature' request
  var memberGetFeatureTemplateURL = "/wfs?service=wfs&version=2.0.0&request=GetFeature&outputFormat=application/json&typeNames={{LAYER_NAME}}&propertyName={{PROPERTIES}}&sortBy={{SORT}}&startIndex={{START_INDEX}}&count={{COUNT}}";
  // Url template for the GeoServer 'GetLegendGraphic' request
  var memberGetLegendGraphicTemplateURL = "/wms?REQUEST=GetLegendGraphic&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&legend_options=forceLabels:on&LAYER={{LAYER_NAME}}&STYLE={{STYLE_NAME}}";
  // Object responsible for keeping the current layer data
  var memberCurrentLayer = {
    id: null,
    numberOfFeatures: 0,
    search: "",
    timeStart: null,
    timeEnd: null,
    analysisTime: null
  };

  /**
   * Returns the valid properties (non geometric properties) of a given layer.
   * @param {string} layer - Layer id
   * @param {string} geoserverUri - GeoServer base url
   * @param {function} callback - Callback function to be executed
   *
   * @private
   * @function getValidProperties
   * @memberof GetAttributesTableController
   * @inner
   */
  var getValidProperties = function(layer, geoserverUri, callback) {

    common.getHttpHandler(geoserverUri).get(geoserverUri + memberDescribeFeatureTypeTemplateURL.replace('{{LAYER_NAME}}', layer), function(resp) {

      var body = '';
      var fields = [];

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);

          for(var i = 0, propertiesLength = body.featureTypes[0].properties.length; i < propertiesLength; i++) {
            fields.push({
              name: body.featureTypes[0].properties[i].name,
              string: (body.featureTypes[0].properties[i].localType === "string" ? true : false),
              dateTime: (body.featureTypes[0].properties[i].localType === "date-time" ? true : false),
              date: (body.featureTypes[0].properties[i].localType === "date" ? true : false)
            });
          }

        } catch(ex) {
          body = {};
        }
        callback(fields);
      });
    }).on("error", function(e) {
      console.error(e.message);
      callback([]);
    });
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function getAttributesTable
   * @memberof GetAttributesTableController
   * @inner
   */
  var getAttributesTable = function(request, response) {
    getValidProperties(request.body.layer, request.body.geoserverUri, function(fields) {
      var properties = "";
      var search = (request.body['search[value]'] !== "" ? "&cql_filter=(" : "");
      var dateTimeField = null;
      var dateField = null;
      var columnsFilter = request.body['columnsFilter[]'];
      var layerType = request.body['layerData[parent]'];

      for(var i = 0, fieldsLength = fields.length; i < fieldsLength; i++) {
        properties += fields[i].name + ",";

        if(fields[i].dateTime) dateTimeField = fields[i].name;
        if(fields[i].date) dateField = fields[i].name;

        if(request.body['search[value]'] !== "" && fields[i].string)
          search += "strToLowerCase(Concatenate(" + fields[i].name + ", '')) like '%25" + request.body['search[value]'].toLowerCase() + "%25' or ";
      }

      var order = fields[request.body['order[0][column]']].name + (request.body['order[0][dir]'] === "desc" ? "+D" : "+A");

      properties = (properties !== "" ? properties.substring(0, properties.length - 1) : properties);

      if(typeof columnsFilter !== 'undefined' && columnsFilter.length > 0){
        properties = "";
        for(var i = 0; i < columnsFilter.length; i++) {
          properties += columnsFilter[i];
          if(i+1 < columnsFilter.length){
            properties += ",";
          }
        }
      }

      var url = request.body.geoserverUri + memberGetFeatureTemplateURL.replace('{{LAYER_NAME}}', request.body.layer);
      url = url.replace('{{PROPERTIES}}', properties);
      url = url.replace('{{SORT}}', order);
      url = url.replace('{{START_INDEX}}', request.body.start);
      url = url.replace('{{COUNT}}', request.body.length);

      if(search !== "") {
        search = search.substring(0, search.length - 4) + ")";

        if(request.body.timeStart && request.body.timeEnd && (dateTimeField || dateField)) {
          search += " and (" + (dateTimeField !== null ? dateTimeField : dateField) + " between '" + request.body.timeStart + "' and '" + request.body.timeEnd + "')";
        }

        url += search;
      } else if(request.body.timeStart && request.body.timeEnd && (dateTimeField || dateField)) {
        url += "&cql_filter=(" + (dateTimeField !== null ? dateTimeField : dateField) + " between '" + request.body.timeStart + "' and '" + request.body.timeEnd + "')";
      } else if(request.body.analysisTime) {
        url += "&cql_filter=(execution_date='" + request.body.analysisTime + "')";
      }

      common.getHttpHandler(url).get(url, function(resp) {
        var body = '';
        var fields = [];

        resp.on('data', function(chunk) {
          body += chunk;
        });

        resp.on('end', function() {
          // Array responsible for keeping the data obtained by the method 'getAttributesTableData'
          var data = [];

          try {
            body = JSON.parse(body);

            let columnsFiltered = properties.split(",");

            // Conversion of the result object to array
            body.features.forEach(function(val) {
              var temp = [];

              if((columnsFiltered.length !== Object.keys(val.properties).length) || (columnsFiltered.length == Object.keys(val.properties).length && verifyEqualsColumns(columnsFiltered, val))){
                columnsFiltered.forEach(function(columnFilter){
                  if(Object.keys(val.properties).includes(columnFilter)){
                    if(val.properties[columnFilter] != null && typeof val.properties[columnFilter] == 'object'){
                      if(typeof val.properties[columnFilter].type !== 'undefined'){
                        temp.push(val.properties[columnFilter].type);
                      }else{
                        let propertiesObj = val.properties[columnFilter];
                        for(let key in propertiesObj){
                          temp.push(propertiesObj[key]);
                          break;
                        }
                      }
                    } else{
                      temp.push(val.properties[columnFilter]);
                    }
                  } else{
                    if(layerType == "static" && val.geometry !== null){
                      temp.push(val.geometry.type);
                    }else{
                      if(typeof val.geometry_name !== 'undefined' && val.geometry_name !== null){
                        temp.push(val.geometry.type);
                      } else{
                        temp.push("");
                      }
                    }
                  }
                });
              } else{
                for(var key in val.properties){
                  temp.push(val.properties[key]);
                }

                if(layerType == "static" && val.geometry != null){
                  temp.push(val.geometry.type);
                }
              }

              data.push(temp);
            });

          } catch(ex) {
            console.log(ex);
            body = {};
          }

          if(
            memberCurrentLayer.id !== request.body.layer ||
            memberCurrentLayer.search !== request.body['search[value]'] ||
            parseInt(memberCurrentLayer.numberOfFeatures) < parseInt(body.totalFeatures) ||
            memberCurrentLayer.timeStart !== request.body.timeStart ||
            memberCurrentLayer.timeEnd !== request.body.timeEnd ||
            memberCurrentLayer.analysisTime !== request.body.analysisTime
          ) {
            memberCurrentLayer.id = request.body.layer;
            memberCurrentLayer.numberOfFeatures = body.totalFeatures;
            memberCurrentLayer.search = request.body['search[value]'];
            memberCurrentLayer.timeStart = request.body.timeStart;
            memberCurrentLayer.timeEnd = request.body.timeEnd;
            memberCurrentLayer.analysisTime = request.body.analysisTime;
          }

          // JSON response
          response.json({
            draw: parseInt(request.body.draw),
            recordsTotal: parseInt(memberCurrentLayer.numberOfFeatures),
            recordsFiltered: parseInt(memberCurrentLayer.numberOfFeatures),
            data: data
          });

        });
      }).on("error", function(e) {
        console.error(e.message);
        callback([]);
      });
    });
  };

  var verifyEqualsColumns = function(filteredByGeoserver, object){
    Object.keys(object.properties).forEach(function(property){
      if(!filteredByGeoserver.includes(property)){
        return false;
      }
    });
    return true;
  }

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function getColumns
   * @memberof GetAttributesTableController
   * @inner
   */
  var getColumns = function(request, response) {
    if(request.query.layer === undefined || request.query.layer === null || request.query.layer === "") {
      response.json({
        fields: []
      });
    } else {
      getValidProperties(request.query.layer, request.query.geoserverUri, function(fields) {
        response.json({
          fields: fields
        });
      });
    }
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function getLegend
   * @memberof GetAttributesTableController
   * @inner
   */
  var getLegend = function(request, response) {
    const uri = request.query.geoserverUri;

    common.getHttpHandler(uri).get(uri + memberGetLegendGraphicTemplateURL.replace('{{LAYER_NAME}}', request.query.layer).replace('{{STYLE_NAME}}', request.query.layer+'_style'), function(resp) {
      if(resp.headers['content-type'].startsWith('image')) {
        resp.pipe(response, {
          end: true
        });
      } else {
        common.getHttpHandler(uri).get(request.query.geoserverUri + memberGetLegendGraphicTemplateURL.replace('{{LAYER_NAME}}', request.query.layer).replace('{{STYLE_NAME}}', request.query.layer+'_style_legend'), function(resp) {
          if(resp.headers['content-type'].startsWith('image')) {
            resp.pipe(response, {
              end: true
            });
          } else {
            console.error('Error retriving legend.');
          }
        }).on("error", function(e) {
          console.error(e.message);
        });
      }
    }).on("error", function(e) {
      console.error(e.message);
    });
  };

  return {
    getAttributesTable: getAttributesTable,
    getColumns: getColumns,
    getLegend: getLegend
  };
};

module.exports = GetAttributesTableController;
