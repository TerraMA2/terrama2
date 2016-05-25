var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var DataSeriesSemanticsError = require("../../core/Exceptions").DataSeriesSemanticsError;
var DataSeriesSemanticsFactory = require('./../../core/data-series-semantics/Factory');

function makeMetadata(identifier) {
  var semanticsStructure = DataSeriesSemanticsFactory.getDataSeriesSemantics(identifier);

  return {
    form: semanticsStructure.form,
    schema: semanticsStructure.schema
  };
}

module.exports = function(app) {
  return {
    "get": function(request, response) {
      var semanticsName = request.params.name;
      var metadata = request.query.metadata == 'true';

      // todo: validate it from database
      // get just one semantics
      if (semanticsName) {

        try {

          var semanticsStructure;
          if (metadata) {
            semanticsStructure = makeMetadata(semanticsName);
          }

          DataManager.getDataSeriesSemantics({code: semanticsName}).then(function(semantics) {
            var output = semantics;

            if (metadata)
              output.metadata = semanticsStructure;

            return response.json(output);
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          })
        } catch (err) {
          return Utils.handleRequestError(response, new DataSeriesSemanticsError(semanticsName + " is not available"), 400);
        }

      } else {
        // todo: semantics structure for each semantic in database
        DataManager.listDataSeriesSemantics().then(function(semanticsList) {
          return response.json(semanticsList);
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        })
      }
    }
  };
};