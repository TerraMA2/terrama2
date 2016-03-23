module.exports = {
  clone: function(object) {
    return Object.assign({}, object);
  },

  handleRequestError: function(response, err, code) {
    response.status(code);
    response.json({status: code, message: err.message});
  }
};