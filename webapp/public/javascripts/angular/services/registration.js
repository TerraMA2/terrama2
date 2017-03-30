define(function() {

/**
 * It handles TerraMA² Service Registration and Service Update
 *
 * @class RegisterUpdate
 */
function RegisterUpdate($scope, $window, Service, MessageBoxService, Socket, i18n, $q, URIParser) {
  var self = this;
  /**
   * It defines a TerraMA² box styles
   * @type {Object}
   */
  self.css = {'boxType': 'box-solid'};

  /**
   * It defines MessageBox object handling
   * @type {MessageBoxService}
   */
  self.MessageBoxService = MessageBoxService;

  /**
   * It defines a service model. If there global service, so adapts to Update mode. Otherwise, use Register Mode
   * @type {Object}
   */
  self.service = {sshPort: 22, pathToBinary: "terrama2_service"};

  /**
   * It defines a log instance model. It tries to get values from service model. If there is, update mode
   */
  self.log = {port: 5432};

  /**
   * It defines a Maps Object URI (View Services only)
   * @type {Object}
   */
  self.mapsServer = {};

  /**
   * It forces active (on/off) in Active service
   *
   * @param {boolean} state - Initial state
   */
  self.initService = function(state) {
    self.service = self.service || {};

    if (self.update && self.service.host && self.service.host !== "") {
      self.service.isLocal = false;
    } else {
      self.service.isLocal = state;
    }
  };

  /**
   * It defines a connection validation of Database
   *
   * @type {Object}
   */
  self.db = {};
  /**
   * It defines a connection validation of SSH
   *
   * @type {Object}
   */
  self.ssh = {};

  // Initializing Async services.
  $q
    .all([
      i18n.ensureLocaleIsLoaded(),
      Service.init()
    ])
    // on success, bind all functions
    .then(function() {
      /**
       * It defines a global config
       * @type {Object}
       */
      var config = $window.configuration;

      /**
       * Flag to handle update and save mode
       *
       * @type {boolean}
       */
      self.update = config.service.name ? true : false;

      /**
       * It handles when user type maps server URI (for View Services). It parses the content and tries to re-fill other fields
       * with port, user and password.
       */
      self.onMapsServerURIChange = function() {
        if (!self.mapsServer.address) {
          return;
        }
        var targetURI = (!self.mapsServer.address.startsWith("http:") && !self.mapsServer.address.startsWith("https:")) ? "http://" + self.mapsServer.address : self.mapsServer.address;
        var uriObject = URIParser(targetURI);
        if (uriObject && uriObject.length !== 0) {
          self.mapsServer.address = uriObject.protocol + "//" + uriObject.hostname + uriObject.pathname;
          self.mapsServer.port = parseInt(uriObject.port === "" ? "80" : uriObject.port) || self.mapsServer.port ||8080;
          self.mapsServer.user = uriObject.username || self.mapsServer.user;
          self.mapsServer.password = uriObject.password || self.mapsServer.password;
        }
      };

      /**
       * It fills out GUI interface with Service model, log and maps server parameters
       * Used in "Update" mode
       */
      var fillGUI = function() {
        self.service = angular.merge(self.service, config.service);

        if (self.service.service_type_id) {
          self.service.service_type_id = self.service.service_type_id.toString();
        }

        self.metadata = self.service.metadata;
        switch(parseInt(self.service.service_type_id)) {
          case Service.types.VIEW:
            self.mapsServer.address = self.service.metadata.mapsServer;
            self.onMapsServerURIChange();
            break;
          case Service.types.ALERT:
            var emailURI = "http" + self.metadata.emailServer.substring(4, self.metadata.emailServer.length);
            
            var parsed = URIParser(emailURI);
            self.metadata.emailServer = {
              host: parsed.hostname,
              port: parseInt(parsed.port),
              username: parsed.username,
              pass: parsed.password
            };
            break;
        }

        self.log = self.service.log;
      };

      /**
       * Flag to handle connection validation
       * @type {boolean}
       */
      self.isCheckingConnection = false;

      /**
       * It defines a list of cached services
       * 
       * @type {Service[]}
       */
      self.services = Service.list();

      /**
       * Internationalization module
       * @type {Object}
       */
      self.i18n = i18n;

      if (self.update) {
        fillGUI();
      }

      // Defining default threads number
      if (!self.service.numberOfThreads) {
        self.service.numberOfThreads = 0;
      }

      /**
       * Flag to determines service form submit (save/update)
       * 
       * @type {boolean}
       */
      self.isChecking = false;

      /**
       * It just reset message box panel in order to hide
       */
      self.close = function() {
        MessageBoxService.reset();
      };

      self.extraProperties = {};

      /**
       * Cache list. Used to auto-complete database during registration based in other services
       */
      self.config = {
        /**
         * It defines a cached list of databases from other services. Used to auto-complete
         * @type {string[]}
         */
        availableDatabases: []
      };

      self.isAlertService = function isAlertService() {
        return self.service && parseInt(self.service.service_type_id) === Service.types.ALERT;
      };

      /**
       * Listener for handling service port check
       * 
       * @param {Object} response
       * @param {Object?} response.error - An error occurred
       * @param {Object} response.port - A port value ok
       */ 
      Socket.on('suggestPortNumberResponse', function(response) {
        if (response.error) {
          console.log("ERROR");
        } else {
          console.log("OK");
          self.service.port = response.port;
        }
      });

      // Getting all service instance to suggest database names
      if (!self.update) {
        var ports = [];
        self.services.forEach(function(service) {
          self.config.availableDatabases.push(Object.assign({name: service.name}, service.log));
          if (self.service.id !== service.id) {
            ports.push(service.port);
          }
        });

        Socket.emit('suggestPortNumber', {ports: ports, host: self.service.host});
      }

      /**
       * Watcher for handling Service Type change. It just fill path to binary if there is not setTimeout
       * 
       * @param {any} value - Current value
       */ 
      $scope.$watch('ctrl.service.service_type_id', function(value) {
        if (!value) {
          return;
        }

        switch(parseInt(value)) {
          case Service.types.COLLECTOR: // collect
          case Service.types.ANALYSIS: // analysis
          case Service.types.VIEW:
            self.service.pathToBinary = self.service.pathToBinary || "terrama2_service";
            break;
        }
      });

      Socket.on('testDbConnectionResponse', function(result) {
        self.db.isLoading = false;
        if (result.error) {
          self.db.isValid = false;
          self.db.message = result.message;
        } else {
          self.db.isValid = true;
          self.db.message = "";
        }
      });

      Socket.on('testSSHConnectionResponse', function(result) {
        self.ssh.isLoading = false;
        if (result.error) {
          self.ssh.isValid = false;
          self.ssh.message = result.message;
        } else {
          self.ssh.isValid = true;
        }
      });
      var initializing = true;

      /**
       * Watcher responsible for detecting checking connection.
       * It is important due the alert box should display error/success state when all of those are same. For example,
       * if DB is OK but SSH not, a error state must be displayed. Same from SSH. Now, for intercepts both SSH and DB, a function
       * is responsible to compare the values.
       */
      $scope.$watch(function() {
        return self.ssh.isValid && self.db.isValid;
      }, function(value) {
        if (initializing) {
          initializing = false;
          return;
        }
        if (value === true) {
          MessageBoxService.success(i18n.__("Connection"), "");
        } else if (value === false) {
          MessageBoxService.danger(i18n.__("Connection"), "");
        }
      });

      /**
       * It checks remote connection of:
       * - SSH (path to file)
       * - DB Log
       * 
       * @returns {void}
       */
      self.checkConnection = function() {
        if (!Socket) {
          return;
        }

        self.isCheckingConnection = true;

        self.ssh = {
          isLoading: true
        };

        self.db = {
          isLoading: true
        };

        setTimeout(function() {
          // SSH
          Socket.emit('testSSHConnectionRequest',
            {
              host: self.service.host,
              port: self.service.sshPort,
              username: self.service.sshUser,
              isLocal: self.service.isLocal,
              pathToBinary: self.service.pathToBinary
            }
          );

          var logCredentials = Object.assign({}, self.log);
          if (logCredentials && (logCredentials.host && (logCredentials.host === "localhost" || logCredentials.host.startsWith("127.")))) {
            logCredentials.host = self.service.host || logCredentials.host;
          }

          Socket.emit('testDbConnection', logCredentials);
        }, 500);
      };

      self.processMetadata = function processMetadata(value) {
        var output = undefined;
        if (value && angular.isObject(value)) {
          var parsedURI = URIParser(value);
          output = parsedURI.href;
        }
        return output;
      }

      /**
       * Hidden save. It performs update or insert service instance
       * 
       * @returns {void}
       */
      self._save = function() {
        self.isChecking = true;
        var request;

        var copyMetadata = angular.merge({}, self.metadata);
        // Processing Metadata (Email Server)
        switch (parseInt(self.service.service_type_id)) {
          case Service.types.ALERT:
            copyMetadata.emailServer = self.processMetadata(copyMetadata.emailServer);
            break;
          case Service.types.VIEW:
            var copyMapsServer = angular.merge({}, self.mapsServer);
            var mapsServerURI = URIParser(copyMapsServer.address);
            mapsServerURI.port = copyMapsServer.port;
            mapsServerURI.username = copyMapsServer.user;
            mapsServerURI.password = copyMapsServer.password;
            copyMetadata.mapsServer = mapsServerURI.href;
            break;
        }

        self.service.metadata = copyMetadata;

        if (self.update) {
          request = Service.update(self.service.id, {
            service: self.service,
            log: self.log
          });
        }
        else {
          request = Service.create({
            service: self.service,
            log: self.log
          });
        }

        request
          .then(function(data) {
            $window.location.href = "/administration/services?token=" + data.token+"&service="+data.service + "&restart="+data.restart;
          })
          .catch(function(err) {
            MessageBoxService.danger(i18n.__("Service Registration"), err.message);
          })
          .finally(function() {
            self.isChecking = false;
          });
      };

      /**
       * It handles Test port number response. Used after saving. On valid validation, it 
       * save/update service. If a error found, it notifies user. 
       * 
       * @param {Object} response - Response object
       * @param {boolean} response.error - Determines if error occurred
       */
      Socket.on('testPortNumberResponse', function(response) {
        if (response.error) {
          if (config.service.port !== self.service.port) {
            MessageBoxService.danger(i18n.__("Service Registration"), response.message);
            return;
          }
        }

        // continue save process

        // checking port number with a registered service
        for(var i = 0; i < self.services.length; ++i) {
          var service = self.services[i];

          if (service.id !== self.service.id && service.port === self.service.port) {
            MessageBoxService.warning(i18n.__("Service Registration"),
                                      i18n.__("There is already a service registered in same port ") + service.port + " (" + service.name + ")");
            self.extraProperties.confirmButtonFn = self._save;
            self.extraProperties.object = {};
            return;
          }
        }

        self._save();
      });

      /**
       * It handles TerraMA² Service Save button. It emits form validation and wait for socket response in order to notify user about port values
       */
      self.save = function() {
        $scope.$broadcast('formFieldValidation');

        if ($scope.serviceForm.$invalid || $scope.logForm.$invalid) {
          return;
        }

        // validating enviroment
        if (self.service.runEnviroment) {
          self.service.runEnviroment = self.service.runEnviroment.split("\n").join(" ");
        }

        if (self.service.isLocal) {
          self.service.host = "";
        }

        // if it is a view service, validate maps server parameters
        if (self.service.service_type_id == Service.types.VIEW) {
          if ($scope.mapsServerForm.$invalid) {
            return;
          }

          var uriObject = URIParser(self.mapsServer.address);
          uriObject.username = self.mapsServer.user;
          uriObject.password = self.mapsServer.password;
          uriObject.port = self.mapsServer.port;
          self.service.maps_server_uri = uriObject.href;
        } else {
          delete self.service.maps_server_uri;
        }

        // testing port number
        Socket.emit('testPortNumber', {port: self.service.port});
      };
    })
    // on error
    .catch(function(err) {
      MessageBoxService.danger(i18n.__("Service"), 
                               i18n.__("An error occurred during TerraMA² Service Page loading." + err.toString()));
    });
}

  RegisterUpdate.$inject = [
    "$scope", 
    "$window",
    "Service",
    "MessageBoxService",
    "Socket",
    "i18n",
    "$q",
    "URIParser"
  ];

  return RegisterUpdate;

})
