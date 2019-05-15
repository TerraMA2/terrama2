const { ConnectionError } = require('../../../../core/utility/connection');

const resultSet = {
  rows: [
    { fake_column: 1 }
  ],
  rowCount: 1
}

const clientMockSuccess = {
  query(sql, callback) {
    callback(null, resultSet)
  },

  end() {

  }
}

const clientMockError = {
  query(sql, callback) {
    callback()
  },

  end() {
    throw new Error('Unknown Error');
  }
}

const clientQueryMockError = {
  query(sql, callback) {
    callback(new Error('QueryError'));
  },

  end() {
  }
}

const doneMock = () => {};


module.exports = { clientMockSuccess, clientMockError, clientQueryMockError, doneMock };