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
      if($scope.projectsCheckboxes[element.id].project) {
        if($scope.projectsCheckboxes[element.id].dataProviders != undefined) {
          for(var property in $scope.projectsCheckboxes[element.id].dataProviders) {
            if($scope.projectsCheckboxes[element.id].dataProviders.hasOwnProperty(property))
              $scope.projectsCheckboxes[element.id].dataProviders[property] = true;
          }
        }

        if($scope.projectsCheckboxes[element.id].dataSeries != undefined) {
          for(var property in $scope.projectsCheckboxes[element.id].dataSeries) {
            if($scope.projectsCheckboxes[element.id].dataSeries.hasOwnProperty(property))
              $scope.projectsCheckboxes[element.id].dataSeries[property] = true;
          }
        }

        if($scope.projectsCheckboxes[element.id].dataSeriesStatic != undefined) {
          for(var property in $scope.projectsCheckboxes[element.id].dataSeriesStatic) {
            if($scope.projectsCheckboxes[element.id].dataSeriesStatic.hasOwnProperty(property))
              $scope.projectsCheckboxes[element.id].dataSeriesStatic[property] = true;
          }
        }

        if($scope.projectsCheckboxes[element.id].analysis != undefined) {
          for(var property in $scope.projectsCheckboxes[element.id].analysis) {
            if($scope.projectsCheckboxes[element.id].analysis.hasOwnProperty(property))
              $scope.projectsCheckboxes[element.id].analysis[property] = true;
          }
        }

        if($scope.projectsCheckboxes[element.id].views != undefined) {
          for(var property in $scope.projectsCheckboxes[element.id].views) {
            if($scope.projectsCheckboxes[element.id].views.hasOwnProperty(property))
              $scope.projectsCheckboxes[element.id].views[property] = true;
          }
        }
      }
    };

    $scope.itemCheck = function(projectId, objectId, objectType) {
      if($scope.projectsCheckboxes[projectId][objectType][objectId]) {
        socket.emit("getDependenciesResponse", {
          objectType: objectType,
          id: objectId,
          projectId: projectId
        });
      } else {
        if($scope.dependencies[projectId][objectId] !== undefined) {
          delete $scope.dependencies[projectId][objectId];
        }


        if($scope.dependencies[projectId][objectId] !== undefined) {
          if($scope.dependencies[projectId][objectId].DataProviders !== undefined) {
            for(var i = 0, dataProvidersLength = $scope.dependencies[projectId][objectId].DataProviders.length; i < dataProvidersLength; i++) {
              $scope.projectsCheckboxes[result.projectId].dataProviders[result.data.DataProviders[i]] = true;
            }
          }

          if($scope.dependencies[projectId][objectId].DataSeries !== undefined) {
            for(var i = 0, dataSeriesLength = result.data.DataSeries.length; i < dataSeriesLength; i++) {
              $scope.projectsCheckboxes[result.projectId].dataSeries[result.data.DataSeries[i]] = true;
            }
          }

          if($scope.dependencies[projectId][objectId].DataSeriesStatic !== undefined) {
            for(var i = 0, dataSeriesStaticLength = result.data.DataSeriesStatic.length; i < dataSeriesStaticLength; i++) {
              $scope.projectsCheckboxes[result.projectId].dataSeriesStatic[result.data.DataSeriesStatic[i]] = true;
            }
          }
        }
      }
    };

    $scope.iconProperties = {};

    $scope.loading = true;

    if (config.message) {
      MessageBoxService.success(title, config.message);
    }

    $scope.close = function() { MessageBoxService.reset() };

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

      if(result.data.DataProviders !== undefined) {
        for(var i = 0, dataProvidersLength = result.data.DataProviders.length; i < dataProvidersLength; i++) {
          $scope.projectsCheckboxes[result.projectId].dataProviders[result.data.DataProviders[i]] = true;
        }
      }

      if(result.data.DataSeries !== undefined) {
        for(var i = 0, dataSeriesLength = result.data.DataSeries.length; i < dataSeriesLength; i++) {
          $scope.projectsCheckboxes[result.projectId].dataSeries[result.data.DataSeries[i]] = true;
        }
      }

      if(result.data.DataSeriesStatic !== undefined) {
        for(var i = 0, dataSeriesStaticLength = result.data.DataSeriesStatic.length; i < dataSeriesStaticLength; i++) {
          $scope.projectsCheckboxes[result.projectId].dataSeriesStatic[result.data.DataSeriesStatic[i]] = true;
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
            if($scope.projectsCheckboxes[element.id].dataProviders != undefined) {
              for(var j = 0, dataProvidersLength = $scope.dataProviders[element.id].length; j < dataProvidersLength; j++) {
                if($scope.projectsCheckboxes[element.id].dataProviders[$scope.dataProviders[element.id][j].id])
                  $scope.exportData.DataProviders.push($scope.dataProviders[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].dataSeries != undefined) {
              for(var j = 0, dataSeriesLength = $scope.dataSeries[element.id].length; j < dataSeriesLength; j++) {
                if($scope.projectsCheckboxes[element.id].dataSeries[$scope.dataSeries[element.id][j].id]) {
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

            if($scope.projectsCheckboxes[element.id].dataSeriesStatic != undefined) {
              for(var j = 0, dataSeriesStaticLength = $scope.dataSeriesStatic[element.id].length; j < dataSeriesStaticLength; j++) {
                if($scope.projectsCheckboxes[element.id].dataSeriesStatic[$scope.dataSeriesStatic[element.id][j].id]) {
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

            if($scope.projectsCheckboxes[element.id].analysis != undefined) {
              for(var j = 0, analysisLength = $scope.analysis[element.id].length; j < analysisLength; j++) {
                if($scope.projectsCheckboxes[element.id].analysis[$scope.analysis[element.id][j].id])
                  $scope.exportData.Analysis.push($scope.analysis[element.id][j]);
              }
            }

            if($scope.projectsCheckboxes[element.id].views != undefined) {
              for(var j = 0, viewsLength = $scope.views[element.id].length; j < viewsLength; j++) {
                if($scope.projectsCheckboxes[element.id].views[$scope.views[element.id][j].id])
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

          if($scope.projectsCheckboxes[project.id].dataProviders == undefined)
            $scope.projectsCheckboxes[project.id].dataProviders = {};

          for(var j = 0, dataProvidersLength = dataProviders.length; j < dataProvidersLength; j++) {
            $scope.projectsCheckboxes[project.id].dataProviders[dataProviders[j].id] = true;
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

            if($scope.projectsCheckboxes[project.id].dataSeries == undefined)
              $scope.projectsCheckboxes[project.id].dataSeries = {};

            for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
              $scope.projectsCheckboxes[project.id].dataSeries[dataSeries.data[j].id] = true;
            }
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

            if($scope.projectsCheckboxes[project.id].dataSeriesStatic == undefined)
              $scope.projectsCheckboxes[project.id].dataSeriesStatic = {};

            for(var j = 0, dataSeriesLength = dataSeries.data.length; j < dataSeriesLength; j++) {
              $scope.projectsCheckboxes[project.id].dataSeriesStatic[dataSeries.data[j].id] = true;
            }
          }, function(err) {
            console.log("Err in retrieving data series static");
          }
        ).finally(function() {
          $scope.loading = false;
        });

        AnalysisService.init({ project_id: project.id }).then(function(analysis) {
          $scope.analysis[project.id] = analysis;

          if($scope.projectsCheckboxes[project.id].analysis == undefined)
            $scope.projectsCheckboxes[project.id].analysis = {};

          for(var j = 0, analysisLength = analysis.length; j < analysisLength; j++) {
            $scope.projectsCheckboxes[project.id].analysis[analysis[j].id] = true;
          }
        }).catch(function(err) {
          $log.info("Err in retrieving Analysis " + err);
        }).finally(function() {
          $scope.loading = false;
        });

        $http.get("/api/View", {}).success(function(views) {
          $scope.views[project.id] = views;

          if($scope.projectsCheckboxes[project.id].views == undefined)
            $scope.projectsCheckboxes[project.id].views = {};

          for(var j = 0, viewsLength = views.length; j < viewsLength; j++) {
            $scope.projectsCheckboxes[project.id].views[views[j].id] = true;
          }
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