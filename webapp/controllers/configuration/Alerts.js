module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

  var controllers = {
    get: function(request, response){
        var parameters = makeTokenParameters(request.query.token, app);
        response.render("configuration/alerts", parameters);
    },
    new: function(request, response){
        response.render("configuration/alert");
    }
  }
  return controllers;
};