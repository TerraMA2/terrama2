"use strict";

var DataManager = require('./../../core/DataManager');

module.exports = function(app) {
  return function(request, response) {
    return DataManager.isSRIDValid(request.params.srid).then(function() {
      response.json({ error: null });
    }).catch(function(err) {
      response.json({ error: err });
    });
  };
};
