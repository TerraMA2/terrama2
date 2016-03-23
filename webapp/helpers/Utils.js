module.exports = {
  UrlHandler: function(request, response, objectType, template, objectOutput) {
    var queryType = request.query.type;

    if (queryType != "static" && queryType != "dynamic") {
      response.statusCode = 404;
      // temp: It should return an default 404 template
      response.render("base/404");
    } else {
      response.render(template, Object.assign({kind: objectType, state: queryType}, objectOutput ? objectOutput : {}));
    }
  }
};