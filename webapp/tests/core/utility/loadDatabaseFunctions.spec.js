const expect = require('chai').expect;
const fs = require('fs');
const sinon = require('sinon');

// TerraMA2
const mockPromise = require('./mocks/mockPromise');
const { Connection } = require('./../../../core/utility/connection');
const loadDatabaseFunctions = require('./../../../core/utility/loadDatabaseFunctions');

describe('loadDatabaseFunctions module test', () => {
  let connectionStub = null;
  let executeStub = null;
  const fakeURI = 'postgres://localhost/db';

  beforeEach(() => {
    connectionStub = sinon.stub(Connection.prototype, 'connect').returns(Promise.resolve());
    executeStub = sinon.stub(Connection.prototype, 'execute').returns(Promise.resolve());
  });

  afterEach(() => {
    connectionStub.restore();
    executeStub.restore();
  })

  it('loads all SQL functions into database successfully', async () => {
    const promise = mockPromise(loadDatabaseFunctions(fakeURI));

    await promise;

    expect(promise.isFulfilled()).equals(true);
  });

  it('throws Error when directory sql-functions does not exists', async () => {
    const expectedError = new Error(`Directory not found`);
    expectedError.code = 'ENOENT';

    const fsStub = sinon.stub(fs, 'readdirSync').throws(expectedError);

    try {
      await loadDatabaseFunctions(fakeURI);
    } catch (err) {
      expect(err).to.equals(expectedError);
      expect(err.code).equals(expectedError.code);
    } finally {
      fsStub.restore();
    }
  });

  it('throws Error on no directory permission', async () => {
    const expectedError = new Error(`Permission denied`);
    expectedError.code = 'EACCES';

    const fsStub = sinon.stub(fs, 'readdirSync').throws(expectedError);

    try {
      await loadDatabaseFunctions(fakeURI);
    } catch (err) {
      expect(err).to.equals(expectedError);
      expect(err.code).equals(expectedError.code);
    } finally {
      fsStub.restore();
    }
  });

  it('throws Error when SQL file is invalid', async () => {
    const expectedError = new Error(`Invalid SQL`);

    executeStub.onCall(0).returns(Promise.reject(expectedError));

    const fsStub = sinon.stub(fs, 'readFileSync').returns(`CREATE TABE fakeTable ( id INT )`);

    try {
      await loadDatabaseFunctions(fakeURI);
    } catch (err) {
      expect(err).to.equals(expectedError);
      expect(err.code).equals(expectedError.code);
    } finally {
      fsStub.restore();
    }
  });
});