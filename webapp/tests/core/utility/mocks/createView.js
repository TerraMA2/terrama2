const resultSetLimitedToOne = {
  rows: [
    { fake_column: 1 }
  ],
  rowCount: 1
};

const resultSet = {
  rows: [
    { fake_column1: 1, fake_column2: 2 },
    { fake_column1: 2, fake_column2: 4 },
    { fake_column1: 3, fake_column2: 6 },
  ],
  rowCount: 3
};

const resultSetWithOneAttribute = {
  rows: [
    { fake_column: 1, },
    { fake_column: 2, },
    { fake_column: 3, },
  ],
  rowCount: 3
}

module.exports = { resultSet, resultSetLimitedToOne, resultSetWithOneAttribute };