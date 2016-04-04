var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var DataSeriesSemanticsError = require("../../core/Exceptions").DataSeriesSemanticsError;


module.exports = function(app) {
  return {
    "get": function(request, response) {
      DataManager.listDataFormats().then(function(dataFormats) {
        response.json(dataFormats);
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      })
      
    }
  };
};