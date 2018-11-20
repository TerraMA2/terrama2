var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var DataSeriesSemanticsError = require("../../core/Exceptions").DataSeriesSemanticsError;
var DataSeriesSemanticsFactory = require('./../../core/data-series-semantics/Factory');
var TemporalityType = require('./../../core/Enums').TemporalityType;
var Promise = require("./../../core/Promise");
var logger = require("./../../core/Logger");

function makeMetadata(identifier, metadata) {
  try {
    var semanticsStructure = DataSeriesSemanticsFactory.build({code: identifier});
    var GUIValues = semanticsStructure.gui;
    var semanticsObject = {
      name: semanticsStructure.name,
      driver: semanticsStructure.driver,
      temporality: semanticsStructure.temporality,
      allow_storage: semanticsStructure.allow_storage || false,
      allow_direct_access: semanticsStructure.allow_direct_access,
      custom_format: semanticsStructure.custom_format || false
    };

    if (metadata){
      semanticsObject.metadata = {
        form: GUIValues.form,
        schema: GUIValues.schema,
        demand: semanticsStructure.providers_type_list,
        metadata: semanticsStructure.metadata || {}
      }
    }

    return semanticsObject;
  } catch (err) {
    logger.error(Utils.format("No data series semantics match. %s - %s", identifier, err.toString()));
    return null;
  }
}

module.exports = function(app) {
  return {
    get: function(request, response) {
      var semanticsName = request.params.name;
      var metadata = request.query.metadata == 'true';
      var semanticsType = request.query.type;

      var queryParams = {};
      var filterByTemporality = semanticsType ? true : false;

      // get just one semantics
      if (semanticsName) {
        queryParams.code = semanticsName;

        try {

          var semanticsStructure = makeMetadata(semanticsName, metadata);

          DataManager.getDataSeriesSemantics(queryParams).then(function(semantics) {
            var output = semantics;
            Object.assign(output, semanticsStructure);

            return response.json(output);
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          });
        } catch (err) {
          logger.error(err);
          return Utils.handleRequestError(response, new DataSeriesSemanticsError(semanticsName + " is not available"), 400);
        }

      } else {
        // todo: semantics structure for each semantic in database
        DataManager.listDataSeriesSemantics(queryParams).then(function(semanticsList) {
          var dataSeriesSemanticsList = [];
          semanticsList.forEach(function(semantics) {
            var semanticsObject = makeMetadata(semantics.code, metadata);
            Object.assign(semantics, semanticsObject);
            if (!semanticsObject) {
              logger.warn(`Semantic ${semantics.code} not found. Skipping`);
              return;
            }

            if (filterByTemporality && semanticsObject.temporality.toLowerCase() != semanticsType.toLowerCase())
              return;
            else
              dataSeriesSemanticsList.push(semantics);
          });

          return response.json(dataSeriesSemanticsList);
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }
    }
  };
};
