var logger = require("./../../core/Logger");
var RequestFactory = require("../../core/RequestFactory");

module.exports = function(app) {
  var DataManager = require("../../core/DataManager.js");
  return {
    "get": function(request, response) {

      DataManager.listDataProviderType().then(function(result) {
        var output = [];
        RequestFactory.listAll().forEach(function(requestFields) {
          // todo: Its temp code. improve it
          result.forEach(function(dataProviderType) {
            if (dataProviderType.name === requestFields.name)
              output.push(requestFields);
          });
        });

        response.json(output);
      }).catch(function(err) {
        logger.error(err);
        response.status(400);
        response.json({status: 400, message: "Could not retrieve data provider types from database. DataManager should be initialized before!"});
      });
    }

  };
};