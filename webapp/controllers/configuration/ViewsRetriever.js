"use strict";

/**
 * Controller responsible for retrieving cached views data.
 * @class ViewsRetriever
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberViewsCache - 'ViewsCache' class.
 * @property {object} memberDataManager - 'DataManager' class.
 * @property {object} memberTcpService - 'TcpService' class.
 */
var ViewsRetriever = function(app) {

  // 'ViewsCache' class
  var memberViewsCache = require("../../core/ViewsCache");
  // 'DataManager' class
  var memberDataManager = require("../../core/DataManager");
  // 'TcpService' class
  var memberTcpService = require('./../../core/facade/tcp-manager/TcpService');

  var retrieveFunction = function(params, response) {
    var sendPrivate = false;

    var processViews = function() {
      var objectsToSend = [];

      if(params.type === memberViewsCache.TYPES.NEW_AND_UPDATED && (params.initialRequest || params.onlyPrivate)) {
        return memberDataManager.listRegisteredViews().then(function(views) {
          return memberDataManager.listAnalysis({}).then(function(analysisList) {
            return memberDataManager.listAlerts().then(function(alerts) {
              var viewsObjects = views.map(function(view) {
                var description = null;

                if(view.dataSeries) {
                  if(view.dataSeriesType === "alert") {
                    alerts.map(function(alert) {
                      if(alert.view.id === view.view.id) {
                        description = alert.description;
                        return;
                      }
                    });
                  }
                }

                var viewObject = view.toObject();
                if(description) {
                  viewObject.description = description;
                }

                return viewObject;
              });

              for(var i = 0, viewsLength = viewsObjects.length; i < viewsLength; i++) {
                if((viewsObjects[i].private && sendPrivate) || (!params.onlyPrivate && !viewsObjects[i].private))
                  objectsToSend.push(viewsObjects[i]);
              }

              var returnData = {
                views: objectsToSend,
                initialRequest: params.initialRequest
              };

              if(params.initialRequest)
                returnData.projects = memberDataManager.listProjects();

              response.json(returnData);
            }).catch(function(err) {
              console.error(err);
              response.json({});
            });
          }).catch(function(err) {
            console.error(err);
            response.json({});
          });
        }).catch(function(err) {
          console.error(err);
          response.json({});
        });
      } else {
        var viewsObjects = memberViewsCache.getViews(params.type, params.clientId);

        for(var i = 0, viewsLength = viewsObjects.length; i < viewsLength; i++) {
          var viewSetPrivate = false;

          if(params.views) {
            var layerId = (viewsObjects[i].workspace ? viewsObjects[i].workspace + ":" + viewsObjects[i].layer : viewsObjects[i].layer);

            if((!params.views[layerId] || params.views[layerId] == "false") && viewsObjects[i].private)
              viewSetPrivate = true;
          }

          if((viewsObjects[i].private && sendPrivate) || !viewsObjects[i].private || (params.type === memberViewsCache.TYPES.REMOVED && viewSetPrivate))
            objectsToSend.push(viewsObjects[i]);
          else if(viewSetPrivate)
            memberTcpService.emitEvent("removeView", viewsObjects[i]);
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
      onlyPrivate: request.body.onlyPrivate,
      views: request.body.views
    }, response);
  };

  var retrieveRemovedViews = function(request, response) {
    retrieveFunction({
      type: memberViewsCache.TYPES.REMOVED,
      clientId: request.body.clientId,
      userToken: request.body.userToken,
      views: request.body.views
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
