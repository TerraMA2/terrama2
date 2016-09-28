module.exports = function(app) {
  'use strict';

  return {
    get: function(request, response) {
      response.render("configuration/views");
    },
    new: function(request, response) {
      return response.render("configuration/view");
    },
    edit: function(request, response) {
      return response.render("configuration/view");
    }
  };
};