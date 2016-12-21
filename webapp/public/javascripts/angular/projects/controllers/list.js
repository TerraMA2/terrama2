define(function() {
  /**
   * TerraMA² Controller responsible for project listing.
   * 
   * @class ListController
   */
  function ListController($scope, $http, Socket, FileDialog, SaveAs, $log, i18n, $window, MessageBoxService) {
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

    $scope.remove = function(object) {
      return "/api/Project/" + object.id + "/delete";
    };

    $scope.link = function(object) {
      return "/configuration/projects/" + object.name + "/activate";
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

      SaveAs(result.data, result.data.Projects[0].name + ".terrama2");
    });

    socket.on("importResponse", function(result) {
      $scope.loading = false;
      $scope.extra.isImporting = false;
      if (result.err) {
        MessageBoxService.danger(title, result.err);
        return;
      }

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

      MessageBoxService.success(title, msg);
    });

    // callback after remove operation
    $scope.extra = {
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

      export: function(element) {
        if (!element) {
          return;
        }
        $scope.extra.isExporting = true;
        socket.emit("export", {"Projects": [element]});
      },

      import: function() {
        var importTitle = i18n.__("Data Import");
        $scope.extra.isImporting = false;

        var setError = function(err) {
          MessageBoxService.danger(importTitle, err.toString());
          $scope.extra.isImporting = false;
        } 

        FileDialog.openFile(function(err, input) {
          if (err) {
            MessageBoxService.danger(title, err.toString());
            return;
          }

          $scope.loading = true;
          FileDialog.readAsJSON(input.files[0], function(error, json) {
            // applying angular scope..
            $scope.$apply(function() {
              $scope.extra.isImporting = true;
              if (error) {
                setError(error);
                return;
              }

              if (!json.hasOwnProperty("Projects") || 
                  !json.hasOwnProperty("DataSeries") || 
                  !json.hasOwnProperty("DataProviders") ||
                  !json.hasOwnProperty("Analysis")) {
                setError(new Error("Invalid configuration file"));
                return;
              }
              socket.emit("import", json);
            });
          });
        }, false, ".terrama2, .json, application/json");
      }
    };

    $http.get("/api/Project/", {}).success(function(projects) {
      $scope.model = projects;
    }).error(function(err) {
      console.log("Err in retrieving projects");
    }).finally(function() {
      $scope.loading = false;
    });
  }

  ListController.$inject = ["$scope", "$http", "Socket", "FileDialog", "SaveAs", "$log", "i18n", "$window", "MessageBoxService"];

  return ListController;
})