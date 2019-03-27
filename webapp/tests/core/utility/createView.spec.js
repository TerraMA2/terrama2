const expect = require('chai').expect;
const sinon = require('sinon');

// TerraMA2
const { createView, /*destroyView, validateView,*/ } = require('./../../../core/utility/createView');
const { Connection } = require('./../../../core/utility/connection');
const { resultSet, resultSetLimitedToOne, resultSetWithOneAttribute } = require('./mocks/createView');

describe('createView module test', () => {
  describe('#createView', () => {
    let connectionStub = null;
    const fakeURI = 'postgres://localhost/db';
    const fakeViewName = 'fakeView';
    const fakeTableName = 'fakeTable';

    beforeEach(() => {
      connectionStub = sinon.stub(Connection.prototype, 'connect').returns(Promise.resolve());
    });

    afterEach(() => {
      connectionStub.restore();
    })

    it('must create view in database from entire table', async () => {
      const queryStub = sinon.stub(Connection.prototype, 'execute').returns(Promise.resolve(resultSet));

      const res = await createView(fakeURI, fakeViewName, fakeTableName);

      expect(res.rowCount).equals(3);
      expect(res.rows[0]).haveOwnPropertyDescriptor('fake_column1');

      queryStub.restore();
    });

    it('must create view with one attribute', async () => {
      const queryStub = sinon.stub(Connection.prototype, 'execute').returns(Promise.resolve(resultSetWithOneAttribute));

      const fakeAttributes = ["fake_column"];
      const res = await createView(fakeURI, fakeViewName, fakeTableName, fakeAttributes);

      expect(res.rowCount).equals(3);

      for(const row of res.rows) {
        expect(row).haveOwnPropertyDescriptor('fake_column');
      }

      queryStub.restore();
    })

    it('must create view limiting result to 1', async () => {
      const queryStub = sinon.stub(Connection.prototype, 'execute').returns(Promise.resolve(resultSetLimitedToOne));

      const res = await createView(fakeURI, fakeViewName, fakeTableName, [], '1 = 1 LIMIT 1');

      expect(res.rowCount).equals(1);
      expect(res.rows[0]).haveOwnPropertyDescriptor('fake_column');
      expect(res.rows[0].fake_column).equals(1);

      queryStub.restore();
    })

    // it('must not create view due invalid attribute', async () => {

    // })

    it('must not create view due where condition is invalid', async () => {
      const queryStub = sinon.stub(Connection.prototype, 'execute').returns(Promise.reject(new Error(`Invalid query`)));


      try {
        await createView(fakeURI, fakeViewName, fakeTableName, [], `fake condition`);

        throw 'Error';
      } catch (err) {
        expect(err).to.be.instanceOf(Error);
        expect(err.message).equals('Invalid query');
      }

      queryStub.restore();
    })
  });
});