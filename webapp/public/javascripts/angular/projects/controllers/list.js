define(function() {
  /**
   * TerraMA² Controller responsible for project listing.
   * 
   * @class ListController
   */
  function ListController($scope, $http, Socket, FileDialog, SaveAs, $log, i18n, $window) {
    $scope.model = [];
    var config = $window.configuration;
    var socket = Socket;
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

    // alert-box
    $scope.alertLevel = "alert-success";
    $scope.alertBox = {
      title: i18n.__(config.context || "Project"),
      message: config.message
    };
    $scope.display = config.message ? true : false;
    $scope.resetState = function() { $scope.display = false; };

    socket.on("exportResponse", function(result) {
      $scope.extra.isExporting = false;
      $scope.display = result.err ? true : false;
      if (result.err) {
        $scope.alertLevel = "alert-danger";
        $scope.alertBox.message = result.err;
        console.log("Err ", result.err);
        return;
      }

      SaveAs(result.data, result.data.Projects[0].name + ".terrama2");
    });

    socket.on("importResponse", function(result) {
      $scope.loading = false;
      $scope.display = true;
      $scope.extra.isImporting = false;
      if (result.err) {
        $scope.alertLevel = "alert-danger";
        console.log("Err ", result.err);
        $scope.alertBox.message = result.err;
        return;
      }
      $scope.alertLevel = "alert-success";

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

      $scope.alertBox.message = msg;
    });

    // callback after remove operation
    $scope.extra = {
      removeOperationCallback: function(err, data) {
        $scope.display = true;
        if (err) {
          $scope.alertLevel = "alert-danger";
          $scope.alertBox.message = err.message;
          return;
        }

        $scope.alertLevel = "alert-success";
        $scope.alertBox.message = data.name + " removed";
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
        $scope.alertBox.title = i18n.__("Data Import");
        $scope.display = false;
        $scope.extra.isImporting = false;

        var setError = function(err) {
          $scope.display = true;
          $scope.alertLevel = "alert-danger";
          $scope.alertBox.message = err.toString();
          $scope.extra.isImporting = false;
        } 

        FileDialog.openFile(function(err, input) {
          if (err) {
            $scope.display = true;
            $scope.alertBox.message = err.toString();
            return;
          }

          $scope.loading = true;
          FileDialog.readAsJSON(input.files[0], function(error, json) {
            // applying angular scope..
            $scope.$apply(function() {
              $scope.extra.isImporting = true;
              if (error) {
                setError(error);
                console.log(error);
                return;
              } else {
                $scope.display = false;
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

  ListController.$inject = ["$scope", "$http", "Socket", "FileDialog", "SaveAs", "$log", "i18n", "$window"];

  return ListController;
})