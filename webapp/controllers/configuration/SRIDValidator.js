"use strict";

var DataManager = require('./../../core/DataManager');

module.exports = function(app) {
   var SRIDValidatorController = function(request, response) {
    return DataManager.isSRIDValid(request.params.srid).then(function() {
      response.json({ error: null });
    }).catch(function(err) {
      response.json({ error: err });
    });
  };

  var getSRIDsController = function(request, response) {
    return DataManager.getSRIDs().then(function(result) {
      response.json({ srids: result });
    });
  };

  return {
    SRIDValidatorController: SRIDValidatorController,
    getSRIDsController: getSRIDsController
  };
};
