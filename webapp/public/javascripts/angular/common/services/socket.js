define([],
  function() {
    /**
     * It defines 
     */
    function Socket($rootScope, $window) {
      var socket = io.connect($window.location.origin, {
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
    }

    Socket.$inject = ["$rootScope", "$window"];

    return Socket;
  }
)