'use strict';

define(
  ['components/Layers', 'components/Utils'],
  function(Layers, Utils) {
    var loadEvents = function() {
      $('#signin').on('click', function() {
        $.post(BASE_URL + "login", {
          username: $("#username").val(),
          password: $("#password").val()
        }, function(data) {
          if(data.error !== null) {
            $("#terrama2Alert > p > strong").text('');
            $("#terrama2Alert > p > span").text(data.error);
            $("#terrama2Alert").removeClass('hide');
          } else {
            $("#user-nav .user-name").text(data.username);

            if(!$("#login-nav").hasClass("hidden"))
              $("#login-nav").addClass("hidden");

            if($("#user-nav").hasClass("hidden"))
              $("#user-nav").removeClass("hidden");

            Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, onlyPrivate: true });
          }
        });
      });

      $('#signout').on('click', function() {
        $.get(BASE_URL + "logout", function(data) {
          if(data.error !== null) {
            console.log(data.error);
          } else {
            Layers.removePrivateLayers();

            if(!$("#user-nav").hasClass("hidden"))
              $("#user-nav").addClass("hidden");

            if($("#login-nav").hasClass("hidden"))
              $("#login-nav").removeClass("hidden");

            $("#user-nav .user-name").text("");
          }
        });
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      init: init
    };
  }
);