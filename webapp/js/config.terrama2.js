var Config = function() {

  var confJsonHTML = null;
  var confJsonComponents = null;

  this.loadConfigurations = function() {
    $.ajax({
      url: "/terrama2/webapp/config/html.json",
      dataType: 'json',
      async: false,
      success: function(data) {
        confJsonHTML = data;
      }
    });

    $.ajax({
      url: "/terrama2/webapp/config/components.json",
      dataType: 'json',
      async: false,
      success: function(data) {
        confJsonComponents = data;
      }
    });
  }

  this.getConfJsonHTML = function() {
    return confJsonHTML;
  }

  this.getConfJsonComponents = function() {
    return confJsonComponents;
  }

  $(document).ready(function(){
    $('.date').mask('00/00/0000');
  });
}
