'use strict';

angular.module("terrama2.services", ['terrama2'])
  .factory("DataProviderFactory", ["$http", function($http) {
    return {
      get: function() {
        return $http.get("/api/DataProvider", {});
      }
    };
  }])

  .factory("DataSeriesSemanticsFactory", ["$http", function($http) {
    var url = "/api/DataSeriesSemantics/";
    return {
      get: function(semanticsName, extra) {
        var data = extra instanceof Object ? extra : {};
        return $http({
          method: 'GET',
          url: url + semanticsName,
          params: data
        });
      },

      list: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      }
    };
  }])

  .factory("DataSeriesFactory", ["$http", "$HttpSync", function($http, $HttpSync) {
    var url = "/api/DataSeries";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(dataSeriesObject) {
        return $http.post(url, dataSeriesObject);
      },

      put: function(dataSeriesId, dataSeriesObject) {
        return $http.put(url + "/" + dataSeriesId, dataSeriesObject);
      }
    };
  }])

  .factory("ServiceInstanceFactory", ["$http", function($http) {
    var url = "/api/Service";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(serviceObject) {
        return $http.post(url, serviceObject);
      },

      put: function(serviceId, serviceObject) {
        return $http.put(url + "/" + serviceId, serviceObject);
      }
    };
  }])

  .factory("AnalysisFactory", ["$http", function($http) {
    var url = "/api/Analysis";
    return {
      get: function(extra) {
        return $http({
          method: 'GET',
          url: url,
          params: extra instanceof Object ? extra : {}
        });
      },

      post: function(serviceObject) {
        return $http.post(url, serviceObject);
      },

      put: function(analysisId, serviceObject) {
        return $http.put(url + "/" + analysisId, serviceObject);
      }
    };
  }]).

  factory("Socket", function($rootScope) {
    var socket = io.connect(window.location.origin, {
      reconnect: false // it avoids to socket io reconnect automatically.
    });

    return {
      on: function(name, callback) {
        socket.on(name, function() {
          var args = arguments;
          $rootScope.$apply(function() {
            callback.apply(socket, args);
          });
        });
      },

      emit: function (eventName, data, callback) {
        socket.emit(eventName, data, function () {
          var args = arguments;
          $rootScope.$apply(function () {
            if (callback) {
              callback.apply(socket, args);
            }
          });
        });
      },

      once: function(eventName, callback) {
        socket.once(eventName, function() {
          var args = arguments;
          $rootScope.$apply(function() {
            callback.apply(socket, args);
          });
        });
      }
    };
  })
  .factory("UserFactory", function($http) {
    var url = "/api/users/";
    return {
      get: function(userId, restriction) {
        var destination = url;
        if (userId) {
          destination += userId;
        }
        return $http({
          method: 'GET',
          url: destination,
          params: restriction instanceof Object ? restriction : {}
        });
      },
      put: function(userId, userObject) {
        return $http.put(url + userId, userObject || {});
      }
    };
  })

  .factory("UniqueNumber", function() {
    function generator() {
      this.length = 5;
      this.timestamp =+ new Date();

      var _getRandomInt = function( min, max ) {
        return Math.floor(Math.random() * ( max - min + 1 )) + min;
      }

      this.generate = function() {
        var ts = this.timestamp.toString();
        var parts = ts.split( "" ).reverse();
        var id = "";

        for( var i = 0; i < this.length; ++i ) {
          var index = _getRandomInt( 0, parts.length - 1 );
          id += parts[index];
        }

        return id;
      }
    }
    return function() {
      var unique = new generator();
      return unique.generate();
    };
  })

  .factory("ProjectFactory", function() {
    var url = "/api/Project/"
    return {
      delete: function(projectId) {
        $http.delete(url+projectId+"/delete", {});
      }
    }
  })

  .factory('FileDialog', [function(){
    var callDialog = function(dialog, callback) {
      dialog.addEventListener('change', function(event) {
        var result = dialog.value;

        if (dialog.files && dialog.files.length > 0) {
          // reader.readAsText(dialog.files[0]);
          return callback(null, dialog);
        } else {
          return callback(new Error("No file"));
        }
      }, false);
      dialog.click();
    };

    var dialogs = {};

    dialogs.saveAs = function(callback, defaultFilename, acceptTypes) {
      var dialog = document.createElement('input');
      dialog.type = 'file';
      dialog.nwsaveas = defaultFilename || '';
      if (angular.isArray(acceptTypes)) {
        dialog.accept = acceptTypes.join(',');
      } else if (angular.isString(acceptTypes)) {
        dialog.accept = acceptTypes;
      }
      callDialog(dialog, callback);
    };

    dialogs.openFile = function(callback, multiple, acceptTypes) {
      var dialog = document.createElement('input');
      dialog.type = 'file';
      if (multiple === true) {
        dialog.multiple = 'multiple';
      }
      if (angular.isArray(acceptTypes)) {
        dialog.accept = acceptTypes.join(',');
      } else if (angular.isString(acceptTypes)) {
        dialog.accept = acceptTypes;
      }
      return callDialog(dialog, callback);
    };

    dialogs.openDir = function(callback) {
      var dialog = document.createElement('input');
      dialog.type = 'file';
      dialog.nwdirectory = 'nwdirectory';
      callDialog(dialog, callback);
    };

    dialogs.readAsJSON = function(fileBlob, callback) {
      if (!fileBlob) {
        return callback(new Error("Invalid file"));
      }
      var reader = new FileReader();
      reader.onload = function(file) {
        try {
          callback(null, JSON.parse(reader.result));
        } catch (e) {
          callback(new Error("Invalid configuration file: " + e.toString()));
        }
      }

      reader.readAsText(fileBlob);
    };
    /**
     * It reads XML file loaded from dialog
     *
     * @param {Blob} fileBlob - A javascript blob with file result
     * @param {Function<Error|string>} callback - A callback function to handle reader async.
     */
    dialogs.readAsXML = function(fileBlob, callback) {
      if (!fileBlob) {
        return callback(new Error("Invalid file"));
      }

      var reader = new FileReader();
      reader.onload = function(file) {
        // detecting dom parser
        if (window.DOMParser) {
          parser = new DOMParser();
          xmlDoc = parser.parseFromString(reader.result, "text/xml");
        } else {
          xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
          xmlDoc.async = false;
          xmlDoc.loadXML(reader.result);
        }
      };
    };

    return dialogs;
  }])

  /**
   * It saves a javascript object as JSON file.
   *
   * @throws {Error} When data is an invalid JSON
   * @param {Object} data - A javascript object to be saved
   * @param {string} fileName - A file name output
   */
  .factory("SaveAs", function() {
    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    return function (data, fileName) {
      var json = JSON.stringify(data, null, 2);

      var blob = new Blob([json], {type: "application/json"});
      var url  = URL.createObjectURL(blob);

      a.download    = fileName;
      a.href        = url;
      a.click();
    };
  });
