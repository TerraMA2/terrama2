define(function() {
  /**
   * TerraMA² Controller responsible for project listing.
   * 
   * @class ListController
   */
  function ListController($scope, $http, Socket, FileDialog, SaveAs, $log, i18n, $window, MessageBoxService, AnalysisService) {
    $scope.model = [];
    var config = $window.configuration;
    var socket = Socket;
    var title = i18n.__(config.context || "Project");
    $scope.MessageBoxService = MessageBoxService;
    $scope.i18n = i18n;
    $scope.linkToAdd = "/configuration/projects/new";
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
      "Views": []
    };
    $scope.selectedServices = {};
    $scope.hasCollect = false;
    $scope.hasAnalysis = false;
    $scope.hasView = false;

    $scope.remove = function(object) {
      return "/api/Project/" + object.id + "/delete";
    };

    $scope.link = function(object) {
      return "/configuration/projects/" + object.name + "/activate";
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
      return "/images/project/project.png"
    };

    $scope.iconProperties = {
      type: "img",
      width: 20,
      height: 20
    };

    $scope.loading = true;

    if (config.message) {
      MessageBoxService.success(title, config.message);
    }

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
        MessageBoxService.danger(title, result.err);
        return;
      }

      SaveAs(result.data, result.projectName + ".terrama2");
    });

    socket.on("getDependenciesResponse", function(result) {
      if(result.err) {
        MessageBoxService.danger(title, result.err);
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
      if (result.err) {
        MessageBoxService.danger(title, result.err);
        return;
      }

      if(result.data.Projects !== undefined && result.data.Projects.length > 0) {
        var msg = result.data.Projects.length + i18n.__(" projects has been imported. ");
        var canPush = [];

        for(var i = 0; i < result.data.Projects.length; ++i) {
          var p = result.data.Projects[i];
          var cont = 0;
          for(var j = 0; j < $scope.model.length; ++j) {
            var modelProject = $scope.model[j];
            if (p.name === modelProject.name) {
              // skip it
              break;
            }
            ++cont;
          }
          if (cont === $scope.model.length) {
            // push
            $scope.model.push(p);
          }
          cont = 0;
        }
      } else {
        var msg = i18n.__(" The data has been imported. ");
      }

      MessageBoxService.success(title, msg);
    });

    // callback after remove operation
    $scope.extra = {
      importJson: null,

      removeOperationCallback: function(err, data) {
        if (err) {
          return MessageBoxService.danger(title, err.message);
        }
        MessageBoxService.success(title, data.name + " removed");
      },

      project: {
        edit: function(element) {
          return "/configuration/projects/edit/" + element.name;
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

            if($scope.exportData.Projects.length == 0) delete $scope.exportData.Projects;
            if($scope.exportData.DataProviders.length == 0) delete $scope.exportData.DataProviders;
            if($scope.exportData.DataSeries.length == 0) delete $scope.exportData.DataSeries;
            if($scope.exportData.Collectors.length == 0) delete $scope.exportData.Collectors;
            if($scope.exportData.Analysis.length == 0) delete $scope.exportData.Analysis;
            if($scope.exportData.Views.length == 0) delete $scope.exportData.Views;
          }
        }

        $scope.extra.isExporting = true;
        $scope.exportData['currentProjectId'] = $scope.currentProjectId;
        $scope.exportData['currentProjectName'] = $scope.currentProjectName;
        socket.emit("export", $scope.exportData);

        $scope.exportData = {
          "Projects": [],
          "DataProviders": [],
          "DataSeries": [],
          "Collectors": [],
          "Analysis": [],
          "Views": []
        };

        $('#exportModal').modal('hide');
      },

      import: function() {
        $scope.extra.isImporting = false;

        FileDialog.openFile(function(err, input) {
          if (err) {
            MessageBoxService.danger("Data Import", err.toString());
            return;
          }

          $scope.loading = true;
          FileDialog.readAsJSON(input.files[0], function(error, json) {
            // applying angular scope..
            $scope.$apply(function() {
              $scope.extra.isImporting = true;
              if(error) {
                MessageBoxService.danger("Data Import", error);
                console.log(error);
                return;
              }

              if (!json.hasOwnProperty("Projects") &&
                  !json.hasOwnProperty("DataSeries") &&
                  !json.hasOwnProperty("DataProviders") &&
                  !json.hasOwnProperty("Analysis") &&
                  !json.hasOwnProperty("Collectors") &&
                  !json.hasOwnProperty("Views")) {
                MessageBoxService.danger("Data Import", new Error("Invalid configuration file"));
                return;
              }

              if(json.Projects !== undefined && json.Projects.length > 0) {
                socket.emit("import", json);
              } else {
                $scope.extra.isImporting = false;
                $scope.extra.importJson = json;

                $scope.hasCollect = false;
                $scope.hasAnalysis = false;
                $scope.hasView = false;

                if(json.Collectors !== undefined && json.Collectors.length > 0) $scope.hasCollect = true;
                if(json.Analysis !== undefined && json.Analysis.length > 0) $scope.hasAnalysis = true;
                if(json.Views !== undefined && json.Views.length > 0) $scope.hasView = true; 

                if($scope.model === undefined || $scope.model.length === 0) {
                  MessageBoxService.danger("Data Import", new Error("To import this file you need to have at least one project"));
                } else if(json.Collectors !== undefined && json.Collectors.length > 0 && $scope.services.COLLECT.length === 0) {
                  MessageBoxService.danger("Data Import", new Error("To import this file you need to have at least one collector service"));
                } else if(json.Analysis !== undefined && json.Analysis.length > 0 && $scope.services.ANALYSIS.length === 0) {
                  MessageBoxService.danger("Data Import", new Error("To import this file you need to have at least one analysis service"));
                } else if(json.Views !== undefined && json.Views.length > 0 && $scope.services.VIEW.length === 0) {
                  MessageBoxService.danger("Data Import", new Error("To import this file you need to have at least one view service"));
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
          MessageBoxService.danger("Data Import", new Error("Select a project"));
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

          socket.emit("import", $scope.extra.importJson);
          $('#importModal').modal('hide');

          $scope.hasCollect = false;
          $scope.hasAnalysis = false;
          $scope.hasView = false; 
        }
      }
    };

    $http.get("/api/Project/", {}).then(function(response) {
      $scope.model = response.data;
      $scope.dataProviders = {};
      $scope.dataSeries = {};
      $scope.dataSeriesStatic = {};
      $scope.analysis = {};
      $scope.views = {};
      $scope.collectors = {};
      $scope.services = {
        COLLECT: [],
        ANALYSIS: [],
        VIEW: []
      };

      response.data.map(function(project, index) {
        if($scope.projectsCheckboxes[project.id] == undefined)
          $scope.projectsCheckboxes[project.id] = {
            project: true
          };

        $http.get("/api/DataProvider/project/" + project.id, {}).success(function(dataProviders) {
          $scope.dataProviders[project.id] = dataProviders;

          if($scope.projectsCheckboxes[project.id].DataProviders == undefined)
            $scope.projectsCheckboxes[project.id].DataProviders = {};

          for(var j = 0, dataProvidersLength = dataProviders.length; j < dataProvidersLength; j++) {
            $scope.projectsCheckboxes[project.id].DataProviders[dataProviders[j].id] = true;
          }
        }).catch(function(err) {
          console.log("Err in retrieving data providers");
        }).finally(function() {
          $scope.loading = false;
        });

        $http({
          url: "/api/DataSeries/project/" + project.id,
          method: "GET",
          params: {
            collector: true,
            type: "dynamic",
            ignoreAnalysisOutputDataSeries: true
          }
        }).then(
          function(dataSeries) {
            $scope.dataSeries[project.id] = dataSeries.data;

            if($scope.projectsCheckboxes[project.id].DataSeries == undefined)
              $scope.projectsCheckboxes[project.id].DataSeries = {};

            var dataSeriesIds = [];

            for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
              $scope.projectsCheckboxes[project.id].DataSeries[dataSeries.data[j].id] = true;
              dataSeriesIds.push(dataSeries.data[j].id);
            }

            socket.emit("getDependencies", {
              objectType: "DataSeries",
              ids: dataSeriesIds,
              projectId: project.id
            });
          }, function(err) {
            console.log("Err in retrieving data series");
          }
        ).finally(function() {
          $scope.loading = false;
        });

        $http({
          url: "/api/DataSeries/project/" + project.id,
          method: "GET",
          params: {
            type: "static",
            ignoreAnalysisOutputDataSeries: true
          }
        }).then(
          function(dataSeries) {
            $scope.dataSeriesStatic[project.id] = dataSeries.data;

            if($scope.projectsCheckboxes[project.id].DataSeriesStatic == undefined)
              $scope.projectsCheckboxes[project.id].DataSeriesStatic = {};

            var dataSeriesIds = [];

            for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
              $scope.projectsCheckboxes[project.id].DataSeriesStatic[dataSeries.data[j].id] = true;
              dataSeriesIds.push(dataSeries.data[j].id);
            }

            socket.emit("getDependencies", {
              objectType: "DataSeriesStatic",
              ids: dataSeriesIds,
              projectId: project.id
            });
          }, function(err) {
            console.log("Err in retrieving data series static");
          }
        ).finally(function() {
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

          socket.emit("getDependencies", {
            objectType: "Analysis",
            ids: analysisIds,
            projectId: project.id
          });
        }).catch(function(err) {
          $log.info("Err in retrieving Analysis " + err);
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get("/api/View", {}).success(function(views) {
          $scope.views[project.id] = views;

          if($scope.projectsCheckboxes[project.id].Views == undefined)
            $scope.projectsCheckboxes[project.id].Views = {};

          var viewsIds = [];

          for(var j = 0, viewsLength = views.length; j < viewsLength; j++) {
            $scope.projectsCheckboxes[project.id].Views[views[j].id] = true;
            viewsIds.push(views[j].id);
          }

          socket.emit("getDependencies", {
            objectType: "Views",
            ids: viewsIds,
            projectId: project.id
          });
        }).catch(function(err) {
          console.log("Err in retrieving views");
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get("/api/Collector/project/" + project.id, {}).success(function(collectors) {
          $scope.collectors[project.id] = collectors;
        }).catch(function(err) {
          console.log("Err in retrieving collectors");
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get("/api/Service/", {}).success(function(services) {
          for(var j = 0, servicesLength = services.length; j < servicesLength; j++) {
            switch(services[j].service_type_id) {
              case 1:
                $scope.services.COLLECT.push(services[j]);
                break;
              case 2:
                $scope.services.ANALYSIS.push(services[j]);
                break;
              case 3:
                $scope.services.VIEW.push(services[j]);
                break;
              default:
                break;
            }
          }
        }).catch(function(err) {
          console.log("Err in retrieving services");
        }).finally(function() {
          $scope.loading = false;
        });
      });
    }).catch(function(err) {
      console.log("Err in retrieving projects");
    }).finally(function() {
      $scope.loading = false;
    });
  }

  ListController.$inject = ["$scope", "$http", "Socket", "FileDialog", "SaveAs", "$log", "i18n", "$window", "MessageBoxService", "AnalysisService"];

  return ListController;
})