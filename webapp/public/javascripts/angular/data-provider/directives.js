define([], function() {
  var moduleName = "terrama2.providers.directives";

  angular.module(moduleName, [])
    .run(["$templateCache", function($templateCache) {
      $templateCache.put(
        "directoryExplorer.html",
        "<div class=\"modal fade\" id=\"filesExplorerModal\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"filesExplorerModalLabel\">" +
          "<div class=\"modal-dialog modal-md\" role=\"document\">" +
            "<div class=\"modal-content\">" +
              "<div class=\"holder-div\" ng-show=\"loadingDirectories\"></div>" +
              "<div class=\"modal-header\">" +
                "<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Close\"><span aria-hidden=\"true\">&times;</span></button>" +
                "<h4 class=\"modal-title\" id=\"dataSeriesModalLabel\">{{ i18n.__('Select the Directory') }}</h4>" +
              "</div>" +
              "<div class=\"modal-body\">" +
                "<div ng-show=\"!rootDirectories.children || rootDirectories.children.length == 0\">{{ i18n.__('No directories to show.') }}</div>" +
                "<script type=\"text/ng-template\" id=\"files-explorer.html\">" +
                  "<div class=\"name-div\" ng-click=\"setDirectoryStatus(directory.fullPath)\" ng-class=\"{ 'selected-directory': directory.fullPath == selectedDirectory }\">" +
                    "<i class=\"fa fa-folder\" ng-show=\"pathLoading != directory.fullPath && !directory.childrenVisible\"></i>" +
                    "<i class=\"fa fa-folder-open\" ng-show=\"pathLoading != directory.fullPath && directory.childrenVisible\"></i>" +
                    "<img ng-show=\"pathLoading == directory.fullPath\" src=\"" + BASE_URL + "images/loader.gif\">" +
                    "{{ directory.name }}" +
                  "</div>" +
                  "<ul class=\"file-explorer-ul\" ng-if=\"directory.children && directory.children.length > 0 && directory.childrenVisible\">" +
                    "<li ng-repeat=\"directory in directory.children\" ng-include=\"'files-explorer.html'\"></li>" +
                  "</ul>" +
                "</script>" +
                "<ul class=\"file-explorer-ul\">" +
                  "<li ng-repeat=\"directory in rootDirectories.children\" ng-include=\"'files-explorer.html'\"></li>" +
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
          // Title of the message boxes
          var title = "Data Server Registration";
          // Object that contains the root directories
          $scope.rootDirectories = {
            children: []
          };
          // Variable that contains the path of the selected directory
          $scope.selectedDirectory = null;
          // Flag that indicates if there are directories being loaded
          $scope.loadingDirectories = false;
          // Variable that contains the path of the directory being loaded
          $scope.pathLoading = null;
          // Variable that contains the path of the root directory
          $scope.basePath = '/';
          // Variable that contains the current path, divided per directory in format of array, ignoring the base path
          $scope.currentPath = [];

          /**
           * Validates the connection data, tries to connect and in case of success, it lists all the directories present in the path.
           * @param {object} form - Form to be validated
           *
           * @function openFileExplorer
           */
          $scope.openFileExplorer = function(form) {
            $scope.$broadcast("schemaFormValidate");

            if(form && !$scope.isValidDataProviderTypeForm(form))
              return;

            $scope.loadingDirectories = false;
            $scope.pathLoading = null;

            $scope.isChecking = true;
            $scope.currentPath = $scope.model['pathname'].split('/').filter(function(a) { return a != '' });

            listDirectories($scope.basePath).then(function(data) {
              if(data.error) {
                $scope.isChecking = false;
                return MessageBoxService.danger(i18n.__(title), data.error);
              }

              $scope.rootDirectories.childrenVisible = true;
              $scope.rootDirectories.children = data.directories;

              if($scope.currentPath.length === 0) {
                $scope.isChecking = false;
                $('#filesExplorerModal').modal();
              } else {
                navigateToDirectory($scope.currentPath, $scope.basePath, [$scope.currentPath[0]], 0).then(function(lele) {
                  $scope.isChecking = false;
                  $('#filesExplorerModal').modal();
                }).catch(function(err) {
                  $scope.isChecking = false;
                  MessageBoxService.danger(i18n.__(title), err.message);
                });
              }
            }).catch(function(err) {
              $scope.isChecking = false;
              return MessageBoxService.danger(i18n.__(title), err.error);
            });
          };

          /**
           * Selects the path of the current selected directory.
           *
           * @function selectPath
           */
          $scope.selectPath = function() {
            if($scope.selectedDirectory)
              $scope.model['pathname'] = $scope.selectedDirectory;
          };

          /**
           * Sets the status of a clicked directory.
           * @param {string} path - Path of the directory
           *
           * @function setDirectoryStatus
           */
          $scope.setDirectoryStatus = function(path) {
            var tempPath = path;
            var pathItems = tempPath.split('/');
            pathItems = pathItems.filter(function(a) { return a != '' });

            var lastDirectory = getLastDirectory($scope.rootDirectories, pathItems, 0);

            if(lastDirectory.childrenVisible) {
              lastDirectory.childrenVisible = false;

              if($scope.selectedDirectory == lastDirectory.fullPath)
                $scope.selectedDirectory = null;
            } else {
              $scope.loadingDirectories = true;
              $scope.pathLoading = path;

              listDirectories(path).then(function(data) {
                if(data.error)
                  return MessageBoxService.danger(i18n.__(title), data.error);

                $scope.selectedDirectory = path;

                lastDirectory.childrenVisible = true;
                lastDirectory.children = data.directories;

                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
              }).catch(function(err) {
                $scope.loadingDirectories = false;
                $scope.pathLoading = null;
                $('#filesExplorerErrorModal .modal-body > p').text(err.error);
                $('#filesExplorerErrorModal').modal();
              });
            }
          };

          /**
           * Lists the directories of a given path.
           * @param {string} pathToList - Path to be listed
           * @returns {angular.IPromise<Object[]>} promiser.promise - Promise
           *
           * @private
           * @function listDirectories
           */
          var listDirectories = function(pathToList) {
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
                return promiser.resolve({ error: null, directories: data.data.data.list });
            }).catch(function(err) {
              if(expired)
                return promiser.reject({ error: i18n.__("Timeout: Request took longer than ") + $scope.timeOutSeconds + i18n.__(" seconds.") });
              else
                return promiser.reject({ error: i18n.__(err.data.message) });
            });

            return promiser.promise;
          };

          /**
           * Opens all the directories present in a given path.
           * @param {array} paths - Path to be listed, divided per directory in format of array
           * @param {string} parent - Path of the parent directory
           * @param {array} currentChildren - Array containing the directories already listed
           * @param {integer} i - Counter
           * @returns {angular.IPromise<Object[]>} promiser.promise - Promise
           *
           * @private
           * @function navigateToDirectory
           */
          var navigateToDirectory = function(paths, parent, currentChildren, i) {
            var promiser = $q.defer();

            parent = (parent == '/' ? '' : parent);

            if(i < paths.length) {
              return listDirectories(parent + '/' + paths[i]).then(function(data) {
                if(data.error)
                  throw new Error((err.error ? err.error : err.message));

                var lastDirectory = getLastDirectory($scope.rootDirectories, currentChildren, 0);

                lastDirectory.childrenVisible = true;
                lastDirectory.children = data.directories;

                if((i + 1) < paths.length)
                  currentChildren.push(paths[(i + 1)]);
                else if((i + 1) === paths.length)
                  $scope.selectedDirectory = parent + '/' + paths[i];

                return navigateToDirectory(paths, (parent + '/' + paths[i]), currentChildren, (i + 1));
              }).catch(function(err) {
                throw new Error((err.error ? err.error : err.message));
              });
            } else {
              return promiser.resolve();
            }

            return promiser.promise;
          };

          /**
           * Returns the last directory in the path (Important! For this function to work, all the directories in the path, except the last, must have its children directories populated).
           * @param {object} item - Object containing the directory explorer
           * @param {array} pathDirectories - Path to be searched, divided per directory in format of array
           * @param {integer} i - Counter
           * @returns {object} item - Last directory object
           *
           * @private
           * @function getLastDirectory
           */
          var getLastDirectory = function(item, pathDirectories, i) {
            if(i < pathDirectories.length) {
              for(var j = 0, childrenLength = item.children.length; j < childrenLength; j++) {
                if(item.children[j].name == pathDirectories[i]) {
                  return getLastDirectory(item.children[j], pathDirectories, (i + 1));
                }
              }
            } else {
              return item;
            }
          };

          // new

          var getBasePathByPermissions = function(directories) {
            var promiser = $q.defer();
            var promises = [];
            var originalDirectories = directories;

            for(var i = (directories.length - 1); i >= 0; i--) {
              promises.push(
                listDirectories('/' + directories.join('/')).then(function(data) {
                  if(data.error) {
                    if(true) {
                      return promiser.resolve(data);
                    } else {
                      return promiser.reject(data.error);
                    }
                  }
                }).catch(function(err) {
                  if(true) {
                    return promiser.resolve(data);
                  } else {
                    return promiser.reject(data.error);
                  }
                })
              );

              directories.splice(i, 1);
            }

            $q.all(promises).then(function() {
              return promiser.resolve('/' + originalDirectories.join('/'));
            });

            return promiser.promise;
          };

          // new
        }]
      };
    });

  return moduleName;
});