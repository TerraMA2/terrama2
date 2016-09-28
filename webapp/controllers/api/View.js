'use strict';

// Dependencies
var DataManager = require("./../../core/DataManager");
var handleRequestError = require("./../../core/Utils").handleRequestError;

/**
 * Injecting NodeJS App configuration as dependency. It retrieves a Views controllers API
 * 
 * @param {Object}
 * @returns {Object}
 */
module.exports = function(app) {
  return {
    get: function(request, response) {
      var viewId = request.params.id;
      if (viewId) {
        // get single view: TODO
      }

      DataManager.listViews({})
        .then(function(views) {
          return response.json(views.map(function(view) {
            return view.toObject();
          }))
        })

        .catch(function(err) {
          return handleRequestError(response, err, 400);
        });
    },

    post: function(request, response) {
      var viewObject = request.body;

      DataManager.orm.transaction(function(t) {
        return DataManager.addView(viewObject, {transaction: t});
      })
      .then(function(view) {
        // TODO: send via service
        return response.json(view);
      })
      
      .catch(function(err){
        return handleRequestError(response, err, 400);
      });
    }
  }
};