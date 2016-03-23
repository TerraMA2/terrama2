module.exports = function(app) {
  var DataManager = require("../../core/DataManager.js");
  return {
    "get": function(request, response) {

      DataManager.listDataProviderType().then(function(result) {
        response.json(result);
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: "Could not retrieve data provider types from database. DataManager should be initialized before!"});
      });
    }

  };
};