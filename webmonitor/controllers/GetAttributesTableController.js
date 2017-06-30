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

  var describeFeatureTypeTemplateURL = "http://localhost:8080/geoserver/wms?service=WFS&version=1.0.0&request=DescribeFeatureType&outputFormat=application/json&typename={{LAYER_NAME}}";
  var getFeatureTemplateURL = "http://localhost:8080/geoserver/wfs?service=wfs&version=2.0.0&request=GetFeature&outputFormat=application/json&typeNames={{LAYER_NAME}}&propertyName={{PROPERTIES}}&sortBy={{SORT}}+A&startIndex={{START_INDEX}}&count={{COUNT}}";

  var getValidProperties = function(layer, callback) {
    memberHttp.get(describeFeatureTypeTemplateURL.replace('{{LAYER_NAME}}', layer), function(resp) {
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
              fields.push(body.featureTypes[0].properties[i].name);
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
    getValidProperties(request.body.layer, function(fields) {
      var properties = "";

      for(var i = 0, fieldsLength = fields.length; i < fieldsLength; i++) {
        properties += fields[i] + ",";
      }
    
      properties = (properties !== "" ? properties.substring(0, properties.length - 1) : properties);

      var url = getFeatureTemplateURL.replace('{{LAYER_NAME}}', request.body.layer);
      url = url.replace('{{PROPERTIES}}', properties);
      url = url.replace('{{SORT}}', fields[0]);
      url = url.replace('{{START_INDEX}}', request.body.start);
      url = url.replace('{{COUNT}}', request.body.length);

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
      getValidProperties(request.query.layer, function(fields) {
        response.json({
          fields: fields
        });
      });
    }
  };

  return {
    getAttributesTableController: getAttributesTableController,
    getColumns: getColumns
  };
};

module.exports = GetAttributesTableController;
