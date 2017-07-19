"use strict";

/**
 * Controller responsible for retrieving cached views data.
 * @class ViewsRetriever
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberViewsCache - 'ViewsCache' class.
 * @property {object} memberDataManager - 'DataManager' class.
 */
var ViewsRetriever = function(app) {

  // 'ViewsCache' class
  var memberViewsCache = require("../../core/ViewsCache");
  // 'DataManager' class
  var memberDataManager = require("../../core/DataManager");

  var retrieveFunction = function(params, response) {
    if(params.type === memberViewsCache.TYPES.NEW_AND_UPDATED && params.initialRequest) {
      return memberDataManager.listRegisteredViews().then(function(views) {
        var viewsObjects = views.map(function(view) {
          return view.toObject();
        });

        response.json({
          views: viewsObjects,
          projects: memberDataManager.listProjects(),
          initialRequest: true
        });
      }).catch(function(err) {
        console.error(err);
        response.json({});
      });
    } else {
      response.json({ views: memberViewsCache.getViews(params.type, params.clientId), initialRequest: false });
      memberViewsCache.emptyViews(params.type, params.clientId);
    }
  };

  var retrieveViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.NEW_AND_UPDATED,
      clientId: request.body.clientId,
      initialRequest: request.body.initialRequest
    }, response);
  };

  var retrieveRemovedViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.REMOVED,
      clientId: request.body.clientId
    }, response);
  };

  var retrieveNotifiedViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.NOTIFIED,
      clientId: request.body.clientId
    }, response);
  };

  return {
    retrieveViews: retrieveViews,
    retrieveRemovedViews: retrieveRemovedViews,
    retrieveNotifiedViews: retrieveNotifiedViews
  };
};

module.exports = ViewsRetriever;