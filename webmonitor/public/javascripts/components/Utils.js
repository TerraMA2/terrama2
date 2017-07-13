'use strict';

define(
  [],
  function() {
    
		var memberSocket;
		var memberWebAppSocket;

    var getSocket = function(){
      return memberSocket;
    }

    var getWebAppSocket = function(){
      return memberWebAppSocket;
    }

    var init = function(){

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

    }

    return {
      init: init,
      getSocket: getSocket,
      getWebAppSocket: getWebAppSocket
    }
  }
)