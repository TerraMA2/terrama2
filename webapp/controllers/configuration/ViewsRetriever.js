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
    var sendPrivate = false;

    var processViews = function() {
      var objectsToSend = [];

      if(params.type === memberViewsCache.TYPES.NEW_AND_UPDATED && (params.initialRequest || params.onlyPrivate)) {
        return memberDataManager.listRegisteredViews().then(function(views) {
          var viewsObjects = views.map(function(view) {
            return view.toObject();
          });

          for(var i = 0, viewsLength = viewsObjects.length; i < viewsLength; i++) {
            if((viewsObjects[i].private && sendPrivate) || (!params.onlyPrivate && !viewsObjects[i].private))
              objectsToSend.push(viewsObjects[i]);
          }

          response.json({
            views: objectsToSend,
            projects: memberDataManager.listProjects(),
            initialRequest: params.initialRequest
          });
        }).catch(function(err) {
          console.error(err);
          response.json({});
        });
      } else {
        var viewsObjects = memberViewsCache.getViews(params.type, params.clientId);

        for(var i = 0, viewsLength = viewsObjects.length; i < viewsLength; i++) {
          if((viewsObjects[i].private && sendPrivate) || !viewsObjects[i].private)
            objectsToSend.push(viewsObjects[i]);
        }

        response.json({ views: objectsToSend, initialRequest: false });
        memberViewsCache.emptyViews(params.type, params.clientId);
      }
    };

    if(params.userToken !== null && params.userToken !== "") {
      memberDataManager.getUser({ token: params.userToken }).then(function(user) {
        if(user.token === params.userToken)
          sendPrivate = true;

        processViews();
      }).catch(function(err) {
        processViews();
      });
    } else {
      processViews();
    }
  };

  var retrieveViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.NEW_AND_UPDATED,
      clientId: request.body.clientId,
      userToken: request.body.userToken,
      initialRequest: request.body.initialRequest,
      onlyPrivate: request.body.onlyPrivate
    }, response);
  };

  var retrieveRemovedViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.REMOVED,
      clientId: request.body.clientId,
      userToken: request.body.userToken
    }, response);
  };

  var retrieveNotifiedViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.NOTIFIED,
      clientId: request.body.clientId,
      userToken: request.body.userToken
    }, response);
  };

  return {
    retrieveViews: retrieveViews,
    retrieveRemovedViews: retrieveRemovedViews,
    retrieveNotifiedViews: retrieveNotifiedViews
  };
};

module.exports = ViewsRetriever;