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

    var orderByProperty = function(array, property, order) {
      function compare(a, b) {
        if(a[property] < b[property]) return (order == "desc" ? 1 : -1);
        if(a[property] > b[property]) return (order == "desc" ? -1 : 1);
        return 0;
      };

      return array.sort(compare);
    };

    var setTagContent = function(element, content, property) {
      $(element).attr("data-i18n", (property ? "[" + property + "]" : "") + content);
      $(element).localize();
    };

    var getTranslatedString = function(string) {
      $("#translation-div").text(string);
      $("#translation-div").localize();

      return $("#translation-div").text();
    };

    var init = function(webMonitorSocketCallback, webAppSocketCallback) {
      jQuery.fn.insertAt = function(index, element) {
        var lastIndex = this.children().size();

        if(index < 0)
          index = Math.max(0, lastIndex + 1 + index);

        this.append(element);

        if(index < lastIndex)
          this.children().eq(index).before(this.children().last());

        return this;
      };

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
      setTagContent: setTagContent,
      getTranslatedString: getTranslatedString,
      getSocket: getSocket,
      getWebAppSocket: getWebAppSocket,
      orderByProperty: orderByProperty
    };
  }
);