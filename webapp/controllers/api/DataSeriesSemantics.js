var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var DataSeriesSemanticsError = require("../../core/Exceptions").DataSeriesSemanticsError;


module.exports = function(app) {
  return {
    "get": function(request, response) {
      var dataFormatName = request.query.dataFormat;

      DataManager.listDataSeriesSemantics(dataFormatName ? {data_format_name: dataFormatName} : {}).then(function(semanticsList) {
        response.json(semanticsList);
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      })
      
    }
  };
};