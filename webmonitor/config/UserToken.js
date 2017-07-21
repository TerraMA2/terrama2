var token = null;

var setToken = function(newToken) {
  token = newToken;
};

var getToken = function() {
  return token;
};

module.exports = {
  setToken: setToken,
  getToken: getToken
};