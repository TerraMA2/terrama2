define([], function() {
  var moduleName = "terrama2.providers.directives";

  angular.module(moduleName, [])
    .run(["$templateCache", function($templateCache) {
      $templateCache.put(
        "directoryExplorer.html",
        "<div class=\"modal fade\" id=\"filesExplorerModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"filesExplorerModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"holder-div\" ng-show=\"loadingFiles\"></div>" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the Folder') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div ng-show=\"!dirs.children || dirs.children.length == 0\">{{ i18n.__('No folders to show.') }}</div>" +
                "<script type=\"text/ng-template\" id=\"files-explorer.html\">" +
                  "<div class=\"name-div\" ng-click=\"setFolderStatus(dir.fullPath)\" ng-class=\"{ 'selected-folder': dir.fullPath == selectedFolder }\">" +
                    "<i class=\"fa fa-folder\" ng-show=\"pathLoading != dir.fullPath && !dir.childrenVisible\"></i>" +
                    "<i class=\"fa fa-folder-open\" ng-show=\"pathLoading != dir.fullPath && dir.childrenVisible\"></i>" +
                    "<img ng-show=\"pathLoading == dir.fullPath\" src=\"" + BASE_URL + "images/loader.gif\">" +
                    "{{ dir.name }}" +
                  "</div>" +
                  "<ul class=\"file-explorer-ul\" ng-if=\"dir.children && dir.children.length > 0 && dir.childrenVisible\">" +
                    "<li ng-repeat=\"dir in dir.children\" ng-include=\"'files-explorer.html'\"></li>" +
                  "</ul>" +
                "</script>" +
                "<ul class=\"file-explorer-ul\">" +
                  "<li ng-repeat=\"dir in dirs.children\" ng-include=\"'files-explorer.html'\"></li>" +
                "</ul>" +
              "</div>" +
              "<div class=\"modal-footer\">" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\">{{ i18n.__('Cancel') }} </button>" +
                "<button type=\"button\" class=\"btn btn-primary\" data-dismiss=\"modal\" ng-click=\"selectPath()\">{{ i18n.__('Select') }}</button>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>" +
        "<div class=\"modal modal-danger fade\" id=\"filesExplorerErrorModal\">" +
          "<div class=\"modal-dialog\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\">" +
                  "<span aria-hidden=\"true\">×</span></button>" +
                "<h4 class=\"modal-title\">{{ i18n.__('Data Server Registration') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<p></p>" +
              "</div>" +
              "<div class=\"modal-footer\">" +
                "<button type=\"button\" class=\"btn btn-outline pull-left\" data-dismiss=\"modal\">{{ i18n.__('Close') }}</button>" +
              "</div>" +
            "</div>" +
          "</div>" +
        "</div>"
      );
    }])
    .directive('terrama2DirectoryExplorer', function() {
      return {
        restrict: 'EA',
        templateUrl: 'directoryExplorer.html',
        controller: ['$scope', 'i18n', 'MessageBoxService', '$q', '$http', function($scope, i18n, MessageBoxService, $q, $http) {
          $scope.dirs = {
            children: []
          };

          $scope.selectedFolder = null;
          $scope.loadingFiles = false;
          $scope.pathLoading = null;
          // The base path of the tree
          $scope.baseAddress = '/';
          // The current address, divided per folder in format of array, ignoring the base address
          $scope.currentPath = [];

          // Validates the connection data, tries to connect and in case of success, it lists all the folder presents in the path
          $scope.openFileExplorer = function(form) {
            $scope.$broadcast("schemaFormValidate");

            if(form && !$scope.isValidDataProviderTypeForm(form))
              return;

            $scope.loadingFiles = false;
            $scope.pathLoading = null;

            $scope.isChecking = true;
            $scope.currentPath = $scope.model['pathname'].split('/').filter(function(a) { return a != '' });

            listFolders($scope.baseAddress).then(function(data) {
              if(data.error) {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), data.error);
              }

              $scope.dirs.childrenVisible = true;
              $scope.dirs.children = data.folders;

              if($scope.currentPath.length === 0) {
                $scope.isChecking = false;
                $('#filesExplorerModal').modal();
              } else {
                navigateToFolder($scope.currentPath, $scope.baseAddress, [$scope.currentPath[0]], 0).then(function() {
                  $scope.isChecking = false;
                  $('#filesExplorerModal').modal();
                }).catch(function(err) {
                  $scope.isChecking = false;
                  MessageBoxService.danger(i18n.__(title), err.error);
                });
              }
            }).catch(function(err) {
              $scope.isChecking = false;
              return MessageBoxService.danger(i18n.__(title), err.error);
            });
          };

          // Selects the path of the current selected folder
          $scope.selectPath = function() {
            if($scope.selectedFolder)
              $scope.model['pathname'] = $scope.selectedFolder;
          };

          // Sets the status of a clicked folder
          $scope.setFolderStatus = function(path) {
            var tempPath = path;
            var pathItems = tempPath.split('/');
            pathItems = pathItems.filter(function(a) { return a != '' });

            var lastFolder = getLastFolder($scope.dirs, pathItems, 0);

            if(lastFolder.childrenVisible) {
              lastFolder.childrenVisible = false;

              if($scope.selectedFolder == lastFolder.fullPath)
                $scope.selectedFolder = null;
            } else {
              $scope.loadingFiles = true;
              $scope.pathLoading = path;

              listFolders(path).then(function(data) {
                if(data.error)
                  return MessageBoxService.danger(i18n.__(title), data.error);

                $scope.selectedFolder = path;

                lastFolder.childrenVisible = true;
                lastFolder.children = data.folders;

                $scope.loadingFiles = false;
                $scope.pathLoading = null;
              }).catch(function(err) {
                $scope.loadingFiles = false;
                $scope.pathLoading = null;
                $('#filesExplorerErrorModal .modal-body > p').text(err.error);
                $('#filesExplorerErrorModal').modal();
              });
            }
          };

          // Lists the folders of a given path
          var listFolders = function(pathToList) {
            var promiser = $q.defer();
            var timeOut = $q.defer();

            var params = angular.copy($scope.model);

            params.protocol = $scope.dataProvider.protocol;
            params.list = true;
            params.pathname = pathToList;

            $scope.timeOutSeconds = 8;

            var expired = false;

            setTimeout(function() {
              expired = true;
              timeOut.resolve();
            }, 1000 * $scope.timeOutSeconds);

            var request = $http({
              method: "POST",
              url: BASE_URL + "uri/",
              data: params,
              timeout: timeOut.promise
            });

            request.then(function(data) {
              if(data.message)
                return promiser.reject({ error: i18n.__(data.message) });
              else
                return promiser.resolve({ error: null, folders: data.data.data.list });
            }).catch(function(err) {
              if(expired)
                return promiser.reject({ error: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds.") });
              else
                return promiser.reject({ error: i18n.__(err.data.message) });
            });

            return promiser.promise;
          };

          // Opens all the folders present in a given path
          var navigateToFolder = function(paths, parent, currentChildren, i) {
            var promiser = $q.defer();

            parent = (parent == '/' ? '' : parent);

            if(i < paths.length) {
              return listFolders(parent + '/' + paths[i]).then(function(data) {
                if(data.error)
                  return promiser.reject(data.error);

                var lastFolder = getLastFolder($scope.dirs, currentChildren, 0);

                lastFolder.childrenVisible = true;
                lastFolder.children = data.folders;

                if((i + 1) < paths.length)
                  currentChildren.push(paths[(i + 1)]);
                else if((i + 1) === paths.length)
                  $scope.selectedFolder = parent + '/' + paths[i];

                return navigateToFolder(paths, (parent + '/' + paths[i]), currentChildren, (i + 1));
              }).catch(function(err) {
                return promiser.reject(err.error);
              });
            } else {
              return promiser.resolve();
            }

            return promiser.promise;
          };

          // Returns the last folder in the path (Important! For this function to work, all the folders in the path, except the last, must have its folders children populated)
          var getLastFolder = function(item, pathFolders, i) {
            if(i < pathFolders.length) {
              for(var j = 0, childrenLength = item.children.length; j < childrenLength; j++) {
                if(item.children[j].name == pathFolders[i]) {
                  return getLastFolder(item.children[j], pathFolders, (i + 1));
                }
              }
            } else {
              return item;
            }
          };
        }]
      };
    });

  return moduleName;
});