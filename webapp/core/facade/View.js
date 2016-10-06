(function() {
  'use strict';

  var DataManager = require("./../DataManager");
  var TcpManager = require("./../TcpManager");
  var Enums = require("./../Enums");
  var Utils = require("./../Utils");
  var PromiseClass = require("bluebird");
  /**
   * @type {RequestFactory}
   */
  var RequestFactory = require("./../RequestFactory");

  /**
   * It represents a mock to handle view.
   * It is used in View API
   * 
   * @class View
   */
  var View = module.exports = {};

  /**
   * Helper to send views via TCP
   * 
   * @param {Array|Object} args A view values to send
   */
  function sendView(args) {
    var objToSend = {
      "Views": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Views.push(arg.toObject());
      });
    } else {
      objToSend.Views.push(args.toObject());
    }

    DataManager.listServiceInstances({service_type_id: Enums.ServiceType.VIEW})
      .then(function(services) {
        try {
          services.forEach(function(service) {
            TcpManager.sendData(service, objToSend);
          });
        } catch (err) {
          console.log(err);
        }
      });
  }
  /**
   * It applies a save operation and send views to the service
   * 
   * @param {Object} viewObject - A view object to save
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  View.save = function(viewObject, projectId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var requester = RequestFactory.build(viewObject.serverUriObject);
        var options = {transaction: t};

        // setting built uri
        viewObject.maps_server_uri = requester.uri;
        // setting current project scope
        viewObject.project_id = projectId;

        return DataManager.addSchedule(viewObject.schedule, options)
          .then(function(schedule) {
            viewObject.schedule_id = schedule.id;
            return DataManager.addView(viewObject, options);
          });
      })
      .then(function(view) {
        // sending to the services
        sendView(view);

        return resolve(view);
      })
      
      .catch(function(err){
        return reject(err);
      });
    });
  };
  /**
   * It retrieves views from database. It applies a filter by ID if there is.
   * 
   * @param {number} viewId - View Identifier
   * @param {number} projectId - A project identifier
   * @returns {Promise<View[]>}
   */
  View.retrieve = function(viewId, projectId) {
    return new PromiseClass(function(resolve, reject) {
      if (viewId) {
        return DataManager.getView({id: viewId})
          .then(function(view) { return resolve(view.toObject()); })
          .catch(function(err) { return reject(err); });
      }

      DataManager.listViews({project_id: projectId})
        .then(function(views) {
          return resolve(views.map(function(view) {
            return view.toObject();
          }));
        })

        .catch(function(err) {
          return reject(err);
        });
    });
  };
  /**
   * It performs update view from database from view identifier
   * 
   * @param {number} viewId - View Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  View.update = function(viewId, viewObject, projectId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        return DataManager.getView({id: viewId, project_id: projectId}, options)
          .then(function(view) {
            return DataManager.updateSchedule(view.schedule.id, viewObject.schedule, options)
              .then(function() {
                var requester = RequestFactory.build(viewObject.serverUriObject);
                // setting built uri
                viewObject.maps_server_uri = requester.uri;
                return DataManager.updateView({id: viewId}, viewObject, options)
                  .then(function() {
                    return DataManager.getView({id: viewId}, options);
                  });
              });
          });
      })

      .then(function(view) {
        sendView(view);

        return resolve(view);
      })

      .catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * It performs remove view from database from view identifier
   * 
   * @param {number} viewId - View Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  View.remove = function(viewId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        
        return DataManager.getView({id: viewId}, options)
          .then(function(view) {
            return DataManager.removeView({id: viewId}, options)
              .then(function() {
                return view;
              });
          });
      })
      
      .then(function(view) {
        // removing views from tcp services
        Utils.removeDataSignal(DataManager, TcpManager, {
          "Views": [view.id]
        });

        return resolve(view);
      })
      
      .catch(function(err) {
        return reject(err);
      });
    });
  };
} ());