'use strict';

define(
  [],
  function() {

    var memberSocket;
    var memberWebAppSocket;
    var memberWebMonitorSocketCallbackExecuted = false;
    var memberWebAppSocketCallbackExecuted = false;
    var memberCurrentLanguage = null;
    var memberTableLanguage = null;
    var memberAnimatedLayerLanguage = null;

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

    var translate = function(element) {
      $(element).localize();

      if($('#table-div').css('display') !== 'none' && memberCurrentLanguage !== memberTableLanguage) {
        memberTableLanguage = memberCurrentLanguage;
        $("#attributes-table-select").trigger("setAttributesTable");
      }

      if((!$("#layer-toolbox").hasClass("hidden") || (!$("#animate-tools").hasClass("hidden") && $("#pauseAnimation").hasClass("hidden"))) && memberCurrentLanguage !== memberAnimatedLayerLanguage) {
        memberAnimatedLayerLanguage = memberCurrentLanguage;
        $.event.trigger({type: "setDatesCalendar"});
      }
    };

    var setTagContent = function(element, content, property) {
      $(element).attr("data-i18n", (property ? "[" + property + "]" : "") + content);
      translate(element);
    };

    var getTranslatedString = function(string) {
      $("#translation-div").attr("data-i18n", string);
      translate("#translation-div");

      return $("#translation-div").text();
    };

    var changeLanguage = function(newLanguage) {
      memberCurrentLanguage = newLanguage;
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
      translate: translate,
      setTagContent: setTagContent,
      getTranslatedString: getTranslatedString,
      changeLanguage: changeLanguage,
      getSocket: getSocket,
      getWebAppSocket: getWebAppSocket,
      orderByProperty: orderByProperty
    };
  }
);