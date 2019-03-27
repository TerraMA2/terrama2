const expect = require('chai').expect;
const sinon = require('sinon');
const pg = require('pg');

// TerraMA2
const { Connection, ConnectionError, NoConnectionError, QueryError } = require('./../../../core/utility/connection');
const { clientMockSuccess, clientQueryMockError, clientMockError, doneMock } = require('./mocks/connection');

describe('Connection test', () => {
  describe('#connect', () => {
    let pgConnectionStub = null;
    let uri = "postgresql://fakeUser:fakePassword@fakeHost:fakePort/fakeDB";
    const conn = new Connection(uri);

    afterEach(async () => {
      await conn.disconnect();
    })

    it('connects successfully from postgres', async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientMockSuccess, doneMock);
      await conn.connect();
      pgConnectionStub.restore();

      expect(conn.isConnected()).equals(true);

      await conn.disconnect();

      expect(conn.isConnected()).equals(false);
    });

    it('does not throw exception when connection already opened', async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientMockSuccess, doneMock);
      await conn.connect();

      expect(conn.isConnected()).equals(true);
      await conn.connect();
      expect(conn.isConnected()).equals(true);

      pgConnectionStub.restore();
    })

    it('throws ConnectionError', async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(new ConnectionError('Error in connection'));

      try {
        await conn.connect();

        throw new Error('Invalid mock');
      } catch (err) {
        expect(err).to.be.instanceOf(ConnectionError);
        expect(conn.isConnected()).equals(false);
        expect(err.message).equals('Could not connect to Postgres due Error in connection');
      } finally {
        await conn.disconnect();
        pgConnectionStub.restore();
      }

    });
  });

  describe('#disconnect', () => {
    let pgConnectionStub = null;
    let uri = "postgresql://fakeUser:fakePassword@fakeHost:fakePort/fakeDB";
    const conn = new Connection(uri);

    it('disconnects successfully from postgres', async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientMockSuccess, doneMock);
      await conn.connect();
      await conn.disconnect();

      expect(conn.isConnected()).equals(false);
      expect(conn.getClient()).equals(null);
      pgConnectionStub.restore();
    });

    it('must not throw Error when no active connection', async () => {
      await conn.disconnect();

      expect(conn.isConnected()).equals(false);
      expect(conn.getClient()).equals(null);
    });

    it('throw Error when PostgreSQL hang connection', async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientMockError, doneMock);

      try {
        await conn.disconnect();
      } catch (err) {
        expect(err).to.be.instanceOf(Error);
        expect(err.message).equals('Error during disconnection. Unknown Error');
      } finally {
        pgConnectionStub.restore();
      }
    });
  });

  describe('#execute', () => {
    let pgConnectionStub = null;
    let uri = "postgresql://fakeUser:fakePassword@fakeHost:fakePort/fakeDB";
    const conn = new Connection(uri);

    const connectStub = async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientMockSuccess, doneMock);
      await conn.connect();
    };

    const connectWithInvalidClientStub = async () => {
      pgConnectionStub = sinon.stub(pg, 'connect').yields(null, clientQueryMockError, doneMock);
      await conn.connect();
    };

    const disconnectStub = async () => {
      pgConnectionStub.restore();
      await conn.disconnect();
    };

    it('executes query in database', async () => {
      connectStub();

      const resultSet = await conn.execute('SELECT 1 AS fake_column');

      disconnectStub();

      expect(resultSet.rowCount).equals(1);
      expect(resultSet.rows[0].fake_column).equals(1);
    });

    it('throws QueryError', async () => {
      connectWithInvalidClientStub();

      try {
        await conn.execute('SELECT 1 AS fake_column');

        throw new Error('Oops');
      } catch (err) {
        expect(err).to.be.instanceOf(QueryError);

      } finally {
        disconnectStub();
      }
    });

    it('throws NoConnectionError', async () => {
      try {
        await conn.execute('SELECT 1 AS fake_column');

        throw new Error('Oops');
      } catch (err) {
        expect(err).to.be.instanceOf(NoConnectionError);
      }
    });
  });
});