define(function() {
  /**
   * TerraMA² Controller responsible for project listing.
   *
   * @class ListController
   */
  function ListController($scope, $http, Socket, FileDialog, SaveAs, $log, i18n, $window, MessageBoxService, AnalysisService, LegendService, InterpolatorService, $timeout) {
    $scope.model = [];
    var config = $window.configuration;
    var socket = Socket;
    var title = config.context || "Project";
    var importTitle = "Data Import";
    $scope.MessageBoxService = MessageBoxService;
    $scope.i18n = i18n;
    $scope.linkToAdd = BASE_URL + "configuration/projects/new";
    $scope.fields = [
      {key: "name", as: i18n.__("Name")},
      {key: "description", as: i18n.__("Description")}
    ];

    $scope.projectsCheckboxes = {};
    $scope.dependencies = {};

    $scope.projectRadio = null;
    $scope.exportData = {
      "Projects": [],
      "DataProviders": [],
      "DataSeries": [],
      "Collectors": [],
      "Analysis": [],
      "Views": [],
      "Legends": [],
      "Alerts": [],
      "Interpolators": []
    };
    $scope.services = {
      COLLECT: [],
      ANALYSIS: [],
      VIEW: [],
      ALERT: [],
      INTERPOLATOR: []
    };
    $scope.selectedServices = {};
    $scope.hasCollect = false;
    $scope.hasAnalysis = false;
    $scope.hasView = false;
    $scope.hasLegend = false;
    $scope.hasAlert = false;

    $scope.remove = function(object) {
      return BASE_URL + "api/Project/" + object.id + "/delete";
    };

    $scope.link = function(object) {
      return BASE_URL + "configuration/projects/" + object.name + "/activate";
    };

    $scope.projectCheck = function(element) {
      var flag = ($scope.projectsCheckboxes[element.id].project ? true : false);

      $scope.dependencies = {};

      if($scope.projectsCheckboxes[element.id].DataProviders != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].DataProviders) {
          if($scope.projectsCheckboxes[element.id].DataProviders.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].DataProviders[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].DataSeries != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].DataSeries) {
          if($scope.projectsCheckboxes[element.id].DataSeries.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].DataSeries[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].DataSeriesStatic != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].DataSeriesStatic) {
          if($scope.projectsCheckboxes[element.id].DataSeriesStatic.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].DataSeriesStatic[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].Analysis != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].Analysis) {
          if($scope.projectsCheckboxes[element.id].Analysis.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].Analysis[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].Views != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].Views) {
          if($scope.projectsCheckboxes[element.id].Views.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].Views[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].Legends != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].Legends) {
          if($scope.projectsCheckboxes[element.id].Legends.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].Legends[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].Alerts != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].Alerts) {
          if($scope.projectsCheckboxes[element.id].Alerts.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].Alerts[property] = flag;
        }
      }

      if($scope.projectsCheckboxes[element.id].Interpolators != undefined) {
        for(var property in $scope.projectsCheckboxes[element.id].Interpolators) {
          if($scope.projectsCheckboxes[element.id].Interpolators.hasOwnProperty(property))
            $scope.projectsCheckboxes[element.id].Interpolators[property] = flag;
        }
      }
    };

    $scope.itemCheck = function(projectId, objectId, objectType) {
      if($scope.projectsCheckboxes[projectId][objectType][objectId]) {
        socket.emit("getDependencies", {
          objectType: objectType,
          ids: [objectId],
          projectId: projectId
        });
      } else {
        if($scope.dependencies.hasOwnProperty(objectType) && $scope.dependencies.hasOwnProperty(objectId))
          delete $scope.dependencies[objectType][objectId];

        uncheckDependents(objectId, objectType, projectId);
      }
    };

    var uncheckDependents = function(dependencie, dependencieType, projectId) {
      for(var propertyType in $scope.dependencies) {
        if($scope.dependencies.hasOwnProperty(propertyType)) {
          for(var propertyId in $scope.dependencies[propertyType]) {
            if($scope.dependencies[propertyType].hasOwnProperty(propertyId) && $scope.dependencies[propertyType][propertyId][dependencieType] !== undefined) {
              var deleteObject = false;

              for(var i = 0, arrayLength = $scope.dependencies[propertyType][propertyId][dependencieType].length; i < arrayLength; i++) {
                if($scope.dependencies[propertyType][propertyId][dependencieType][i] == dependencie) {
                  deleteObject = true;
                  break;
                }
              }

              if(deleteObject) {
                $scope.projectsCheckboxes[projectId][propertyType][propertyId] = false;
                delete $scope.dependencies[propertyType][propertyId];
                uncheckDependents(propertyId, propertyType, projectId);
              }
            }
          }
        }
      }
    };

    $scope.iconFn = function(object){
      return BASE_URL + "images/project/project.png"
    };

    $scope.iconProperties = {
      type: "img",
      width: 20,
      height: 20
    };

    $scope.loading = true;

    $scope.close = function() { MessageBoxService.reset() };

    Array.prototype.unique = function() {
      var a = this.concat();

      for(var i = 0; i < a.length; ++i) {
        for(var j = i + 1; j < a.length; ++j) {
          if(a[i] === a[j])
            a.splice(j--, 1);
        }
      }

      return a;
    };

    socket.on("exportResponse", function(result) {
      $scope.extra.isExporting = false;
      if (result.err) {
        MessageBoxService.danger(i18n.__(title), i18n.__(result.err));
        return;
      }

      var extension = result.fileName.split(".");

      SaveAs(result.data, (extension.length > 1 && extension[extension.length - 1] === "terrama2" ? result.fileName : result.fileName + ".terrama2"));
    });

    socket.on("getDependenciesResponse", function(result) {
      if(result.err) {
        MessageBoxService.danger(i18n.__(title), i18n.__(result.err));
        return;
      }

      for(var property in result.data) {
        if(result.data.hasOwnProperty(property)) {
          var typeAndId = property.split('_');

          if(!$scope.dependencies.hasOwnProperty(typeAndId[0]))
            $scope.dependencies[typeAndId[0]] = {};

          if(!$scope.dependencies[typeAndId[0]].hasOwnProperty(typeAndId[1]))
            $scope.dependencies[typeAndId[0]][typeAndId[1]] = {};

          for(var arrayItem in result.data[property]) {
            if(result.data[property].hasOwnProperty(arrayItem)) {
              if($scope.dependencies[typeAndId[0]][typeAndId[1]].hasOwnProperty(arrayItem))
                $scope.dependencies[typeAndId[0]][typeAndId[1]][arrayItem] = $scope.dependencies[typeAndId[0]][typeAndId[1]][arrayItem].concat(result.data[property][arrayItem]).unique();
              else
                $scope.dependencies[typeAndId[0]][typeAndId[1]][arrayItem] = result.data[property][arrayItem];

              for(var i = 0, arrayLength = result.data[property][arrayItem].length; i < arrayLength; i++) {
                $scope.projectsCheckboxes[result.projectId][arrayItem][result.data[property][arrayItem][i]] = true;
              }
            }
          }
        }
      }
    });

    socket.on("importResponse", function(result) {
      $scope.loading = false;
      $scope.extra.isImporting = false;
      if(result.err) {
        MessageBoxService.danger(i18n.__(title), i18n.__(result.err));
        return;
      }

      if(result.data.Projects !== undefined && result.data.Projects.length > 0) {
        var msg = result.data.Projects.length + (result.data.Projects.length > 1 ? i18n.__(" projects has been imported. ") : i18n.__(" project has been imported. "));
        var canPush = [];

        for(var i = 0; i < result.data.Projects.length; ++i) {
          var p = result.data.Projects[i];
          var cont = 0;
          for(var j = 0; j < $scope.model.length; ++j) {
            var modelProject = $scope.model[j];
            if(p.name === modelProject.name) {
              // skip it
              break;
            }
            ++cont;
          }
          if(cont === $scope.model.length) {
            // push
            $scope.model.push(p);
          }
          cont = 0;
        }
      } else {
        var msg = i18n.__(" The data has been imported. ");
      }

      MessageBoxService.success(i18n.__(title), msg);
    });

    // callback after remove operation
    $scope.extra = {
      canRemove: function(element){
        return element.hasPermission;
      },
      canEdit: function(element){
        return element.hasPermission
      },
      importJson: null,

      removeOperationCallback: function(err, data) {
        if(err)
          return MessageBoxService.danger(i18n.__(title), i18n.__(err.message));

        $window.location.href = BASE_URL + "configuration/projects?token=" + data.token;
      },

      project: {
        edit: function(element) {
          return BASE_URL + "configuration/projects/edit/" + element.name;
        }
      },

      openExportModal: function(element) {
        if(!element) return;

        $scope.currentProjectId = element.id;
        $scope.currentProjectName = element.name;
        $('#exportModal').modal('show');
      },

      export: function(element) {
        if(!element) return;

        if($scope.projectsCheckboxes[element.id] != undefined) {
          if($scope.projectsCheckboxes[element.id].project) {
            $scope.exportData.Projects.push(element);

            delete $scope.exportData.DataProviders;
            delete $scope.exportData.DataSeries;
            delete $scope.exportData.Collectors;
            delete $scope.exportData.Analysis;
            delete $scope.exportData.Views;
            delete $scope.exportData.Legends;
            delete $scope.exportData.Alerts;
            delete $scope.exportData.Interpolators;
          } else {
            if($scope.projectsCheckboxes[element.id].DataProviders != undefined) {
              for(var j = 0, dataProvidersLength = $scope.dataProviders[element.id].length; j < dataProvidersLength; j++) {
                if($scope.projectsCheckboxes[element.id].DataProviders[$scope.dataProviders[element.id][j].id])
                  $scope.exportData.DataProviders.push($scope.dataProviders[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].DataSeries != undefined) {
              for(var j = 0, dataSeriesLength = $scope.dataSeries[element.id].length; j < dataSeriesLength; j++) {
                if($scope.projectsCheckboxes[element.id].DataSeries[$scope.dataSeries[element.id][j].id]) {
                  $scope.exportData.DataSeries.push($scope.dataSeries[element.id][j]);

                  for(var x = 0, collectorsLength = $scope.collectors[element.id].length; x < collectorsLength; x++) {
                    if($scope.collectors[element.id][x].output_data_series === $scope.dataSeries[element.id][j].id) {
                      $scope.exportData.Collectors.push($scope.collectors[element.id][x]);
                      break;
                    }
                  }
                }
              }
            }

            if($scope.projectsCheckboxes[element.id].DataSeriesStatic != undefined) {
              for(var j = 0, dataSeriesStaticLength = $scope.dataSeriesStatic[element.id].length; j < dataSeriesStaticLength; j++) {
                if($scope.projectsCheckboxes[element.id].DataSeriesStatic[$scope.dataSeriesStatic[element.id][j].id]) {
                  $scope.exportData.DataSeries.push($scope.dataSeriesStatic[element.id][j]);

                  for(var x = 0, collectorsLength = $scope.collectors[element.id].length; x < collectorsLength; x++) {
                    if($scope.collectors[element.id][x].output_data_series === $scope.dataSeriesStatic[element.id][j].id) {
                      $scope.exportData.Collectors.push($scope.collectors[element.id][x]);
                      break;
                    }
                  }
                }
              }
            }

            if($scope.projectsCheckboxes[element.id].Analysis != undefined) {
              for(var j = 0, analysisLength = $scope.analysis[element.id].length; j < analysisLength; j++) {
                if($scope.projectsCheckboxes[element.id].Analysis[$scope.analysis[element.id][j].id])
                  $scope.exportData.Analysis.push($scope.analysis[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].Views != undefined) {
              for(var j = 0, viewsLength = $scope.views[element.id].length; j < viewsLength; j++) {
                if($scope.projectsCheckboxes[element.id].Views[$scope.views[element.id][j].id])
                  $scope.exportData.Views.push($scope.views[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].Legends != undefined) {
              for(var j = 0, legendsLength = $scope.legends[element.id].length; j < legendsLength; j++) {
                if($scope.projectsCheckboxes[element.id].Legends[$scope.legends[element.id][j].id])
                  $scope.exportData.Legends.push($scope.legends[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].Alerts != undefined) {
              for(var j = 0, alertsLength = $scope.alerts[element.id].length; j < alertsLength; j++) {
                if($scope.projectsCheckboxes[element.id].Alerts[$scope.alerts[element.id][j].id])
                  $scope.exportData.Alerts.push($scope.alerts[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].Interpolators != undefined) {
              for(var j = 0, interpolatorsLength = $scope.interpolators[element.id].length; j < interpolatorsLength; j++) {
                if($scope.projectsCheckboxes[element.id].Interpolators[$scope.interpolators[element.id][j].id])
                  $scope.exportData.Interpolators.push($scope.interpolators[element.id][j]);
              }
            }

            if($scope.exportData.Projects.length == 0) delete $scope.exportData.Projects;
            if($scope.exportData.DataProviders.length == 0) delete $scope.exportData.DataProviders;
            if($scope.exportData.DataSeries.length == 0) delete $scope.exportData.DataSeries;
            if($scope.exportData.Collectors.length == 0) delete $scope.exportData.Collectors;
            if($scope.exportData.Analysis.length == 0) delete $scope.exportData.Analysis;
            if($scope.exportData.Views.length == 0) delete $scope.exportData.Views;
            if($scope.exportData.Legends.length == 0) delete $scope.exportData.Legends;
            if($scope.exportData.Alerts.length == 0) delete $scope.exportData.Alerts;
            if($scope.exportData.Interpolators.length == 0) delete $scope.exportData.Interpolators;
          }
        }

        $scope.extra.isExporting = true;
        $scope.exportData['currentProjectId'] = $scope.currentProjectId;
        $scope.exportData['currentProjectName'] = $scope.currentProjectName;
        $scope.exportData['fileName'] = $scope.projectsCheckboxes[element.id].fileName;
        socket.emit("export", $scope.exportData);

        $scope.exportData = {
          "Projects": [],
          "DataProviders": [],
          "DataSeries": [],
          "Collectors": [],
          "Analysis": [],
          "Views": [],
          "Legends": [],
          "Alerts": [],
          "Interpolators": []
        };

        $('#exportModal').modal('hide');
      },

      import: function() {
        $scope.extra.isImporting = false;

        FileDialog.openFile(function(err, input) {
          if (err) {
            MessageBoxService.danger(i18n.__(importTitle), i18n.__(err.toString()));
            return;
          }

          $scope.loading = true;
          FileDialog.readAsJSON(input.files[0], function(error, json) {
            // applying angular scope..
            $scope.$apply(function() {
              $scope.extra.isImporting = true;
              if(error) {
                $scope.extra.isImporting = false;
                MessageBoxService.danger(i18n.__(importTitle), i18n.__(error.message));
                return;
              }

              if (!json.hasOwnProperty("Projects") &&
                  !json.hasOwnProperty("DataSeries") &&
                  !json.hasOwnProperty("DataProviders") &&
                  !json.hasOwnProperty("Analysis") &&
                  !json.hasOwnProperty("Collectors") &&
                  !json.hasOwnProperty("Views") &&
                  !json.hasOwnProperty("Legends") &&
                  !json.hasOwnProperty("Alerts") &&
                  !json.hasOwnProperty("Interpolators")) {
                MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("Invalid configuration file")));
                return;
              }

              if(json.Projects !== undefined && json.Projects.length > 0) {
                json.userId = config.user_id
                socket.emit("import", json);
              } else {
                $scope.extra.isImporting = false;
                $scope.extra.importJson = json;

                $scope.hasCollect = false;
                $scope.hasAnalysis = false;
                $scope.hasView = false;
                $scope.hasLegend = false;
                $scope.hasAlert = false;
                $scope.hasInterpolator = false;

                if(json.Collectors !== undefined && json.Collectors.length > 0) $scope.hasCollect = true;
                if(json.Analysis !== undefined && json.Analysis.length > 0) $scope.hasAnalysis = true;
                if(json.Views !== undefined && json.Views.length > 0) $scope.hasView = true;
                if(json.Legends !== undefined && json.Legends.length > 0) $scope.hasLegend = true;
                if(json.Alerts !== undefined && json.Alerts.length > 0) $scope.hasAlert = true;
                if(json.Interpolators !== undefined && json.Interpolators.length > 0) $scope.hasInterpolator = true;

                if($scope.model === undefined || $scope.model.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one project")));
                } else if(json.Collectors !== undefined && json.Collectors.length > 0 && $scope.services.COLLECT.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one collector service")));
                } else if(json.Analysis !== undefined && json.Analysis.length > 0 && $scope.services.ANALYSIS.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one analysis service")));
                } else if(json.Views !== undefined && json.Views.length > 0 && $scope.services.VIEW.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one view service")));
                } else if(json.Alerts !== undefined && json.Alerts.length > 0 && $scope.services.ALERT.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one alert service")));
                } else if(json.Interpolators !== undefined && json.Interpolators.length > 0 && $scope.services.INTERPOLATOR.length === 0) {
                  MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("To import this file you need to have at least one interpolator service")));
                } else {
                  $('#importModal').modal('show');
                }
              }
            });
          });
        }, false, ".terrama2, .json, application/json");
      },

      finalizeImportation: function() {
        if($scope.projectRadio === null) {
          MessageBoxService.danger(i18n.__(importTitle), new Error(i18n.__("Select a project")));
          $('#importModal').modal('hide');
        } else {
          $scope.extra.isImporting = true;
          $scope.extra.importJson['selectedProject'] = $scope.projectRadio;

          if($scope.hasCollect)
            $scope.extra.importJson['servicesCollect'] = $scope.selectedServices.Collect;

          if($scope.hasAnalysis)
            $scope.extra.importJson['servicesAnalysis'] = $scope.selectedServices.Analysis;

          if($scope.hasView)
            $scope.extra.importJson['servicesView'] = $scope.selectedServices.View;

          if($scope.hasAlert)
            $scope.extra.importJson['servicesAlert'] = $scope.selectedServices.Alert;

          if($scope.hasInterpolator)
            $scope.extra.importJson['servicesInterpolator'] = $scope.selectedServices.Interpolator;

          socket.emit("import", $scope.extra.importJson);
          $('#importModal').modal('hide');

          $scope.hasCollect = false;
          $scope.hasAnalysis = false;
          $scope.hasView = false;
          $scope.hasLegend = false;
          $scope.hasAlert = false;
        }
      }
    };

    $http.get(BASE_URL + "api/Service/", {}).then(function(services) {
      for(var j = 0, servicesLength = services.data.length; j < servicesLength; j++) {
        switch(services.data[j].service_type_id) {
          case 1:
            $scope.services.COLLECT.push(services.data[j]);
            break;
          case 2:
            $scope.services.ANALYSIS.push(services.data[j]);
            break;
          case 3:
            $scope.services.VIEW.push(services.data[j]);
            break;
          case 4:
            $scope.services.ALERT.push(services.data[j]);
            break;
          case 5:
            $scope.services.INTERPOLATOR.push(services.data[j]);
            break;
          default:
            break;
        }
      }
    }, function(err) {
      console.log("Err in retrieving services");
    }).finally(function() {
      $scope.loading = false;
    });

    $http.get(BASE_URL + "api/Project/", {}).then(function(response) {
      $scope.model = response.data;
      $scope.dataProviders = {};
      $scope.dataSeries = {};
      $scope.dataSeriesStatic = {};
      $scope.analysis = {};
      $scope.views = {};
      $scope.alerts = {};
      $scope.legends = {};
      $scope.collectors = {};
      $scope.interpolators = {};

      response.data.map(function(project, index) {
        if($scope.projectsCheckboxes[project.id] == undefined)
          $scope.projectsCheckboxes[project.id] = {
            fileName: project.name,
            project: true
          };

        $http.get(BASE_URL + "api/DataProvider/project/" + project.id, {}).then(function(dataProviders) {
          $scope.dataProviders[project.id] = dataProviders.data;

          if($scope.projectsCheckboxes[project.id].DataProviders == undefined)
            $scope.projectsCheckboxes[project.id].DataProviders = {};

          for(var j = 0, dataProvidersLength = dataProviders.data.length; j < dataProvidersLength; j++) {
            $scope.projectsCheckboxes[project.id].DataProviders[dataProviders.data[j].id] = true;
          }
        }, function(err) {
          console.log("Err in retrieving data providers");
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get(BASE_URL + "api/DataSeries/project/" + project.id, {
          params: {
            collector: true,
            type: "dynamic",
            ignoreAnalysisOutputDataSeries: true,
            ignoreInterpolatorOutputDataSeries: true
          }
        }).then(function(dataSeries) {
          $scope.dataSeries[project.id] = dataSeries.data;

          if($scope.projectsCheckboxes[project.id].DataSeries == undefined)
            $scope.projectsCheckboxes[project.id].DataSeries = {};

          var dataSeriesIds = [];

          for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
            $scope.projectsCheckboxes[project.id].DataSeries[dataSeries.data[j].id] = true;
            dataSeriesIds.push(dataSeries.data[j].id);
          }

          if(dataSeriesIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "DataSeries",
              ids: dataSeriesIds,
              projectId: project.id
            });
          }
        }, function(err) {
          console.log("Err in retrieving data series");
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get(BASE_URL + "api/DataSeries/project/" + project.id, {
          params: {
            type: "static",
            ignoreAnalysisOutputDataSeries: true
          }
        }).then(function(dataSeries) {
          $scope.dataSeriesStatic[project.id] = dataSeries.data;

          if($scope.projectsCheckboxes[project.id].DataSeriesStatic == undefined)
            $scope.projectsCheckboxes[project.id].DataSeriesStatic = {};

          var dataSeriesIds = [];

          for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
            $scope.projectsCheckboxes[project.id].DataSeriesStatic[dataSeries.data[j].id] = true;
            dataSeriesIds.push(dataSeries.data[j].id);
          }

          if(dataSeriesIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "DataSeriesStatic",
              ids: dataSeriesIds,
              projectId: project.id
            });
          }
        }, function(err) {
          console.log("Err in retrieving data series static");
        }).finally(function() {
          $scope.loading = false;
        });

        AnalysisService.init({ project_id: project.id }).then(function(analysis) {
          $scope.analysis[project.id] = analysis;

          if($scope.projectsCheckboxes[project.id].Analysis == undefined)
            $scope.projectsCheckboxes[project.id].Analysis = {};

          var analysisIds = [];

          for(var j = 0, analysisLength = analysis.length; j < analysisLength; j++) {
            $scope.projectsCheckboxes[project.id].Analysis[analysis[j].id] = true;
            analysisIds.push(analysis[j].id);
          }

          if(analysisIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "Analysis",
              ids: analysisIds,
              projectId: project.id
            });
          }
        }).catch(function(err) {
          $log.info("Err in retrieving Analysis " + err);
        }).finally(function() {
          $scope.loading = false;
        });

        InterpolatorService.init({ project_id: project.id }).then(function(interpolators){
          interpolators.map(function(interpolator){
            interpolator.dataSeriesInput = JSON.parse(interpolator.dataSeriesInput);
            interpolator.dataSeriesOutput = JSON.parse(interpolator.dataSeriesOutput);
            return interpolator;
          });
          $scope.interpolators[project.id] = interpolators;

          if($scope.projectsCheckboxes[project.id].Interpolators == undefined)
            $scope.projectsCheckboxes[project.id].Interpolators = {};

          var interpolatorsIds = [];

          for(var j = 0, interpolatorsLength = interpolators.length; j < interpolatorsLength; j++) {
            $scope.projectsCheckboxes[project.id].Interpolators[interpolators[j].id] = true;
            interpolatorsIds.push(interpolators[j].id);
          }

        }).catch(function(err){
          console.log(err);
        }).finally(function(){
          $scope.loading = false;
        });

        $http.get(BASE_URL + "api/ViewByProject/" + project.id, {}).then(function(views) {
          var viewFiltered = [];
          //Removing alerts view
          views.data.map(function(view){if (view.source_type != 4) viewFiltered.push(view);})
          $scope.views[project.id] = viewFiltered;

          if($scope.projectsCheckboxes[project.id].Views == undefined)
            $scope.projectsCheckboxes[project.id].Views = {};

          var viewsIds = [];

          for(var j = 0, viewsLength = viewFiltered.length; j < viewsLength; j++) {
            $scope.projectsCheckboxes[project.id].Views[viewFiltered[j].id] = true;
            viewsIds.push(viewFiltered[j].id);
          }

          if(viewsIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "Views",
              ids: viewsIds,
              projectId: project.id
            });
          }
        }, function(err) {
          console.log("Err in retrieving views");
        }).finally(function() {
          $scope.loading = false;
        });

        LegendService.init({ project_id: project.id }).then(function(legends) {
          $scope.legends[project.id] = legends;

          if($scope.projectsCheckboxes[project.id].Legends == undefined)
            $scope.projectsCheckboxes[project.id].Legends = {};

          var legendsIds = [];

          for(var j = 0, legendsLength = legends.length; j < legendsLength; j++) {
            $scope.projectsCheckboxes[project.id].Legends[legends[j].id] = true;
            legendsIds.push(legends[j].id);
          }

          if(legendsIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "Legends",
              ids: legendsIds,
              projectId: project.id
            });
          }
        }).catch(function(err) {
          $log.info("Err in retrieving Legends " + err);
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get(BASE_URL + "api/AlertByProject/" + project.id, {}).then(function(alerts) {
          $scope.alerts[project.id] = alerts.data;

          if($scope.projectsCheckboxes[project.id].Alerts == undefined)
            $scope.projectsCheckboxes[project.id].Alerts = {};

          var alertsIds = [];

          for(var j = 0, alertsLength = alerts.data.length; j < alertsLength; j++) {
            $scope.projectsCheckboxes[project.id].Alerts[alerts.data[j].id] = true;
            alertsIds.push(alerts.data[j].id);
          }

          if(alertsIds.length > 0) {
            socket.emit("getDependencies", {
              objectType: "Alerts",
              ids: alertsIds,
              projectId: project.id
            });
          }
        }, function(err) {
          console.log("Err in retrieving alerts");
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get(BASE_URL + "api/Collector/project/" + project.id, {}).then(function(collectors) {
          $scope.collectors[project.id] = collectors;
        }, function(err) {
          console.log("Err in retrieving collectors");
        }).finally(function() {
          $scope.loading = false;
        });
      });
    }, function(err) {
      console.log("Err in retrieving projects");
    }).finally(function() {
      $scope.loading = false;
    });

    if(config.message) {
      var messageArray = config.message.split(" ");
      var tokenCodeMessage = messageArray[messageArray.length - 1];
      messageArray.splice(messageArray.length - 1, 1);

      $timeout(function() {
        var finalMessage = messageArray.join(" ") + " " + i18n.__(tokenCodeMessage);
        MessageBoxService.success(i18n.__(title), finalMessage);
      }, 2000);
    }
  }

  ListController.$inject = ["$scope", "$http", "Socket", "FileDialog", "SaveAs", "$log", "i18n", "$window", "MessageBoxService", "AnalysisService", "LegendService", "InterpolatorService", "$timeout"];

  return ListController;
})
