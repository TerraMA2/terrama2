(function() {
  'use strict';

  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var PromiseClass = require("./../Promise");
  var Application = require("./../Application");
  var ProjectError = require("./../Exceptions").ProjectError;
  const ProjectModel = require("./../data-model/Project");

  /**
   * It represents a mock to handle project.
   * It is used in Project API
   *
   * @class Project
   */
  var Project = module.exports = {};

  /**
   * Helper to send providers via TCP
   *
   * @param {Array} providers Providers values to send
   * @param {Array} projects Projects values
   */
  function sendProviders(providers, projects) {
    var objToSend = {
      "Projects": projects,
      "DataProviders": []
    };

    providers.forEach(function(provider){
      objToSend.DataProviders.push(provider.toService());
    });

    TcpService.send(objToSend);
  };

  /**
   * Helper to send project data via TCP
   *
   * @param {Object} project Project values to send
   */
  function projectReceived(project) {
    TcpService.emitEvent("projectReceived", project);
  };

  /**
   * It applies a save operation and send project to the service
   *
   * @param {Object} projectObject - A project object to save
   * @param {number} userId - A user identifier
   * @param {Object} options - Transaction options
   * @returns {Promise<Project>}
   */
  Project.save = function(projectObject, userId, options) {
    return new PromiseClass(function(resolve, reject) {
      projectObject.user_id = userId;

      DataManager.addProject(projectObject)
        .then(function(project) {
          projectReceived(project);

          // Creating default PostGIS and File providers
          var configFile = Application.getContextConfig();

          // File data provider object
          // configFile.defaultFilePathList - The first list position returns the default file path.
          var DefaultFileProvider = {
            name: "Local Folder",
            uri: "file://" + configFile.defaultFilePathList[0],
            description: "Local Folder data server",
            data_provider_intent_id: 1,
            data_provider_type_id: 1,
            project_id: project.id,
            active: true
          };

          // PostGIS data provider object
          var uriPostgis = "postgis://" + configFile.db.username + ":" + configFile.db.password + "@" + configFile.db.host + ":5432/" + configFile.db.database;
          var DefaultPostgisProvider = {
            name: "Local Database PostGIS",
            uri: uriPostgis,
            description: "Local Database PostGIS data server",
            data_provider_intent_id: 1,
            data_provider_type_id: 4,
            project_id: project.id,
            active: true
          };
          var promises = [];
          promises.push(DataManager.addDataProvider(DefaultFileProvider));
          promises.push(DataManager.addDataProvider(DefaultPostgisProvider));

          return Promise.all(promises)
            .then(providers => sendProviders(providers, [new ProjectModel(project).toObject()]))
            .then(() => resolve(project));
        })
        .catch(err =>reject(err));
    });
  };

  /**
   * It retrieves projects from database. It applies a filter by ID if there is.
   *
   * @param {number} projectId - Project Identifier
   * @param {object} user - User data
   * @returns {Promise<Project[]>}
   */
  Project.retrieve = function(projectId, user) {
    return new PromiseClass(function(resolve, reject) {
      if(projectId) {
        DataManager.getProject({ id: projectId }).then(function(project) {
          return resolve(project);
        }).catch(function(err) {
          return reject(err);
        });
      } else {
        var projects = DataManager.listProjects();

        projects.forEach(function(project) {
          project.hasPermission = !project.protected || user.administrator || user.id == project.user_id;
        });

        return resolve(projects);
      }
    });
  };

  /**
   * It performs update project from database from project identifier
   *
   * @param {number} projectId - Project Identifier
   * @param {Object} projectObject - Project object values
   * @param {number} userId - A user identifier
   * @returns {Promise<Project>}
   */
  Project.update = function(projectId, projectObject, userId) {
    return new PromiseClass(function(resolve, reject) {
      if(projectId) {
        var projectGiven = projectObject;
        projectGiven.id = projectId;
        projectGiven.user_id = projectGiven.user_id ? projectGiven.user_id : userId;

        DataManager.updateProject(projectGiven).then(function(project) {
          projectReceived(project);

          // Sending through TCP
          TcpService.send({"Projects": [new ProjectModel(project).toObject()]});
          return resolve(project);
        }).catch(function(err) {
          return reject(err);
        });
      } else {
        return reject(new ProjectError("Project name not identified"));
      }
    });
  };

  /**
   * It performs remove project from database from project identifier
   *
   * @param {number} projectId - Project Identifier
   * @returns {Promise<Project>}
   */
  Project.remove = function(projectId) {
    return new PromiseClass(function(resolve, reject) {
      if(projectId) {
        DataManager.getProject({id: projectId}).then(function(project) {
          var objectToSend = {
            "Alerts": [],
            "Analysis": [],
            "Collectors": [],
            "DataProvider": [],
            "DataSeries": [],
            "Legends": [],
            "Views": [],
            "Storages": [],
            "Projects": [new ProjectModel(project).toObject()]
          };

          var dataProviders = DataManager.listDataProviders({ project_id: projectId });

          dataProviders.forEach(function(dataProvider) {
            objectToSend.DataProvider.push(dataProvider.id);
          });

          var collectorPromises = [];

          DataManager.listAlerts({ project_id: projectId }).then(function(alerts) {
            alerts.forEach(function(alert) {
              objectToSend.Alerts.push(alert.id);
            });

            return DataManager.listAnalysis({ project_id: projectId });
          }).then(function(analysisList) {
            analysisList.forEach(function(analysis) {
              objectToSend.Analysis.push(analysis.id);
            });

            return DataManager.listDataSeries({ project_id: projectId });
          }).then(function(dataSeriesList) {
            dataSeriesList.forEach(function(dataSeries) {
              objectToSend.DataSeries.push(dataSeries.id);

              collectorPromises.push(
                DataManager.getCollector({ data_series_output: dataSeries.id }).then(function(collector) {
                  objectToSend.Collectors.push(collector.id);
                })
              );
            });

            return DataManager.listLegends({ project_id: projectId });
          }).then(function(legends) {
            legends.forEach(function(legend) {
              objectToSend.Legends.push(legend.id);
            });

            return DataManager.listViews({ project_id: projectId });
          }).then(function(views) {
            views.forEach(function(view) {
              objectToSend.Views.push(view.id);
            });

            return DataManager.listStorages({ project_id: projectId });
          }).then(function(storages) {
            storages.forEach(function(storage) {
              objectToSend.Storages.push(storage.id);
            });

            return Promise.all(collectorPromises).catch(function(err) {
              if(!err.name || (err.name && err.name != "CollectorErrorNotFound"))
                return reject(new ProjectError("Failed to load dependents"));
            });
          }).then(function() {
            DataManager.removeProject({id: projectId}).then(function() {
              TcpService.emitEvent("projectDeleted", { id: projectId });

              if(objectToSend.Alerts.length === 0)
                delete objectToSend.Alerts;

              if(objectToSend.Analysis.length === 0)
                delete objectToSend.Analysis;

              if(objectToSend.Collectors.length === 0)
                delete objectToSend.Collectors;

              if(objectToSend.DataProvider.length === 0)
                delete objectToSend.DataProvider;

              if(objectToSend.DataSeries.length === 0)
                delete objectToSend.DataSeries;

              if(objectToSend.Legends.length === 0)
                delete objectToSend.Legends;

              if(objectToSend.Views.length === 0)
              delete objectToSend.Views;

              if(objectToSend.Storages.length === 0)
              delete objectToSend.Storages;

              if(
                objectToSend.hasOwnProperty("Alerts") ||
                objectToSend.hasOwnProperty("Analysis") ||
                objectToSend.hasOwnProperty("Collectors") ||
                objectToSend.hasOwnProperty("DataProvider") ||
                objectToSend.hasOwnProperty("DataSeries") ||
                objectToSend.hasOwnProperty("Legends") ||
                objectToSend.hasOwnProperty("Views") ||
                objectToSend.hasOwnProperty("Storages")
              ) {
                TcpService.remove(objectToSend);
              }

              return resolve(project);
            }).catch(function(err) {
              return reject(err);
            });
          }).catch(function(err) {
            return reject(new ProjectError("Failed to load dependents"));
          });
        }).catch(function(err) {
          return reject(new ProjectError("Project not found"));
        });
      } else {
        return reject(new ProjectError("Project id not typed"));
      }
    });
  };
} ());