'use strict';

define(
  ['components/Layers', 'components/Utils'],
  function(Layers, Utils) {

    var signin = function(error, username) {
      if(!$('#authentication-div').hasClass('hidden'))
        $('#authentication-div').addClass('hidden');

      $("#username").val("");
      $("#password").val("");

      if(error !== null) {
        $("#terrama2Alert > p > strong").text('');
        $("#terrama2Alert > p > span").text(error);
        $("#terrama2Alert").removeClass('hide');
      } else {
        if($("#loginButton > button > i").hasClass("fa-user-times"))
          $("#loginButton > button > i").removeClass("fa-user-times");

        if($("#loginButton > button > i").hasClass("font-red"))
          $("#loginButton > button > i").removeClass("font-red");

        if(!$("#loginButton > button > i").hasClass("fa-user"))
          $("#loginButton > button > i").addClass("fa-user");

        if(!$("#loginButton > button > i").hasClass("font-green"))
          $("#loginButton > button > i").addClass("font-green");

        $("#user-div .user-name").text(username);

        if(!$("#login-div").hasClass("hidden"))
          $("#login-div").addClass("hidden");

        if($("#user-div").hasClass("hidden"))
          $("#user-div").removeClass("hidden");

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

      if($("#loginButton > button > i").hasClass("fa-user"))
        $("#loginButton > button > i").removeClass("fa-user");

      if($("#loginButton > button > i").hasClass("font-green"))
        $("#loginButton > button > i").removeClass("font-green");

      if(!$("#loginButton > button > i").hasClass("fa-user-times"))
        $("#loginButton > button > i").addClass("fa-user-times");

      if(!$("#loginButton > button > i").hasClass("font-red"))
        $("#loginButton > button > i").addClass("font-red");

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