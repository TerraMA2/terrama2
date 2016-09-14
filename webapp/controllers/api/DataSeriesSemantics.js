var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var DataSeriesSemanticsError = require("../../core/Exceptions").DataSeriesSemanticsError;
var DataSeriesSemanticsFactory = require('./../../core/data-series-semantics/Factory');
var DataSeriesType = require('./../../core/Enums').DataSeriesType;

function makeMetadata(identifier) {
  var semanticsStructure = DataSeriesSemanticsFactory.build({code: identifier});

  return {
    form: semanticsStructure.form(),
    schema: semanticsStructure.schema(),
    demand: semanticsStructure.demand(),
    metadata: semanticsStructure.metadata()
  };
}

module.exports = function(app) {
  return {
    get: function(request, response) {
      var semanticsName = request.params.name;
      var metadata = request.query.metadata == 'true';
      var semanticsType = request.query.type;

      var queryParams = {};

      if (semanticsType) {
        if (semanticsType.toLowerCase() == "static")
          queryParams["data_series_type_name"] = DataSeriesType.STATIC_DATA;
        else {
          queryParams["$and"] = {
            $or: Utils.listDynamicDataSeriesType()
          }
        }
      }

      // get just one semantics
      if (semanticsName) {
        queryParams.code = semanticsName;

        try {

          var semanticsStructure;
          if (metadata) {
            semanticsStructure = makeMetadata(semanticsName);
          }

          DataManager.getDataSeriesSemantics(queryParams).then(function(semantics) {
            DataManager.listSemanticsProvidersType({data_series_semantics_id: semantics.id}).then(function(semanticsProvider) {
              var output = semantics;
              output.data_providers_semantics = semanticsProvider;

              if (metadata)
                output.metadata = semanticsStructure;

              return response.json(output);
            }).catch(function(err) {
               return Utils.handleRequestError(response, err, 400);
            });
          }).catch(function(err) {
            return Utils.handleRequestError(response, err, 400);
          });
        } catch (err) {
          return Utils.handleRequestError(response, new DataSeriesSemanticsError(semanticsName + " is not available"), 400);
        }

      } else {
        // todo: semantics structure for each semantic in database
        DataManager.listDataSeriesSemantics(queryParams).then(function(semanticsList) {
          if (metadata) {
            semanticsList.forEach(function(semantics) {
              semantics.metadata = makeMetadata(semantics.code);
            });
          }

          return response.json(semanticsList);
        }).catch(function(err) {
          return Utils.handleRequestError(response, err, 400);
        });
      }
    }
  };
};
