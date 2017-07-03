"use strict";

/**
 * Controller responsible for returning the attributes table data accordingly with the received parameters.
 * @class GetAttributesTableController
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberAttributesTable - 'AttributesTable' model.
 */
var GetAttributesTableController = function(app) {

  var memberHttp = require('http');

  var describeFeatureTypeTemplateURL = "/wms?service=WFS&version=1.0.0&request=DescribeFeatureType&outputFormat=application/json&typename={{LAYER_NAME}}";
  var getFeatureTemplateURL = "/wfs?service=wfs&version=2.0.0&request=GetFeature&outputFormat=application/json&typeNames={{LAYER_NAME}}&propertyName={{PROPERTIES}}&sortBy={{SORT}}&startIndex={{START_INDEX}}&count={{COUNT}}";
  var getLegendGraphicTemplateURL = "/wms?REQUEST=GetLegendGraphic&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&legend_options=forceLabels:on&LAYER={{LAYER_NAME}}";

  var getValidProperties = function(layer, geoserverUri, callback) {
    memberHttp.get(geoserverUri + describeFeatureTypeTemplateURL.replace('{{LAYER_NAME}}', layer), function(resp) {
      var body = '';
      var fields = [];

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);

          for(var i = 0, propertiesLength = body.featureTypes[0].properties.length; i < propertiesLength; i++) {
            var type = body.featureTypes[0].properties[i].type.split(':');

            if(type[0] !== "gml")
              fields.push({ name: body.featureTypes[0].properties[i].name, string: (body.featureTypes[0].properties[i].localType === "string" ? true : false) });
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
   * @function getAttributesTableController
   * @memberof GetAttributesTableController
   * @inner
   */
  var getAttributesTableController = function(request, response) {
    getValidProperties(request.body.layer, request.body.geoserverUri, function(fields) {
      var properties = "";
      var search = (request.body['search[value]'] !== "" ? "&cql_filter=" : "");

      for(var i = 0, fieldsLength = fields.length; i < fieldsLength; i++) {
        properties += fields[i].name + ",";

        if(request.body['search[value]'] !== "" && fields[i].string)
          search += "strToLowerCase(Concatenate(" + fields[i].name + ", '')) like '%25" + request.body['search[value]'].toLowerCase() + "%25' or ";
      }

      var order = fields[request.body['order[0][column]']].name + (request.body['order[0][dir]'] === "desc" ? "+D" : "+A");

      properties = (properties !== "" ? properties.substring(0, properties.length - 1) : properties);

      var url = request.body.geoserverUri + getFeatureTemplateURL.replace('{{LAYER_NAME}}', request.body.layer);
      url = url.replace('{{PROPERTIES}}', properties);
      url = url.replace('{{SORT}}', order);
      url = url.replace('{{START_INDEX}}', request.body.start);
      url = url.replace('{{COUNT}}', request.body.length);

      if(search !== "")
        url += search.substring(0, search.length - 4);

      memberHttp.get(url, function(resp) {
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

            // Conversion of the result object to array
            body.features.forEach(function(val) {
              var temp = [];
              for(var key in val.properties) temp.push(val.properties[key]);
              data.push(temp);
            });
          } catch(ex) {
            body = {};
          }

          // JSON response
          response.json({
            draw: parseInt(request.body.draw),
            recordsTotal: parseInt(request.body.start) + parseInt(body.totalFeatures),
            recordsFiltered: parseInt(request.body.start) + parseInt(body.totalFeatures),
            data: data
          });
        });
      }).on("error", function(e) {
        console.error(e.message);
        callback([]);
      });
    });
  };

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

  var getLegend = function(request, response) {
    memberHttp.get(request.query.geoserverUri + getLegendGraphicTemplateURL.replace('{{LAYER_NAME}}', request.query.layer), function(resp) {
      resp.pipe(response, {
        end: true
      });
    }).on("error", function(e) {
      console.error(e.message);
    });
  };

  return {
    getAttributesTableController: getAttributesTableController,
    getColumns: getColumns,
    getLegend: getLegend
  };
};

module.exports = GetAttributesTableController;
