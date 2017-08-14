'use strict';

define(
  [],
  function() {

    var memberSocket;
    var memberWebAppSocket;
    var memberWebMonitorSocketCallbackExecuted = false;
    var memberWebAppSocketCallbackExecuted = false;

    var getSocket = function() {
      return memberSocket;
    };

    var getWebAppSocket = function() {
      return memberWebAppSocket;
    };

    var init = function(webMonitorSocketCallback, webAppSocketCallback) {
      if(webmonitorHostInfo && webmonitorHostInfo.basePath) {
        memberSocket = io.connect(window.location.origin, {
          path: webmonitorHostInfo.basePath + 'socket.io'
        });
      } else {
        memberSocket = io.connect(":36001");
      }

      if(webadminHostInfo && webadminHostInfo.host && webadminHostInfo.port && webadminHostInfo.basePath) {
        memberWebAppSocket = io.connect(webadminHostInfo.host + ":" + webadminHostInfo.port, {
          path: webadminHostInfo.basePath + 'socket.io'
        });
      } else {
        memberWebAppSocket = io.connect(":36000");
      }

      if(webMonitorSocketCallback) {
        memberSocket.on('connect', function() {
          if(!memberWebMonitorSocketCallbackExecuted) {
            webMonitorSocketCallback();
            memberWebMonitorSocketCallbackExecuted = true;
          }
        });
      }

      if(webAppSocketCallback) {
        memberWebAppSocket.on('connect', function() {
          if(!memberWebAppSocketCallbackExecuted) {
            webAppSocketCallback();
            memberWebAppSocketCallbackExecuted = true;
          }
        });
      }
    };

    return {
      init: init,
      getSocket: getSocket,
      getWebAppSocket: getWebAppSocket
    };
  }
);