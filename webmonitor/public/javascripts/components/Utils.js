'use strict';

define(
  ['TerraMA2WebComponents'],
  function(TerraMA2WebComponents) {
    
		var socket;
		var webAppSocket;

    var getSocket = function(){
      return socket;
    }

    var getWebAppSocket = function(){
      return webAppSocket;
    }

    var init = function(){

			if(webmonitorHostInfo && webmonitorHostInfo.basePath) {
				socket = io.connect(window.location.origin, {
					path: webmonitorHostInfo.basePath + 'socket.io'
				});
			} else {
				socket = io.connect(":36001");
			}

			if(webadminHostInfo && webadminHostInfo.host && webadminHostInfo.port && webadminHostInfo.basePath) {
				webAppSocket = io.connect(webadminHostInfo.host + ":" + webadminHostInfo.port, {
					path: webadminHostInfo.basePath + 'socket.io'
				});
			} else {
				webAppSocket = io.connect(":36000");
			}

    }

    return {
      init: init,
      getSocket: getSocket,
      getWebAppSocket: getWebAppSocket
    }
  }
)