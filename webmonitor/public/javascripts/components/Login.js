'use strict';

define(
  ['components/Layers', 'components/Utils', 'components/State'],
  function(Layers, Utils, State) {

    var signin = function(error, username) {
      if(!$('#authentication-div').hasClass('hidden'))
        $('#authentication-div').addClass('hidden');

      $("#username").val("");
      $("#password").val("");

      if(error !== null) {
        $("#terrama2Alert > p > strong").text('');
        Utils.setTagContent("#terrama2Alert > p > span", error);
        $("#terrama2Alert").removeClass('hide');
      } else {
        if(!$("#loginButton .fa-times").hasClass("hidden"))
          $("#loginButton .fa-times").addClass("hidden");

        if($("#loginButton .fa-circle").hasClass("hidden"))
          $("#loginButton .fa-circle").removeClass("hidden");

        $("#user-div .user-name").text(username);

        if(!$("#login-div").hasClass("hidden"))
          $("#login-div").addClass("hidden");

        if($("#user-div").hasClass("hidden"))
          $("#user-div").removeClass("hidden");

        State.verifyState();
        Utils.getSocket().emit('retrieveViews', { clientId: Utils.getWebAppSocket().id, onlyPrivate: true });
      }
    };

    var signout = function() {
      Layers.removePrivateLayers();

      if(!$('#authentication-div').hasClass('hidden'))
        $('#authentication-div').addClass('hidden');

      if(!$("#user-div").hasClass("hidden"))
        $("#user-div").addClass("hidden");

      if($("#login-div").hasClass("hidden"))
        $("#login-div").removeClass("hidden");

      if(!$("#loginButton .fa-circle").hasClass("hidden"))
        $("#loginButton .fa-circle").addClass("hidden");

      if($("#loginButton .fa-times").hasClass("hidden"))
        $("#loginButton .fa-times").removeClass("hidden");

      $("#user-div .user-name").text("");
    };

    var loadEvents = function() {
      $('#signin').on('click', function() {
        $.post(BASE_URL + "login", {
          username: $("#username").val(),
          password: $("#password").val()
        }, function(data) {
          signin(data.error, data.username);
        });
      });

      $('#signout').on('click', function() {
        $.get(BASE_URL + "logout", function(data) {
          signout();
        });
      });
    };

    var init = function() {
      loadEvents();
    };

    return {
      signin: signin,
      signout: signout,
      init: init
    };
  }
);